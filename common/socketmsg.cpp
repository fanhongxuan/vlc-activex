#include <string.h>
#include <stdio.h>  
#include <sys/types.h> 
#include <stdlib.h>  
#include <errno.h>  
#include <fcntl.h>

#ifndef WIN32
#include <sys/socket.h>  
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <arpa/inet.h>  
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define closesocket(x)  close(x)
#else
#include <Windows.h>
#include <Winsock.h>
#include <time.h> // for time
#include <process.h>
#define snprintf _snprintf
#define read(a,b, c) recv((a),(char*)(b),(c), 0)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef int socklen_t;
typedef size_t ssize_t;
inline void sleep(int sec){
	Sleep(sec * 1000);
}
inline void usleep(int uSec){
	Sleep(uSec / 1000);
}

#pragma comment(lib, "Ws2_32.lib")

const char *inet_ntop(int af, const void *src,
                             char *dst, socklen_t size)
{
	struct in_addr in;
	in = *(struct in_addr *)(src);
	snprintf(dst, size, "%s", inet_ntoa(in));
	return dst;
}
typedef struct{
	char placeHolder;
} pthread_attr_t;

typedef struct{
	char placeHolder;
} pthread_mutexattr_t;

#define pthread_mutex_t HWND
#define pthread_cond_t HWND
#define PTHREAD_MUTEX_INITIALIZER NULL
#define pthread_t DWORD
#define STDCALL __stdcall

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          unsigned int( __stdcall *start_routine) (void *), void *arg)
{
	_beginthreadex(NULL, 0, start_routine, arg, 0, (unsigned int *)thread);
	return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
	return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return 0;
}

pthread_t pthread_self(void)
{
	DWORD id = GetCurrentThreadId();
	return id;
}
#endif

#include "msg.h"

#define USE_TCP 0
#define USE_UDP 1
#define USE_UNIX 2

#define LISTENLEN 10
#define MAX_CLIENT 32
#define MAX_HANDLER 128
#define MAX_CONFIG_ITEM 10
#define MAX_REGISTER 256
#define VALUE_LEN 128
#define NAME_LEN 128
#define MAX_SHORT_CONNECTION 128

#ifndef __cplusplus
typedef char bool;
#define false 0
#define true 1
#endif

typedef enum{
    msg_type_register_req,
    msg_type_register_resp,
    msg_type_unregister_req,
    msg_type_unregister_resp,
    msg_type_user_msg_req,
    msg_type_user_msg_resp,
    msg_type_user_msg_ind,
}msg_type_t;

typedef struct{
    u32 msgId;
}msg_register_req_t;

typedef struct{
    u32 msgId;
}msg_unregister_req_t;

typedef struct{
    u32 msgType;
    u32 msgId;
    u32 msgSize;
    u32 msgSrc;
} msg_header_t;

typedef struct msg_client{
    struct in_addr addr;
    unsigned short port;
    int sock;
    msg_header_t head;
    bool bHeadValid;
    int offset;
    unsigned char *pBuffer;
} msg_client_t;

typedef struct{
    bool bUsed;
    u32 msgId;
    msg_client_t *client;
}msg_register_t;

typedef struct{
    bool bUsed;
    u32 msgId;
    msgSerial hton;
    msgSerial ntoh;
}msg_register_serial_t;

typedef struct{
    u32 id;
    msgHandler handler;
}msg_handler_t;

typedef struct{
    char name[NAME_LEN];
    char value[VALUE_LEN];
}msg_cfg_item_t;

typedef struct{
    u32 id;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    unsigned char *pBuffer;
    u32 buffSize;
    u32 dataSize;
    bool bUsed;
}msg_recv_pending_t;

typedef struct{
    bool bUsed;
    int sock;
    pthread_t threadId;
}msg_short_connection_info_t;

static msg_short_connection_info_t gShortConnections[MAX_SHORT_CONNECTION];
static pthread_mutex_t gShortConnectionsMutex = PTHREAD_MUTEX_INITIALIZER;

static msg_client_t gClients[MAX_CLIENT];
static msg_handler_t gHandlers[MAX_HANDLER];
static msg_cfg_item_t gConfigMaps[MAX_CONFIG_ITEM];
static bool gIsServer = false;

static bool gbInit = false;
/**
 * note:fanhongxuan@gmail.com
 * change the default value of gServerIp to INADDR_ANY will cause all the client
 * wait the broadcast of the address.
 */
/* static u32 gServerIp = INADDR_ANY; */
static u32 gServerIp = 0x7F000001; /* 127.0.0.1 */
static unsigned short gPort = 8000;
static unsigned char gProtocol = USE_TCP;
static u32 gRecvTimeout = 2000; // default is 2 seconds.

static int gServerSocket = 0;
static pthread_t gServerThread = 0;
static pthread_t gServerBroadcastThread = 0;
static bool gServerRunning = false;

static msg_register_t gServerRegister[MAX_REGISTER];

static msg_register_serial_t gSerialRegister[MAX_REGISTER];


static int gClientSocket = 0;
static pthread_t gClientThread = 0;
static bool gClientRunning = false;

static bool do_msg_init();
static bool do_msg_exit();
static int handle_unregister_all_req(msg_client_t *pClient);
static int close_client_socket(int sock);
static void wait_for_server_broadcast();

static int do_create_server_socket(unsigned char protocol, unsigned short port)
{
    struct sockaddr_in	servaddr;
 
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0){
        printf("failed to set reuseaddr\n");
    }
    /**
     * note:fanhongxuan@gmail.com
     * not supported on old kerenel
     */
    /* if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const void *)&opt, sizeof(opt)) < 0){ */
    /*     printf("failed to set reuseport\n"); */
    /* } */
    
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
    
    /**
     * note:fanhongxuan@gmail.com
     * always listen all all the iface.
     */
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);
    printf("bind to port:%d\n", port);
	bind(sock, (struct sockaddr*) &servaddr, sizeof(servaddr));
    return sock;
}

static int do_create_client_socket(unsigned char protocol, short port)
{
    struct sockaddr_in servaddr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct in_addr addr;

    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(gServerIp);
	servaddr.sin_port = htons(port);
    addr.s_addr = htonl(gServerIp);
    /* printf("connect to %s:%d\n", inet_ntoa(addr), port); */
	int ret = connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr));
    if (ret != 0){
        printf("Failed to connect to server\n");
        closesocket(sock);
        return -1;
    }
    else{
        printf("connect to %s:%d OK\n", inet_ntoa(addr), port);
    }
    // todo:fanhongxuan@gmail.com
    // set the socket to non-block mode.
    return sock;
}

static int open_client_socket()
{
    // make sure we have receive the server ipaddress.
    if ((!gIsServer) &&
        gServerIp == INADDR_ANY){
        // wait for the server ipaddr.
        wait_for_server_broadcast();
        if (gServerIp == INADDR_ANY){
            return (-2);
        }
    }
    
    pthread_mutex_lock(&gShortConnectionsMutex);
    int i = 0;
    // 1, find the exist short connection by current thread id.
    pthread_t cur = pthread_self();
    for (i = 0; i < MAX_SHORT_CONNECTION; i++){
        if (gShortConnections[i].bUsed == true &&
            pthread_equal(cur, gShortConnections[i].threadId) &&
            gShortConnections[i].sock > 0){
            pthread_mutex_unlock(&gShortConnectionsMutex);
            return gShortConnections[i].sock;
        }
    }
    
    // 2, create a new socket for this thread.
    int ret = do_create_client_socket(gProtocol, gPort);
    if (ret < 0){
        pthread_mutex_unlock(&gShortConnectionsMutex);
        return -1;
    }
    
    // 3, find a free slot to store this.
    for (i = 0; i < MAX_SHORT_CONNECTION; i++){
        if (gShortConnections[i].bUsed == false){
            gShortConnections[i].bUsed = true;
            gShortConnections[i].sock = ret;
            gShortConnections[i].threadId = pthread_self();
            pthread_mutex_unlock(&gShortConnectionsMutex);
                return ret;
        }
    }
    close_client_socket(ret);
    pthread_mutex_unlock(&gShortConnectionsMutex);
    return -1;
}

static int close_client_socket(int sock)
{
    int i = 0;
    pthread_mutex_lock(&gShortConnectionsMutex);
    for (i = 0; i < MAX_SHORT_CONNECTION;i++){
        if (gShortConnections[i].bUsed == true &&
            gShortConnections[i].sock == sock){
            gShortConnections[i].sock = 0;
            gShortConnections[i].bUsed = false;
        }
    }
    pthread_mutex_unlock(&gShortConnectionsMutex);
    closesocket(sock);
    return 0;
}

static msg_client_t *get_client_by_sock(int sock){
    int i = 0;
    for (i = 0; i < MAX_CLIENT; i++){
        if (gClients[i].sock == sock){
            return &gClients[i];
        }
    }
    return NULL;
}

static void remove_client_by_sock(int sock){
    int i = 0;
    for (i = 0; i < MAX_CLIENT; i++){
        if (gClients[i].sock == sock){
            // note:fanhongxuan@gmail.com
            // when socket is closed, make sure all the register is released.
            handle_unregister_all_req(&gClients[i]);
            gClients[i].sock = 0;
            if (NULL != gClients[i].pBuffer){
                free(gClients[i].pBuffer);
                gClients[i].pBuffer = NULL;
            }
        }
    }
}

static msg_client_t *get_free_client(){
    int i = 0;
    for (i = 0; i < MAX_CLIENT; i++){
        if (gClients[i].sock == 0){
            gClients[i].bHeadValid = false;
            gClients[i].pBuffer = NULL;
            gClients[i].offset = 0;
            return &gClients[i];
        }
    }
    return NULL;
}

static u32 msg_sendto(u32 ulMsgId, void *pPayload, u32 ulSize, u32 dstSock, u32 type, u32 srcSock, bool needConvert)
{
    msg_header_t head;
    void *pData = pPayload;
    u32 data_len = ulSize;
    int ret = 0;
    bool convertOk = false;
    msgSerial hton = NULL;
    int i = 0;
	do_msg_init();
    if (needConvert){
        for (i = 0; i < MAX_REGISTER; i++){
            if (gSerialRegister[i].bUsed == true &&
                gSerialRegister[i].msgId == ulMsgId &&
                gSerialRegister[i].hton != NULL){
                hton = gSerialRegister[i].hton;
                break;
            }
        }
    
        if (NULL != hton){
            if (hton(ulMsgId, pPayload, ulSize, &pData, &data_len) == 0){
                convertOk = true;
                /* printf("hton convert ok:(%d:%s)\n", data_len, (char*)pData); */
            }
        }
    }
    head.msgType = htonl(type);
    head.msgId = htonl(ulMsgId);
    head.msgSize = htonl(data_len);
    head.msgSrc = htonl(srcSock);
    int sock = (int)dstSock;
    if (sock <= 0){
        printf("msg_sendto:invalid socket:%d\n", sock);
        ret = -1;
        goto out;
    }
    // send the head
    ret = send(sock, (const char*)&head, sizeof(head), 0);
    if (ret != sizeof(head)){
        printf("msg_sendto:send head failed:%d\n", ret);
        ret = -2;
        goto out;
    }
    
    // send the payload
    ret = send(sock, (const char*)pData, data_len, 0);
    if (ret != data_len){
        printf("msg_sendto:send payload failed:%d\n", ret);
        ret = -1;
        goto out;
    }
    ret = 0;
out:
    if (convertOk && NULL != pData){
        free(pData);
    }
    return ret;
}

static int handle_register_req(msg_client_t *pClient)
{
    msg_register_req_t *pRet = (msg_register_req_t*)(pClient->pBuffer);
    if (pClient->head.msgSize != sizeof(msg_register_req_t)){
        return -1;
    }
    /* printf("Register %u\n", ntohl(pRet->msgId)); */
    int i = 0;
    // first try to replace an exist register.
    for (i = 0; i < MAX_REGISTER; i++){
        if (gServerRegister[i].bUsed && gServerRegister[i].msgId == ntohl(pRet->msgId)){
            gServerRegister[i].client = pClient;
            return 0;
        }
    }
    for (i = 0; i < MAX_REGISTER; i++){
        if (gServerRegister[i].bUsed == false){
            gServerRegister[i].bUsed = true;
            gServerRegister[i].msgId = ntohl(pRet->msgId);
            gServerRegister[i].client = pClient;
            break;
        }
    }
    if (i == MAX_REGISTER){
        printf("Too many register\n");
        return -1;
    }
    return 0;
}

static int handle_unregister_req(msg_client_t *pClient)
{
    msg_unregister_req_t *pRet = (msg_unregister_req_t*)(pClient->pBuffer);
    if (pClient->head.msgSize != sizeof(msg_unregister_req_t)){
        return -1;
    }
    /* printf("Unregister %u\n", ntohl(pRet->msgId)); */
    int i = 0;
    for (i = 0; i < MAX_REGISTER; i++){
        if (gServerRegister[i].bUsed  && gServerRegister[i].msgId == ntohl(pRet->msgId)){
            gServerRegister[i].bUsed = false;
            gServerRegister[i].msgId = 0;
            gServerRegister[i].client = NULL;
            break;
        }
    }
    return 0;
}

static int handle_unregister_all_req(msg_client_t *pClient)
{
    int i = 0;
    for (i = 0; i < MAX_REGISTER; i++){
        if (gServerRegister[i].bUsed && gServerRegister[i].client == pClient){
            gServerRegister[i].bUsed = false;
            gServerRegister[i].client = NULL;
            gServerRegister[i].msgId = 0;
        }
    }
    return 0;
}

static int handle_user_msg_req(msg_client_t *pClient)
{
    if (NULL == pClient){
        return -1;
    }
    /* printf("handle_user_msg_req:%u from:%d\n", pClient->head.msgId, pClient->sock); */
    int i = 0;
    for (i = 0; i < MAX_REGISTER; i++){
        if (gServerRegister[i].bUsed &&
            gServerRegister[i].msgId == pClient->head.msgId &&
            NULL != gServerRegister[i].client &&
            gServerRegister[i].client->sock > 0){
            // forward this msg to the
            /* printf("forward msg:%u to %d\n", pClient->head.msgId, gServerRegister[i].client->sock); */
            msg_sendto(pClient->head.msgId,
                       pClient->pBuffer,
                       pClient->head.msgSize,
                       gServerRegister[i].client->sock,
                       msg_type_user_msg_req,
                       pClient->sock,
                       false);
            break;
        }
    }
    if (i == MAX_REGISTER){
        printf("ERROR: unhandled user req:%d\n", pClient->head.msgId);
    }
    return 0;
}

static int handle_user_msg_resp(msg_client_t *pClient)
{
    if (NULL == pClient){
        return -1;
    }
    /* printf("handle_user_msg_resp:%u\n", pClient->head.msgId); */
    msg_client_t *pTarget = get_client_by_sock(pClient->head.msgSrc);
    if (NULL != pTarget){
        /* printf("forward this resp to the src:%u\n", pClient->head.msgSrc); */
        /* printf("target sock:%d\n", pTarget->sock); */
        msg_sendto(pClient->head.msgId,
                   pClient->pBuffer,
                   pClient->head.msgSize,
                   pTarget->sock,
                   pClient->head.msgType,
                   0,
                   false);
    }
    else{
        
    }
    return 0;
}

static int handle_msg(msg_client_t *pClient)
{
    if (NULL == pClient){
        return -1;
    }
    int ret = 0;
    /* printf("Receive a msg(type:%u,id:%u,len:%u), port:%u\n", */
    /*        pClient->head.msgType, */
    /*        pClient->head.msgId, */
    /*        pClient->head.msgSize, */
    /*        pClient->port); */
    switch(pClient->head.msgType){
    case msg_type_register_req:
        ret = handle_register_req(pClient);
        break;
    case msg_type_register_resp:
        break;
    case msg_type_unregister_req:
        ret = handle_unregister_req(pClient);
        break;
    case msg_type_unregister_resp:
        break;
    case msg_type_user_msg_req:
        ret = handle_user_msg_req(pClient);
        break;
    case msg_type_user_msg_resp:
        ret = handle_user_msg_resp(pClient);
        break;
    case msg_type_user_msg_ind:
        break;
    default:
        printf("Unspported msg type:%u\n", pClient->head.msgType);
        ret = -1;
    }
    return ret;
}

static int handle_client(int sock)
{
    msg_client_t *pClient = get_client_by_sock(sock);
    if (NULL == pClient){
        return -1;
    }
    int ret = 0;
    if (!pClient->bHeadValid){
        ret = read(sock, (char*)(&pClient->head) + pClient->offset,
                   sizeof(pClient->head) - pClient->offset);
        if (ret <= 0){
            char addr[32] = {0};
            inet_ntop(AF_INET, &pClient->addr, addr, 32);
            printf("Read head failed:%d(%s:%d)\n", ret, addr, pClient->port);
            return -1;
        }
        pClient->offset += ret;
        if (pClient->offset == sizeof(pClient->head)){
            pClient->bHeadValid = true;
            pClient->head.msgType = ntohl(pClient->head.msgType);
            pClient->head.msgId = ntohl(pClient->head.msgId);
            pClient->head.msgSize = ntohl(pClient->head.msgSize);
            pClient->head.msgSrc = ntohl(pClient->head.msgSrc);
            pClient->pBuffer = (unsigned char*)realloc(pClient->pBuffer, pClient->head.msgSize);
            pClient->offset = 0;
        }
        return 0;
    }
    else{
        // read the next part of the packet
        int left = pClient->head.msgSize - pClient->offset;
        if (left != 0){
            ret = read(sock, pClient->pBuffer + pClient->offset, left);
            if (ret <= 0){
                char addr[32] = {0};
                inet_ntop(AF_INET, &pClient->addr, addr, 32);
                printf("Read body failed:%d(%s:%d)\n", ret, addr, pClient->port);
                return -1;
            }
            pClient->offset += ret;
        }
    }

    if (pClient->offset == pClient->head.msgSize){
        ret = handle_msg(pClient);
        if (ret < 0){
            char addr[32] = {0};
            inet_ntop(AF_INET, &pClient->addr, addr, 32);
            printf("Handle msg failed:%d(%s:%d)\n", ret, addr, pClient->port);       
        }
        pClient->bHeadValid = false;
        pClient->offset = 0;
    }
    return 0;
}

static void *client_long_connection_handler()
{
    int ret = 0;
    fd_set rset;
    msg_header_t head;
    bool isHeadValid = false;
    unsigned char *pBuffer = NULL;
    u32 offset = 0;
    struct timeval timeout;
    printf("client_long_connection_loop:\n");
    while(gClientRunning){
        FD_ZERO(&rset);
        FD_SET(gClientSocket, &rset);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100 * 1000; // default is 100 ms
        int nready = select(gClientSocket + 1, &rset, NULL, NULL, &timeout);
        /* printf("select return\n"); */
        if (nready > 0 && FD_ISSET(gClientSocket, &rset)){
            // handle it
            if (!isHeadValid){
                // read the head.
                ret = read(gClientSocket, ((unsigned char*)&head) + offset, sizeof(head) - offset);
                if (ret <= 0){
                    // failed to receive the head.
                    printf("client_long_connection_loop:Failed to read head\n");
                    return NULL;
                }
                offset += ret;
                if (offset != sizeof(head)){
                    continue;   
                }
                head.msgSrc = ntohl(head.msgSrc);
                head.msgType = ntohl(head.msgType);
                head.msgSize = ntohl(head.msgSize);
                head.msgId = ntohl(head.msgId);
                // printf("Head id:%u,size:%u\n", head.msgId, head.msgSize);
                pBuffer = (unsigned char *)malloc(head.msgSize);
                if (NULL == pBuffer){
                    printf("client_long_connection_loop:Failed to mallock buffer\n");
                    return NULL;
                }
                isHeadValid = true;
                offset = 0;
            }
            else if (isHeadValid && NULL != pBuffer){
                ret = read(gClientSocket, pBuffer + offset, head.msgSize - offset);
                if (ret <= 0){
                    printf("client_long_connection_loop:Read body failed\n");
                    return NULL;
                }
                offset += ret;
                if (offset == head.msgSize){
                    /* printf("client_long_connection_loop:Receive a (type:%u,id:%u,size:%u)\n", head.msgType, head.msgId, head.msgSize); */
                    if (head.msgType == msg_type_user_msg_req){
                        int i = 0;
                        bool convertOk = false;
                        void *pData = pBuffer;
                        u32 data_len = head.msgSize;
                        // 1, call the registed handler.
                        for (i = 0; i < MAX_HANDLER; i++){
                            if (gHandlers[i].id == head.msgId && NULL != gHandlers[i].handler){
                                /* 1, before call the registed handler, first call convert it */
                                int j = 0;
                                msgSerial ntoh = NULL;
                                for (j = 0; j< MAX_REGISTER; j++){
                                    if (gSerialRegister[j].bUsed == true &&
                                        gSerialRegister[j].msgId == head.msgId &&
                                        gSerialRegister[j].ntoh != NULL){
                                        ntoh = gSerialRegister[j].ntoh;
                                        break;
                                    }
                                }
                                if (NULL != ntoh){
                                    if (ntoh(head.msgId, pBuffer, head.msgSize, &pData, &data_len) == 0){
                                        convertOk = true;
                                    }
                                }
                                gHandlers[i].handler(pData);
                                break;
                            }
                        }
                        /* printf("client_long_connection_loop:Send Response to Server\n"); */
                        // 2, send out the response to the server.
                        msg_sendto(head.msgId,
                                   pData,
                                   head.msgSize,
                                   gClientSocket,
                                   msg_type_user_msg_resp,
                                   head.msgSrc, true);
                        if (convertOk){
                            free(pData);
                        }
                    }                    
                    /* wait for the next msg */
                    isHeadValid = false;
                    offset = 0;
                }
            }
        }
    }
    if (NULL != pBuffer){
        free(pBuffer);
    }
    printf("client_long_connection_loop quit\n");
    return NULL;
}

static unsigned int STDCALL client_long_connection_loop(void *arg)
{
    gClientRunning = true;
    while(gClientRunning){
        if (gClientSocket > 0){
            client_long_connection_handler();
        }
        if (gClientSocket > 0){
            closesocket(gClientSocket);
        }
        gClientSocket = do_create_client_socket(gProtocol, gPort);
        if (gClientSocket > 0){
            /* resend all the register info */
            int i = 0;
            for (i = 0; i < MAX_HANDLER; i++){
                if (gHandlers[i].handler != NULL){
                    msg_register_req_t req;
                    req.msgId = htonl(gHandlers[i].id);
                    msg_sendto(0, &req, sizeof(req), gClientSocket, msg_type_register_req, 0, false);
                }
            }
        }
        else{
            // wait one seconds and retry.
            sleep(1);
        }
    }
    return NULL;
}

#define MCAST_PORT (16800)
#define MCAST_ADDR ("224.0.0.89")
#define MAX_INTERFACE	(16)

extern int aesEncrypt(const uint8_t *key, uint32_t keyLen, const uint8_t *pt, uint8_t *ct, uint32_t len);
extern int aesDecrypt(const uint8_t *key, uint32_t keyLen, const uint8_t *ct, uint8_t *pt, uint32_t len);
static u8 aes_key[16] = {0x12, 0x35, 0x38, 0x19,
                         0x30, 0x1f, 0x2b, 0x05,
                         0x81, 0xa1, 0xbc, 0x8f,
                         0x13, 0x25, 0x4b, 0x13};
static int update_key_by_ip(u32 ip){
    aes_key[1] = (ip) & 0xff;
    aes_key[5] = (ip >> 8) & 0xff;
    aes_key[6] = (ip >> 16) & 0xff;
    aes_key[10] = (ip >> 24) & 0xff;
    aes_key[13] = (ip >> 20) & 0xff;
    return 0;
}

static int server_broadcast_build_record(char *pBuffer, size_t len, const char *pIfaceName, u32 ipaddr){
    if (NULL == pBuffer || len <= 256){
        return -1;
    }
    char ip[128];
    char buffer[256] = {0};
    int cur = time(NULL);
    struct in_addr addr;
    addr.s_addr = htonl(ipaddr);
    strcpy(ip, inet_ntoa(addr));
    snprintf(buffer, 256, "%s:%s:port:%d:time:%d", pIfaceName, ip, (int)gPort, (int)cur);
    /* printf("Broadcast record:%s\n", buffer); */
    update_key_by_ip(ipaddr);
    aesEncrypt(aes_key, 16, (const uint8_t *)buffer, (uint8_t *)pBuffer, 256);
    return 256;
}

static void parse_broadcast_record(const char *pInput, size_t len)
{
    // find the first :
    if (NULL == pInput){
        return;
    }
    int i = 0;
    /* printf("prase_broadcast_record:%s\n", pInput); */
    int ipStart = -1, ipStop = -1, portStart = -1, portStop = -1;
    for (i = 0; i < len; i++){
        if (pInput[i] != ':'){
            continue;
        }
        if (ipStart == -1){
            ipStart = i+1;
        }
        else if (ipStop == -1){
            ipStop = i-1;
        }
        else if (portStart == -1){
            portStart = i+1;
        }
        else if (portStop == -1){
            portStop = i-1;
        }
    }
    char ip[128] = {0};
    char port[128] = {0};
    int j = 0, k = 0;
    for (i = 0; i < len; i++){
        if (i >= ipStart && i <= ipStop){
            ip[j++] = pInput[i];
        }
        if (i >= portStart && i <= portStop){
            port[k++] = pInput[i];
        }
    }
    gServerIp = ntohl(inet_addr(ip));
    gPort = atoi(port);
    /* printf("~~~~~~~~~~~IP:%s\n", ip); */
    /* printf("~~~~~~~~~~~PORT:%s\n", port); */
}

/**
 * note:fanhongxuan@gmail.com
 * if there are more than one network interface
 * call this on all the interface name
 */
static void server_broadcast_ourself(const char *pIfaceName, u32 ipaddr){
    int sock = 0;
    struct sockaddr_in mcast_addr, servaddr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0){
        printf("Create socket failed:%d\n", sock);
        return;
    }
    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(ipaddr);
	/* servaddr.sin_port        = htons(port); */
    /* printf("bind to iface:%s\n", pIfaceName); */
	bind(sock, (struct sockaddr*) &servaddr, sizeof(servaddr));
    
    /* printf("server_broadcast_ourself\n"); */
    char data[1024] = {0};
    int len = server_broadcast_build_record(data, 1024, pIfaceName, ipaddr);

    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);
    mcast_addr.sin_port = htons(MCAST_PORT);
    int ret = sendto(sock, data, len, 0, (struct sockaddr*)&mcast_addr, sizeof(mcast_addr));
    if (ret < 0){
        printf("sendto failed:%d, errno:%d\n", ret, errno);
        // Loge() << "Sendto failed:" << ret << "(" << errno << ")";
    }
    closesocket(sock);
}

static int server_broadcast_ourself_on_all_iface()
{
#ifndef WIN32
    struct ifreq buf[MAX_INTERFACE];
    struct ifconf ifc;
    int ret = 0;
    int if_num = 0;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0){
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t )buf;
    ret = ioctl(fd, SIOCGIFCONF, (char*)&ifc);
    if (ret){
        closesocket(fd);
        return -1;
    }
    
    if_num = ifc.ifc_len/sizeof(struct ifreq);

    while(if_num > 0){
        if_num--;
        ret = ioctl(fd, SIOCGIFFLAGS, (char*)&buf[if_num]);
        if (ret){
            continue;
        }
        unsigned int flags = buf[if_num].ifr_flags;
        if (flags & IFF_LOOPBACK){
            /* skip the loopback iface */
            continue;
        }
        if ((flags & IFF_UP) &&
            (flags & IFF_BROADCAST) &&
            (flags & IFF_RUNNING)){

            ret = ioctl(fd, SIOCGIFADDR, (char*)&buf[if_num]);
            if (ret){
                continue;
            }
            server_broadcast_ourself(buf[if_num].ifr_name,
                                     ntohl(((struct sockaddr_in *)(&buf[if_num].ifr_addr))->sin_addr.s_addr));
        }
    }
#endif
	return 0;
}

static unsigned int STDCALL server_broadcast_loop(void *arg){
    int timeout = gRecvTimeout/2000;
    if (timeout >= 0){
        timeout = 1;
    }
    while(gServerRunning){
        server_broadcast_ourself_on_all_iface();
        sleep(timeout);
    }
    return NULL;
}

static int create_mcast_socket()
{
    int sock;
    struct sockaddr_in local_addr;
    int ret = -1;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0){
        printf("Create socket failed:%d\n", sock);
        return -1;
    }
#ifdef WIN32
	unsigned long flags = 1;
	ioctlsocket(sock, FIONBIO, (unsigned long *)&flags);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif    
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(MCAST_PORT);
    
    ret = bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if (ret < 0){
        printf("Bind failed:%d,errno:%d\n", ret, errno);
        closesocket(sock);
        return -1;
    }
    int loop = 1;
    ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *)&loop, sizeof(loop));
    if (ret < 0){
        printf("set sockopt IP_MULTICAST_LOOP failed:%d\n",ret);
        closesocket(sock);
        return -1;
    }

    // set the recv timeout
    struct timeval tv_out;
    tv_out.tv_sec = gRecvTimeout/1000;
    tv_out.tv_usec = (gRecvTimeout%1000) * 1000;
    ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out));
    if (ret < 0){
        printf("set sockopt SOL_RCVTIMEO failed:%d\n", ret);
        closesocket(sock);
        return -1;
    }
    
    // join the multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq));
    if (ret < 0){
        printf("setsockopt IP_ADD_MEMERSHIP failed:%d, errno:%d\n",ret, errno);
        closesocket(sock);
        return -1;
    }
    return sock;
}

static void destroy_mcast_socket(int sock)
{
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    int ret = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&mreq, sizeof(mreq));
    if (ret < 0){
        printf("setsockopt IP_DROP_MEMBERSHIP failed:%d, errno:%d\n", ret, errno);
    }
    closesocket(sock);
}

static void wait_for_server_broadcast()
{
    int sock = create_mcast_socket();
    if (sock < 0){
        printf("create_mcast_socket failed:%d\n", sock);
        return;
    }
    int times = 0;
    socklen_t addr_len = 0;
    char buff[1024+1] = {0};
    struct sockaddr_in local_addr;
    addr_len = sizeof(local_addr);
    memset(buff, 0, 1024+1);
    for (times = 0; times < 5; times++){
        int ret = recvfrom(sock, buff, 1024, 0, (struct sockaddr*)&local_addr, &addr_len);
        if (ret == -1){
            if (errno != EAGAIN && errno != EWOULDBLOCK){
                printf("recvfrom failed:%d, errno:%d", ret, errno);
            }
            sleep(1);
        }
        else{
            // receive ok;
            char output[256];
            update_key_by_ip(local_addr.sin_addr.s_addr);
            aesDecrypt(aes_key, 16, (const uint8_t *)buff, (uint8_t *)output, 256);
            // update the gServerIp and gPort here.
            parse_broadcast_record(output, 256);
            break;
        }
    }
    destroy_mcast_socket(sock);
}

static unsigned int STDCALL server_loop(void *arg){
    int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
    char                addr[32];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    listenfd = gServerSocket;
    if (listen <= 0){
        return NULL;
    }

    printf("Enter server_loop\n");
    listen(listenfd, LISTENLEN);
    
    maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
    gServerRunning = true;
    while(gServerRunning){
		rset = allset;		/* structure assignment */
        struct timeval timeout;
        timeout.tv_sec = gRecvTimeout/1000;
        timeout.tv_usec = (gRecvTimeout%1000) * 1000;
		nready = select(maxfd+1, &rset, NULL, NULL, &timeout);
        if (nready < 0){
            printf("server_loop:select error:%d\n", errno);
            break;
        }
        if (nready == 0){
            continue;
        }
		if (FD_ISSET(listenfd, &rset)){ /* new client connection */	
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);

            inet_ntop(AF_INET, &cliaddr.sin_addr, addr, 32);
            printf("server_loop:new client(%s:%d)\n", addr, ntohs(cliaddr.sin_port));
            
			for (i = 0; i < FD_SETSIZE; i++){
				if (client[i] < 0) {
					client[i] = connfd;	/* save descriptor */
					break;
				}
            }
			if (i == FD_SETSIZE){
				printf("server_loop:too many clients");
				break;
			}
            msg_client_t *pClient = get_free_client();
            if (NULL != pClient){
                pClient->port = ntohs(cliaddr.sin_port);
                pClient->addr = cliaddr.sin_addr;
                pClient->sock = connfd;
                pClient->offset = 0;
            }
			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i; /* max index in client[] array */
 
			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}
 
		for (i = 0; i <= maxi; i++){ 	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)){
                if (handle_client(sockfd) != 0){
                    inet_ntop(AF_INET, &cliaddr.sin_addr, addr, 32);
                    printf("server_loop:client close(%s:%d)\n", addr, ntohs(cliaddr.sin_port));
            
                    closesocket(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
                    remove_client_by_sock(sockfd);
				}
				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
    
    // release all the client
    for (i = 0; i < MAX_CLIENT; i++){
        if (gClients[i].sock != 0){
            closesocket(gClients[i].sock);
        }
    }
    printf("Quit the Server Thread\n");
    return NULL;
}

static bool do_msg_init()
{
    if (gbInit){
        return true;
    }
    gbInit = true;
    pthread_mutex_init(&gShortConnectionsMutex, NULL);
    
    // todo:fanhongxuan@gmail.com
    // convert the value from the map.
    char buffer[128] = {0};
    if (msg_getcfg("IsServer", buffer, 128) == 0){
        if (strcmp(buffer, "true") == 0){
            gIsServer = true;
        }
    }
    if (msg_getcfg(MSG_SERVERIP, buffer, 128) == 0){
        // update the serverip.
        gServerIp = ntohl(inet_addr(buffer));
    }
    if (msg_getcfg(MSG_PROTOCOL, buffer, 128) == 0){
        if (strcmp(buffer,"udp") == 0){
            gProtocol = USE_UDP;
        }
        else if (strcmp(buffer, "unix") == 0){
            gProtocol = USE_UNIX;
        }
    }
    if (msg_getcfg(MSG_PORT, buffer, 128) == 0){
        if (strlen(buffer) != 0){
            gPort = atoi(buffer);
        }
    }
    if (msg_getcfg(MSG_TIMEOUT, buffer, 128) == 0){
        if (strlen(buffer) != 0){
            gRecvTimeout = atoi(buffer);
        }
    }

    // for server, start the server_loop
    if (gIsServer){
        printf("run as server\n");
        gServerSocket = do_create_server_socket(gProtocol, gPort);
        pthread_create(&gServerThread, NULL, server_loop, NULL);
        pthread_create(&gServerBroadcastThread, NULL, server_broadcast_loop, NULL);
    }
    int count = 0;
    while(gServerRunning != true && count < 100){
        usleep(10 * 1000);
        count++;
    }
    return 0;
}

static bool do_msg_exit()
{
    /* int i = 0; */
    /* for (i = 0; i < MAX_HANDLER; i++){ */
    /*     if (gHandlers[i].handler != NULL){ */
    /*         return true; */
    /*     } */
    /* } */
    
    /* gbInit = false; */
    
    /* if (0 != gServerThread){ */
    /*     gServerRunning = false; */
    /*     pthread_join(gServerThread, NULL); */
    /*     gServerThread = 0; */
    /* } */

    /* if (0 != gServerBroadcastThread){ */
    /*     gServerRunning = false; */
    /*     pthread_join(gServerBroadcastThread, NULL); */
    /*     gServerBroadcastThread = 0; */
    /* } */
    
    /* if (0 != gServerSocket){ */
    /*     close(gServerSocket); */
    /*     gServerSocket = 0; */
    /* } */
    
    /* if (0 != gClientSocket){ */
    /*     close(gClientSocket); */
    /*     gClientSocket = 0; */
    /* } */
    /* if (0 != gClientThread){ */
    /*     gClientRunning = false; */
    /*     pthread_join(gClientThread, NULL); */
    /*     gClientThread = 0; */
    /* } */
    return true;
}

void msg_dump(char *pBuffer, u32 size)
{
    // todo:fanhongxuan@gmail.com
}

u32 msg_setcfg(const char *pProp, const char *pValue)
{
    if (NULL == pProp || NULL == pValue){
        return -1;
    }

    // 1, try to replace an exist cfg.
    int i = 0;
    for (i = 0; i < MAX_CONFIG_ITEM; i++){
        if (strcmp(pProp, gConfigMaps[i].name) == 0){
            strncpy(gConfigMaps[i].value, pValue, VALUE_LEN);
            return 0;
        }
    }

    // 2, this is a new cfg, find a free item and store it.
    for (i = 0; i < MAX_CONFIG_ITEM; i++){
        if (strlen(gConfigMaps[i].name) == 0){
            strncpy(gConfigMaps[i].name, pProp, NAME_LEN);
            strncpy(gConfigMaps[i].value, pValue, VALUE_LEN);
            return 0;
        }
    }
    printf("msg_setcfg failed\n");
    return -1;
}

u32 msg_getcfg(const char *pProp, char *pValue, unsigned int size)
{
    if (NULL == pProp || NULL == pValue){
        return -1;
    }
    int i = 0;
    for (i = 0; i < MAX_CONFIG_ITEM; i++){
        if (strcmp(pProp, gConfigMaps[i].name) == 0){
            strncpy(pValue, gConfigMaps[i].value, size);
            return 0;
        }
    }
    return -1;
}

u32 msg_init(void)
{
    // note:fanhongxuan@gmail.com
    // if use DO NOT use msg_setcfg("IsServer", value) to set the IsServer prop,
    // when call msg_init, will set IsServer to true.
    char buffer[128] = {0};
    if (msg_getcfg("IsServer", buffer, 128) != 0){
        msg_setcfg("IsServer", "true");
    }
    do_msg_init();
    return 0;
}

u32 msg_exit()
{
    return 0;
}

u32 msg_register_serial(u32 ulMsgId, msgSerial hton, msgSerial ntoh)
{
    int i = 0;
    for (i = 0; i < MAX_REGISTER; i++){
        if (gSerialRegister[i].bUsed == true &&
            gSerialRegister[i].msgId == ulMsgId){
            gSerialRegister[i].hton = hton;
            gSerialRegister[i].ntoh = ntoh;
            return 0;
        }
    }
    for (i = 0; i < MAX_REGISTER; i++){
        if (gSerialRegister[i].bUsed == false){
            gSerialRegister[i].bUsed = true;
            gSerialRegister[i].msgId = ulMsgId;
            gSerialRegister[i].hton = hton;
            gSerialRegister[i].ntoh = ntoh;
            return 0;
        }
    }
    printf("msg_register_serial failed\n");
    return -1;
}

u32 msg_register(u32 ulMsgId, msgHandler handler)
{
    // note:fanhongxuan@gmail.com
    // 1, do_msg_init
    do_msg_init();

    // 2, if we are running as client, check if we have get the right server ip.
    if ((!gIsServer) &&
        gServerIp == INADDR_ANY){
        // wait for the server ipaddr.
        wait_for_server_broadcast();
        if (gServerIp == INADDR_ANY){
            return (-2);
        }
    }

    // 2, create the client socket
    if (gClientSocket <= 0){
        gClientSocket = do_create_client_socket(gProtocol, gPort);
    }
    if (gClientSocket <= 0){
        /* failed to create client socket */
        return -1;
    }
    
    // 3, create client thread.
    if (gClientThread == 0){
        pthread_create(&gClientThread, NULL, client_long_connection_loop, NULL);
    }
    
    if (0 == gClientThread){
        return -1;
    }

    // 4, send register msg to server.
    msg_register_req_t req;
    req.msgId = htonl(ulMsgId);
    if (msg_sendto(0, &req, sizeof(req), gClientSocket, msg_type_register_req, 0, false) != 0){
        return -1;
    }
    
    // 5, try to replace an exist handler
    int i = 0;
    for (i = 0; i < MAX_HANDLER; i++){
        if (gHandlers[i].id == ulMsgId){
            gHandlers[i].handler = handler;
            return 0;
        }
    }
    
    // 6, try to add a new handler.
    for (i = 0; i < MAX_HANDLER; i++){
        if (gHandlers[i].id == 0 && gHandlers[i].handler == NULL){
            gHandlers[i].id = ulMsgId;
            gHandlers[i].handler = handler;
            return 0;
        }
    }
    
    printf("Failed to register msg:%d\n", ulMsgId);
    return -1;
}

u32 msg_unregister(u32 ulMsgId)
{
    int i = 0;
    do_msg_init();
    if (gClientSocket <= 0){
        return 0;   
    }
    msg_unregister_req_t req;
    req.msgId = htonl(ulMsgId);
    if (msg_sendto(0, &req, sizeof(req), gClientSocket, msg_type_unregister_req, 0, false) != 0){
        return -1;
    }
    for (i = 0; i < MAX_HANDLER; i++){
        if (gHandlers[i].id == ulMsgId){
            gHandlers[i].id = 0;
            gHandlers[i].handler = NULL;
        }
    }
    do_msg_exit();
    return 0;
}

u32 msg_send(u32 ulMsgId, void *pPayload, u32 ulSize)
{
    do_msg_init();
    int sock = open_client_socket();
    if (sock <= 0){
        return -1;
    }
    return msg_sendto(ulMsgId, pPayload, ulSize, sock, msg_type_user_msg_req, 0, true);
}

u32 msg_recv(u32 ulMsgId, void *pPayload, u32 ulSize)
{
    do_msg_init();

    int sock = open_client_socket();
    if (sock <= 0){
        return -1;
    }
    
    int ret = 0;
    fd_set rset;
    msg_header_t head;
    bool isHeadValid = false;
    unsigned char *pBuffer = NULL;
    u32 offset = 0;
    struct timeval timeout;
    while(1){
        FD_ZERO(&rset);
        FD_SET(sock, &rset);
        timeout.tv_sec = gRecvTimeout/1000;
        timeout.tv_usec = (gRecvTimeout%1000) * 1000;
        int nready = select(sock + 1, &rset, NULL, NULL, &timeout);
        if (nready == 0){
            break;
        }
        if (nready > 0 && FD_ISSET(sock, &rset)){
            if (!isHeadValid){
                /* read the head first. */
                ret = read(sock, ((unsigned char*)&head) + offset, sizeof(head) - offset);
                if (ret <= 0){
                    // failed to receive the head.
                    printf("client_short_connection_loop:Failed to read head\n");
                    close_client_socket(sock);
                    return -1;
                }
                offset += ret;
                if (offset != sizeof(head)){
                    continue;   
                }
                head.msgSrc = ntohl(head.msgSrc);
                head.msgType = ntohl(head.msgType);
                head.msgSize = ntohl(head.msgSize);
                head.msgId = ntohl(head.msgId);
                // printf("Head id:%u,size:%u\n", head.msgId, head.msgSize);
                pBuffer = (unsigned char *)malloc(head.msgSize);
                if (NULL == pBuffer){
                    printf("client_short_connection_loop:Failed to mallock buffer\n");
                    close_client_socket(sock);
                    return -1;
                }
                isHeadValid = true;
                offset = 0;
            }
            else if (isHeadValid && NULL != pBuffer){
                /* read the body */
                ret = read(sock, pBuffer + offset, head.msgSize - offset);
                if (ret <= 0){
                    free(pBuffer);
                    printf("client_short_connection_loop:read msg body failed:%d\n", ret);
                    close_client_socket(sock);
                    return -1;
                }
                offset += ret;
                if (offset == head.msgSize){
                    /* printf("client_short_connection_loop:Receive a (type:%u,id:%u,size:%u)\n", head.msgType, head.msgId, head.msgSize); */
                    if (head.msgType == msg_type_user_msg_resp && head.msgId == ulMsgId){
                        /**
                         * note:fanhongxuan@gmail.com
                         * call convert to convert this to host
                         */
                        int i = 0;
                        msgSerial ntoh = NULL;
                        for (i = 0; i < MAX_REGISTER; i++){
                            if (gSerialRegister[i].bUsed == true &&
                                gSerialRegister[i].msgId == head.msgId &&
                                gSerialRegister[i].ntoh != NULL){
                                ntoh = gSerialRegister[i].ntoh;
                                break;
                            }
                        }
                        void *pData = pBuffer;
                        u32 data_len = head.msgSize;
                        bool convertOk = false;
                        
                        if (NULL != ntoh){
                            if (ntoh(ulMsgId, pBuffer, head.msgSize, &pData, &data_len) == 0){
                                convertOk = true;
                            }
                        }
                        if (data_len >= ulSize){
                            data_len = ulSize;
                        }
                        memcpy(pPayload, pData, data_len);
                        if (convertOk){
                            free(pData);
                        }
                        /* u32 size = head.msgSize; */
                        /* if (size >= ulSize){ */
                        /*     size = ulSize; */
                        /* } */
                        /* memcpy(pPayload, pBuffer, size); */
                        break;
                    }
                    /* wait for the next msg */
                    isHeadValid = false;
                    offset = 0;
                }
            }
        }
    }
    
    if (NULL != pBuffer){
        free(pBuffer);
    }
    close_client_socket(sock);
    return 0;
}

u32 msg_response(u32 ulMsgId, void *pPayload, u32 ulSize)
{
    return msg_recv(ulMsgId, pPayload, ulSize);
}

