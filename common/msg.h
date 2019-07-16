#ifndef _VA_MSG_HPP_
#define _VA_MSG_HPP_

#define MSG_TIMEOUT "timeout"
#define MSG_SERVERIP "serverIp"
#define MSG_PORT "port"
#define MSG_PROTOCOL "protocol"
#define MSG_PATH "path"
#define MSG_SERVER "IsServer"
#ifdef __cplusplus
extern "C"{
#endif
    typedef unsigned int u32;
    typedef unsigned short u16;
    typedef unsigned char u8;
    typedef u32 (*msgSerial)(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **pOutputMsg, u32 *pOutputSize);
    typedef u32 (*msgHandler) (void *pMsg);

    /**
     * call this to send a message
     */
    u32 msg_send(u32 ulMsgId,void *pPayload, u32 ulSize);

    /**
     * call this to get the response of the previous send message.
     *   default receive timetout is 2 seconds,
     *   use msg_setcfg(MSG_TIMEOUT, "xxxx")  to change the default wait time.
     * return < 0, mean error, return value > 0, is the response msg size.
     */
    u32 msg_response(u32 ulMsgId,void *pPayload, u32 ulSize);

    /**
     * if call msg_init and don't set IsServer, will automatic call msg_setcfg("IsServer", "true");
     * and work as msgserver.
     */
    u32 msg_init(void);
    u32 msg_exit(void);
    
    /**
     * register/unregister a message handler for a specified msg id.
     * when receive a msg with id set as ulMsgId, will call the handler.
     * NOTE:
     * after handler return, will automatically call msg_send(ulMsgId, xxx) to send a response to the msg sender.
     */
    u32 msg_register(u32 ulMsgId, msgHandler handler);
    u32 msg_unregister(u32 ulMsgId);

    /**
     * note:fanhongxuan@gmail.com
     * hton: call this function to convert the msg payload before send to socket.
     * ntoh: call this function to convert the msg payload after receive from socket.
     * the default handler is directly memcpy
     */
    u32 msg_register_serial(u32 ulMsgId, msgSerial hton, msgSerial ntoh);
    
    /**
     * new api add by fanhongxuan@gmail.com
     * those function must used before call other other function to take effective.
     * msg_setcfg("timeout", "2000");
     * set the default recv timeout to 2000 ms.
     * msg_setcfg("port", "8000");
     * set the default server socket port to 8000.
     * msg_setcfg("protocol", "tcp");
     * set the default socket protocol to tcp, default is tcp,
     * support tcp/udp/unix/msgq
     * msg_setcfg("path", "/tmp/domainsocket");
     * set the default domain socket path, or the default system-V msg-queue path.
     */
    u32 msg_setcfg(const char *pProp, const char *pValue);
    
    /**
     * new api added by fanhongxuan@gmail.com
     * read the property value.
     */
    u32 msg_getcfg(const char *pProp, char *pValue, u32 size);
    /**
     * dump the internal status of the implmenation.
     */
    void msg_dump(char *pbuffer, u32 size);
    
#ifdef __cplusplus
}
#endif

#endif /*VA_MSG_HPP*/

