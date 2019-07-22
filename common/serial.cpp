#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <arpa/inet.h>
#endif
#include "serial.h"
#include "firmware.h"
#ifdef __cplusplus
extern "C"{
#endif
    
u32 register_all_serial()
{
    msg_register_serial(FIRM_LOGIN, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_GET_RESOURCE, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_RECORD_QUERY, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_PTZ_OPERATE, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_START_VIDEO, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_STOP_VIDEO, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_START_AUDIO, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_STOP_AUDIO, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_HISTORY_ALARM_QUERY, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_GET_SIM_INFO, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_EVENT_IND, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_SECURITY_CHIP_ENCRYPTION, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_SECURITY_CHIP_DECRYPTION, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_TAKE_PHOTO_REQ, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_GET_BATTERY_INFO, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_CONFIG_BIFACE_REQ, msg_h2n, msg_n2h);
    msg_register_serial(FIRM_CONFIG_BIFACE_IND, msg_h2n, msg_n2h);
    return 0;
}

u32 msg_put_double(u8 **pStart, double value)
{
    if (NULL != pStart && NULL != *pStart){
        memcpy(*pStart, &value, sizeof(value));
        *pStart += sizeof(double);
    }
    return sizeof(double);
}

u32 msg_get_double(u8 **pStart, double *value)
{
    if (NULL != pStart && NULL != *pStart){
        if(NULL != value){
            memcpy(value, *pStart, sizeof(double));
        }
        *pStart += sizeof(double);
        return sizeof(double);
    }
    return 0;
}

u32 msg_put_u32(u8 **pStart, u32 value)
{
    u32 v = htonl(value);
    if (NULL != pStart && NULL != *pStart){
        memcpy(*pStart, &v, 4);
        *pStart += 4;
    }
    return 4;
}

u32 msg_get_u32(u8 **pStart, u32 *value)
{
    u32 v = 0;
    if (NULL != pStart && NULL != *pStart){
        memcpy(&v, *pStart, 4);
        if (NULL != value){
            *value = ntohl(v);
        }
        *pStart += 4;
        return 4;
    }
    return 0;
}

u32 msg_put_u16(u8 **pStart, u16 value)
{
    u16 v = htons(value);
    if (NULL != pStart && NULL != *pStart){
        memcpy(*pStart, &v, 2);
        *pStart += 2;
    }
    return 2;
}

u32 msg_get_u16(u8 **pStart, u16 *value)
{
    u16 v = 0;
    if (NULL != pStart && NULL != *pStart){
        memcpy(&v, *pStart, 2);
        if (NULL != value){
            *value = ntohs(v);
        }
        pStart += 2;
        return 2;
    }
    return 0;
}

u32 msg_put_u8(u8 **pStart, u8 value)
{
    if (NULL != pStart && NULL != *pStart){
        memcpy(*pStart, &value, 1);
        *pStart += 1;
    }
    return 1;
}

u32 msg_get_u8(u8 **pStart, u8 *value)
{
    if (NULL != pStart && NULL != *pStart){
        if (NULL != value){
            memcpy(*pStart, value, 1);
        }
        *pStart += 1;
        return 1;
    }
    return 0;
}

u32 msg_put_string(u8 **pStart, const char *pStr)
{
    u32 len = strlen(pStr);
    u32 nlen = htonl(len);
    if (NULL != pStart && NULL != *pStart){
        memcpy(*pStart, &nlen, 4);
        *pStart += 4;
        memcpy(*pStart, pStr, len);
        *pStart += len;
    }
    return 4 + len;
}

u32 msg_get_string(u8 **pStart, u8 *pStr, u32 len)
{
    u32 strLen = 0;
    msg_get_u32(pStart, &strLen);
    if (strLen > 0 && NULL != pStart && NULL != *pStart){
        if (NULL != pStr && len != 0){
            memset(pStr, 0, len);
            if (len > strLen){
                len = strLen;
            }
            memcpy(pStr, *pStart, len);
            *pStart += strLen;
        }
        return strLen;
    }
    return 0;
}
    
u32 msg_h2n(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **ppOutputMsg, u32 *pOutputSize)
{
    if (NULL == pInputMsg || NULL == ppOutputMsg || NULL == pOutputSize){
        return -1;
    }
    if (inputSize == 0){
        return -1;
    }
    if (ulMsgId == FIRM_LOGIN){
        MB_BEGIN(firm_login_req_t);
        MB_COMMON();
        MB_STR(UserName);
        MB_STR(Password);
        MB_U32(SourceIP.family);
        MB_STR(SourceIP.addr.ipv4);
        MB_U32(Expires);
        MB_END();
    }
    else if (ulMsgId == FIRM_GET_RESOURCE){
        MB_BEGIN(firm_get_resource_req_t);
        int i = 0;
        MB_COMMON();
        MB_STR(Code);
        MB_U32(VideoNumber);
        for (i = 0; i < pReq->VideoNumber; i++){
            MB_STR(Videos[i].Name);
            MB_STR(Videos[i].Code);
            MB_U32(Videos[i].Status);
            MB_U32(Videos[i].SubNum);
            MB_U32(Videos[i].DecoderTag);
            MB_STR(Videos[i].Longitude);
            MB_STR(Videos[i].Latitude);
        }
        MB_U32(AudioNumber);
        for (i = 0; i < pReq->AudioNumber; i++){
            MB_STR(Audios[i].Desc);
        }
        MB_U32(AlarmInNumber);
        for (i = 0; i < pReq->AlarmInNumber; i++){
            MB_STR(AlarmIn[i].Desc);
        }
        MB_U32(AlarmOutNumber);
        for (i = 0; i < pReq->AlarmOutNumber; i++){
            MB_STR(AlarmOut[i].Desc);
        }
        MB_U32(AnalogInNumber);
        for (i = 0; i < pReq->AnalogInNumber; i++){
            MB_STR(AnalogIn[i].Desc);
        }
        MB_U32(SerialNumber);
        for (i = 0; i < pReq->SerialNumber; i++){
            MB_STR(Serial[i].Desc);
        }
        MB_U32(StoreDevNumber);
        for (i = 0; i < pReq->StoreDevNumber; i++){
            MB_STR(StoreDev[i].Desc);
        }
        MB_END();
    }
    else if (ulMsgId == FIRM_RECORD_QUERY){
        MB_BEGIN(firm_record_query_req_t);
        int i = 0;
        MB_COMMON();
        MB_U32(VideoID);
        MB_U32(Type);
        MB_STR(StartTime);
        MB_STR(EndTime);
        MB_U32(FromIndex);
        MB_U32(ToIndex);
        MB_U32(RealNum);
        MB_U32(SubNum);
        for (i = 0; i < pReq->SubNum; i++){
            MB_STR(RecordItems[i].FileName);
            MB_STR(RecordItems[i].FilePath);
            MB_STR(RecordItems[i].BeginTime);
            MB_STR(RecordItems[i].EndTime);
            MB_U32(RecordItems[i].Size);
            MB_U32(RecordItems[i].Type);
        }
        MB_END();
    }
    else if (ulMsgId == FIRM_PTZ_OPERATE){
        MB_BEGIN(firm_ptz_operate_req_t);
        MB_COMMON();
        MB_U32(VideoID);
        MB_U32(Command);
        MB_U32(CommandParam1);
        MB_U32(CommandParam2);
        MB_U32(CommandParam3);
        MB_END();
    }
    else if (ulMsgId == FIRM_START_VIDEO){
        MB_BEGIN(firm_start_video_req_t);
        MB_COMMON();
        MB_U32(VideoID);
        MB_U32(VideoFlag);
        MB_U32(TranType);
        MB_STR(DstIPAddr.addr.ipv4);
        MB_U32(DstPort);
        MB_U32(VideoCodec);
        MB_STR(SrcIPAddr.addr.ipv4);
        MB_U32(SrcPort);
        MB_END();
    }
    else if (ulMsgId == FIRM_STOP_VIDEO){
        MB_BEGIN(firm_stop_video_req_t);
        MB_COMMON();
        MB_U32(VideoID);
        MB_U32(VideoFlag);
        MB_U32(TranType);
        MB_STR(SrcIPAddr.addr.ipv4);
        MB_U32(SrcPort);
        MB_END();
    }
    else if (ulMsgId == FIRM_START_AUDIO){
        MB_BEGIN(firm_start_audio_req_t);
        MB_COMMON();
        MB_U32(AudioID);
        MB_U32(TalkType);
        MB_U32(TranType);
        MB_STR(SrcIPAddr.addr.ipv4);
        MB_U32(SrcPort);
        MB_STR(DstIPAddr.addr.ipv4);
        MB_U32(DstPort);
        MB_U32(AudioCodec);
        MB_END();
    }
    else if (ulMsgId == FIRM_STOP_AUDIO){
        MB_BEGIN(firm_stop_audio_req_t);
        MB_COMMON();
        MB_U32(AudioID);
        MB_STR(SrcIPAddr.addr.ipv4);
        MB_U32(SrcPort);
        MB_STR(DstIPAddr.addr.ipv4);
        MB_U32(DstPort);
        MB_END();
    }
    else if (ulMsgId == FIRM_HISTORY_ALARM_QUERY){
        MB_BEGIN(firm_history_alarm_query_req_t);
        MB_COMMON();
        MB_STR(StartTime);
        MB_STR(EndTime);
        MB_U32(FromIndex);
        MB_U32(ToIndex);
        MB_U32(RealNum);
        MB_U32(SubNum);
        int i = 0;
        for (i = 0; i < pReq->SubNum && i < FIRM_EVENTITEM_NUMBER; i++){
            MB_U32(EventItems[i].DevID);
            MB_U32(EventItems[i].DevType);
            MB_U32(EventItems[i].EventType);
            MB_STR(EventItems[i].StartTime);
            MB_STR(EventItems[i].EndTime);
            MB_U32(EventItems[i].EventStatus);
        }
        MB_END();
    }
    else if (ulMsgId == FIRM_EVENT_IND){
        MB_BEGIN(firm_event_ind_t);
        MB_U32(DevID);
        MB_STR(Code);
        MB_U32(DevType);
        MB_U32(EventType);
        MB_STR(StartTime);
        MB_STR(EndTime);
        MB_U32(EventStatus);
        MB_END();
    }
    else if (ulMsgId == FIRM_GET_SIM_INFO){
        MB_BEGIN(firm_get_sim_info_req_t);
        MB_COMMON();
        MB_STR(Code);
        MB_STR(Name);
        MB_STR(Longitude);
        MB_STR(Latitude);
        MB_STR(Supplier);
        MB_STR(APN);
        MB_STR(SIMCARDNo);
        MB_STR(GPS);
        MB_U32(RSSI);
        MB_STR(IPADDR.addr.ipv4);
        MB_U32(MonConsum);
        MB_STR(SIMStatus);
        MB_END();
    }
    else if (ulMsgId == FIRM_SECURITY_CHIP_ENCRYPTION ||
             ulMsgId == FIRM_SECURITY_CHIP_DECRYPTION){
        MB_BEGIN(firm_security_frame_packet_t);
        MB_U32(id);
        MB_U32(DataLength);
        MB_STR(Data);
        MB_END();
    }
    else if (ulMsgId == FIRM_TAKE_PHOTO_REQ){
        MB_BEGIN(firm_take_picture_req_t);
        MB_COMMON();
        MB_U32(VideoID);
        MB_STR(Code);
        MB_STR(PicServer);
        MB_U32(SnapType);
        MB_STR(Range);
        MB_U32(Interval);
        MB_END();
    }
    else if (ulMsgId == FIRM_GET_BATTERY_INFO){
        MB_BEGIN(firm_get_battery_info_req_t);
        MB_COMMON();
        MB_STR(Code);
        MB_STR(Name);
        MB_STR(SolarPanelStatus);
        MB_DOUBLE(PVArrayVoltage);
        MB_DOUBLE(PVArrayCurrent);
        MB_DOUBLE(BatteryVoltage);
        MB_DOUBLE(BatteryCurrent);
        MB_DOUBLE(BatteryCurrent);
        MB_DOUBLE(DeviceVoltage);
        MB_DOUBLE(DeviceCurrent);
        MB_DOUBLE(BatteryLevel);
        MB_DOUBLE(BatteryTemp);
        MB_END();
    }
    else if (ulMsgId == FIRM_CONFIG_BIFACE_REQ ||
             ulMsgId == FIRM_CONFIG_BIFACE_IND){
        MB_BEGIN(firm_enable_b_interface_req_t);
        MB_COMMON();
        MB_STR(ServerID);
        MB_STR(ServerIP);
        MB_U32(ServerPort);
        MB_STR(DeviceID);
        MB_STR(DeviceIP);
        MB_U32(ActChannelNum);
        int i = 0;
        for (i = 0; i < pReq->ActChannelNum; i++){
            MB_STR(Channels[i].channelID);
        }
        MB_STR(DevicePassword);
        MB_U32(EnableB);
        MB_END();
    }
    else if (ulMsgId == FIRM_GET_TIME_REQ){
        MB_BEGIN(firm_get_time_req_t);
        MB_COMMON();
        MB_STR(Code);
        MB_STR(Time);
        MB_END();
    }
    else if (ulMsgId == FIRM_SET_TIME_REQ){
        MB_BEGIN(firm_set_time_req_t);
        MB_COMMON();
        MB_STR(Code);
        MB_STR(Time);
        MB_U32(flag);
        MB_END();
    }
    return -1;
}

u32 msg_n2h(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **ppOutputMsg, u32 *pOutputSize)
{
    if (NULL == pInputMsg || NULL == ppOutputMsg || NULL == pOutputSize){
        return -1;
    }
    if (inputSize == 0){
        return -1;
    }
    if (ulMsgId == FIRM_LOGIN){
        MP_BEGIN(firm_login_req_t);
        MP_COMMON();
        MP_STR(UserName, FIRM_USERNAME_LEN);
        MP_STR(Password, FIRM_PASSWORD_LEN);
        MP_U32(SourceIP.family);
        MP_STR(SourceIP.addr.ipv4, FIRM_IP_LEN);
        MP_U32(Expires);
        MP_END();
    }
    else if (ulMsgId == FIRM_GET_RESOURCE){
        MP_BEGIN(firm_get_resource_req_t);
        int i = 0;
        MP_COMMON();
        MP_STR(Code, FIRM_CODE_LEN);
        MP_U32(VideoNumber);
        for (i = 0; i < pReq->VideoNumber; i++){
            MP_STR(Videos[i].Name, FIRM_NAME_LEN);
            MP_STR(Videos[i].Code, FIRM_CODE_LEN);
            MP_U32(Videos[i].Status);
            MP_U32(Videos[i].SubNum);
            MP_U32(Videos[i].DecoderTag);
            MP_STR(Videos[i].Longitude, FIRM_NAME_LEN);
            MP_STR(Videos[i].Latitude, FIRM_NAME_LEN);
        }
        MP_U32(AudioNumber);
        for (i = 0; i < pReq->AudioNumber; i++){
            MP_STR(Audios[i].Desc, FIRM_DESC_LEN);
        }
        MP_U32(AlarmInNumber);
        for (i = 0; i < pReq->AlarmInNumber; i++){
            MP_STR(AlarmIn[i].Desc, FIRM_DESC_LEN);
        }
        MP_U32(AlarmOutNumber);
        for (i = 0; i < pReq->AlarmOutNumber; i++){
            MP_STR(AlarmOut[i].Desc, FIRM_DESC_LEN);
        }
        MP_U32(AnalogInNumber);
        for (i = 0; i < pReq->AnalogInNumber; i++){
            MP_STR(AnalogIn[i].Desc, FIRM_DESC_LEN);
        }
        MP_U32(SerialNumber);
        for (i = 0; i < pReq->SerialNumber; i++){
            MP_STR(Serial[i].Desc, FIRM_DESC_LEN);
        }
        MP_U32(StoreDevNumber);
        for (i = 0; i < pReq->StoreDevNumber; i++){
            MP_STR(StoreDev[i].Desc, FIRM_DESC_LEN);
        }
        MP_END();
    }
    else if (ulMsgId == FIRM_RECORD_QUERY){
        MP_BEGIN(firm_record_query_req_t);
        int i = 0;
        MP_COMMON();
        MP_U32(VideoID);
        MP_U32(Type);
        MP_STR(StartTime, FIRM_TIME_LEN);
        MP_STR(EndTime, FIRM_TIME_LEN);
        MP_U32(FromIndex);
        MP_U32(ToIndex);
        MP_U32(RealNum);
        MP_U32(SubNum);
        for (i = 0; i < pReq->SubNum; i++){
            MP_STR(RecordItems[i].FileName, FIRM_NAME_LEN);
            MP_STR(RecordItems[i].FilePath, FIRM_PATH_LEN);
            MP_STR(RecordItems[i].BeginTime, FIRM_TIME_LEN);
            MP_STR(RecordItems[i].EndTime, FIRM_TIME_LEN);
            MP_U32(RecordItems[i].Size);
            MP_U32(RecordItems[i].Type);
        }
        MP_END();
    }
    else if (ulMsgId == FIRM_PTZ_OPERATE){
        MP_BEGIN(firm_ptz_operate_req_t);
        MP_COMMON();
        MP_U32(VideoID);
        MP_U32(Command);
        MP_U32(CommandParam1);
        MP_U32(CommandParam2);
        MP_U32(CommandParam3);
        MP_END();
    }
    else if (ulMsgId == FIRM_START_VIDEO){
        MP_BEGIN(firm_start_video_req_t);
        MP_COMMON();
        MP_U32(VideoID);
        MP_U32(VideoFlag);
        MP_U32(TranType);
        MP_STR(DstIPAddr.addr.ipv4, FIRM_IP_LEN);
        MP_U32(DstPort);
        MP_U32(VideoCodec);
        MP_STR(SrcIPAddr.addr.ipv4, FIRM_IP_LEN);
        MP_U32(SrcPort);
        MP_END();
    }
    else if (ulMsgId == FIRM_STOP_VIDEO){
        MP_BEGIN(firm_stop_video_req_t);
        MP_COMMON();
        MP_U32(VideoID);
        MP_U32(VideoFlag);
        MP_U32(TranType);
        MP_STR(SrcIPAddr.addr.ipv4, FIRM_IP_LEN);
        MP_U32(SrcPort);
        MP_END();
    }
    else if (ulMsgId == FIRM_START_AUDIO){
        MP_BEGIN(firm_start_audio_req_t);
        MP_COMMON();
        MP_U32(AudioID);
        MP_U32(TalkType);
        MP_U32(TranType);
        MP_STR(SrcIPAddr.addr.ipv4, FIRM_IP_LEN);
        MP_U32(SrcPort);
        MP_STR(DstIPAddr.addr.ipv4, FIRM_IP_LEN);
        MP_U32(DstPort);
        MP_U32(AudioCodec);
        MP_END();
    }
    else if (ulMsgId == FIRM_STOP_AUDIO){
        MP_BEGIN(firm_stop_audio_req_t);
        MP_COMMON();
        MP_U32(AudioID);
        MP_STR(SrcIPAddr.addr.ipv4, FIRM_IP_LEN);
        MP_U32(SrcPort);
        MP_STR(DstIPAddr.addr.ipv4, FIRM_IP_LEN);
        MP_U32(DstPort);
        MP_END();
    }
    else if (ulMsgId == FIRM_HISTORY_ALARM_QUERY){
        MP_BEGIN(firm_history_alarm_query_req_t);
        MP_COMMON();
        MP_STR(StartTime, FIRM_TIME_LEN);
        MP_STR(EndTime, FIRM_TIME_LEN);
        MP_U32(FromIndex);
        MP_U32(ToIndex);
        MP_U32(RealNum);
        MP_U32(SubNum);
        int i = 0;
        for (i = 0; i < pReq->SubNum && i < FIRM_EVENTITEM_NUMBER; i++){
            MP_U32(EventItems[i].DevID);
            MP_U32(EventItems[i].DevType);
            MP_U32(EventItems[i].EventType);
            MP_STR(EventItems[i].StartTime, FIRM_TIME_LEN);
            MP_STR(EventItems[i].EndTime, FIRM_TIME_LEN);
            MP_U32(EventItems[i].EventStatus);
        }
        MP_END();
    }
    else if (ulMsgId == FIRM_EVENT_IND){
        MP_BEGIN(firm_event_ind_t);
        MP_U32(DevID);
        MP_STR(Code, FIRM_CODE_LEN);
        MP_U32(DevType);
        MP_U32(EventType);
        MP_STR(StartTime, FIRM_TIME_LEN);
        MP_STR(EndTime, FIRM_TIME_LEN);
        MP_U32(EventStatus);
        MP_END();
    }
    else if (ulMsgId == FIRM_GET_SIM_INFO){
        MP_BEGIN(firm_get_sim_info_req_t);
        MP_COMMON();
        MP_STR(Code, FIRM_CODE_LEN);
        MP_STR(Name, FIRM_NAME_LEN);
        MP_STR(Longitude, FIRM_NAME_LEN);
        MP_STR(Latitude, FIRM_NAME_LEN);
        MP_STR(Supplier, FIRM_NAME_LEN);
        MP_STR(APN, FIRM_NAME_LEN);
        MP_STR(SIMCARDNo, FIRM_SIMCARDNUMBER_LEN);
        MP_STR(GPS, FIRM_NAME_LEN);
        MP_U32(RSSI);
        MP_STR(IPADDR.addr.ipv4, FIRM_IP_LEN);
        MP_U32(MonConsum);
        MP_STR(SIMStatus, FIRM_NAME_LEN);
        MP_END();
    }
    else if (ulMsgId == FIRM_SECURITY_CHIP_ENCRYPTION ||
             ulMsgId == FIRM_SECURITY_CHIP_DECRYPTION){
        MP_BEGIN(firm_security_frame_packet_t);
        MP_U32(id);
        MP_U32(DataLength);
        MP_STR(Data, SECURITY_INFO_LEN);
        MP_END();
    }
    else if (ulMsgId == FIRM_TAKE_PHOTO_REQ){
        MP_BEGIN(firm_take_picture_req_t);
        MP_COMMON();
        MP_U32(VideoID);
        MP_STR(Code, FIRM_CODE_LEN);
        MP_STR(PicServer, 64);
        MP_U32(SnapType);
        MP_STR(Range, 64);
        MP_U32(Interval);
        MP_END();
    }
    else if (ulMsgId == FIRM_GET_BATTERY_INFO){
        MP_BEGIN(firm_get_battery_info_req_t);
        MP_COMMON();
        MP_STR(Code, FIRM_CODE_LEN);
        MP_STR(Name, FIRM_NAME_LEN);
        MP_STR(SolarPanelStatus, FIRM_NAME_LEN);
        MP_DOUBLE(PVArrayVoltage);
        MP_DOUBLE(PVArrayCurrent);
        MP_DOUBLE(BatteryVoltage);
        MP_DOUBLE(BatteryCurrent);
        MP_DOUBLE(BatteryCurrent);
        MP_DOUBLE(DeviceVoltage);
        MP_DOUBLE(DeviceCurrent);
        MP_DOUBLE(BatteryLevel);
        MP_DOUBLE(BatteryTemp);
        MP_END();
    }
    else if (ulMsgId == FIRM_CONFIG_BIFACE_REQ ||
             ulMsgId == FIRM_CONFIG_BIFACE_IND){
        MP_BEGIN(firm_enable_b_interface_req_t);
        MP_COMMON();
        MP_STR(ServerID, FIRM_NAME_LEN);
        MP_STR(ServerIP, FIRM_IP_LEN);
        MP_U32(ServerPort);
        MP_STR(DeviceID, FIRM_NAME_LEN);
        MP_STR(DeviceIP, FIRM_IP_LEN);
        MP_U32(ActChannelNum);
        int i = 0;
        for (i = 0; i < pReq->ActChannelNum; i++){
            MP_STR(Channels[i].channelID, FIRM_NAME_LEN);
        }
        MP_STR(DevicePassword, FIRM_PASSWORD_LEN);
        MP_U32(EnableB);
        MP_END();
    }
    else if (ulMsgId == FIRM_SET_TIME_REQ){
        MP_BEGIN(firm_set_time_req_t);
        MP_COMMON();
        MP_STR(Code, FIRM_CODE_LEN);
        MP_STR(Time, 64);
        MP_U32(flag);
        MP_END();
    }
    else if (ulMsgId == FIRM_GET_TIME_REQ){
        MP_BEGIN(firm_get_time_req_t);
        MP_COMMON();
        MP_STR(Code, FIRM_CODE_LEN);
        MP_STR(Time, 64);
        MP_END();
    }
    return -1;
}

#ifdef __cplusplus
}
#endif

