#ifndef _EXAMPLE_SERIAL_H_
#define _EXAMPLE_SERIAL_H_
#include "msg.h"
#ifdef __cplusplus
extern "C"{
#endif

#define BUFFER_SIZE (1024 * 50)
#define MB_BEGIN(beginType) beginType *pReq = (beginType *)pInputMsg; \
    if (NULL != pReq){                                                \
        u8 buffer[BUFFER_SIZE];                                       \
        u8 *pBuffer = buffer;                                         \
        u32 len = 0;
#define MB_END() if (len > BUFFER_SIZE){                \
        printf("Buffer is tow small, need:%d\n", len);  \
        len = BUFFER_SIZE;                              \
    }                                                   \
    *ppOutputMsg = malloc(len);                         \
    if (NULL != *ppOutputMsg){                          \
        memcpy(*ppOutputMsg, buffer, len);              \
        *pOutputSize = len;                             \
        return 0;                                       \
    }                                                   \
    }

#define MB_DOUBLE(itemtag) do{                       \
    len += msg_put_double(&pBuffer, pReq->itemtag);  \
    }while(0)

#define MB_U32(itemtag) do{                          \
        len += msg_put_u32(&pBuffer, pReq->itemtag); \
    }while(0)

#define MB_STR(itemtag) do{                                             \
        len += msg_put_string(&pBuffer, (const char*)pReq->itemtag);    \
    }while(0)

#define MB_COMMON() do{                         \
        MB_U32(id);                             \
        MB_U32(result.Valid);                   \
        MB_STR(result.Description);             \
    }while(0)

#define MP_BEGIN(beginType) beginType *pReq = (beginType*)malloc(sizeof(beginType)); \
    if (NULL != pReq){                                                  \
    int len = 0;                                                        \
    u8 *pStart = (u8*)pInputMsg;                                        \
    *ppOutputMsg = pReq;                                                \
    *pOutputSize = sizeof(beginType);                                   \
    memset(pReq, 0, sizeof(beginType));
#define MP_END() return 0;}

#define MP_DOUBLE(itemtag) do{                          \
    len += msg_get_double(&pStart, &pReq->itemtag);     \
    }while(0)

#define MP_U32(itemtag) do{                             \
        len += msg_get_u32((u8 **)&pStart, (u32 *)&pReq->itemtag);   \
    }while(0)
#define MP_STR(itemtag, maxlen) do{                             \
        len += msg_get_string((u8 **)&pStart, (u8 *)pReq->itemtag, maxlen); \
    }while(0)
#define MP_COMMON() do{                             \
        MP_U32(id);                                 \
        MP_U32(result.Valid);                       \
        MP_STR(result.Description, FIRM_DESC_LEN);  \
    }while(0)

u32 msg2json(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **ppOutputMsg, u32 *pOutputSize);
u32 json2msg(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **pOutputMsg, u32 *pOutputSize);

u32 msg_h2n(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **ppOutputMsg, u32 *pOutputSize);
u32 msg_n2h(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **pOutputMsg, u32 *pOutputSize);
u32 register_all_serial();

u32 msg_put_double(u8 **pStart, double value);
u32 msg_get_double(u8 **pStart, double *value);
u32 msg_put_u32(u8 **pStart, u32 value);
u32 msg_get_u32(u8 **pStart, u32 *value);
u32 msg_put_u16(u8 **pStart, u16 value);
u32 msg_get_u16(u8 **pStart, u16 *value);
u32 msg_put_u8(u8 **pStart, u8 value);
u32 msg_get_u8(u8 **pStart, u8 *value);
u32 msg_put_string(u8 **pStart, const char *pStr);
u32 msg_get_string(u8 **pStart, u8 *pStr, u32 len);
#ifdef __cplusplus
}
#endif
#endif

