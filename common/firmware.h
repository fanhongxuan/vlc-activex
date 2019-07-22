#ifndef _VA_FIRMWARE_H_
#define _VA_FIRMWARE_H_

typedef unsigned int u32;
typedef enum{
    FIRM_DESC_LEN = 256,
    FIRM_USERNAME_LEN = 256,
    FIRM_PASSWORD_LEN = 256,
    FIRM_CODE_LEN = 256,
    FIRM_NAME_LEN = 256,
    FIRM_PATH_LEN = 1024,
    FIRM_IP_LEN = 32,
    FIRM_SIMCARDNUMBER_LEN = 32,
    FIRM_TIME_LEN = 32,
    FIRM_VIDEO_NUMBER = 10,
    FIRM_AUDIO_NUMBER = 10,
    FIRM_ALARMIN_NUMBER = 10,
    FIRM_ALARMOUT_NUMBER = 10,
    FIRM_ANALOGIN_NUMBER = 10,
    FIRM_SERIAL_NUMBER = 10,
    FIRM_STOREDEV_NUMBER = 10,
    FIRM_RECOURDITEM_NUMBER = 10,
    FIRM_EVENTITEM_NUMBER = 10,
    FIRM_CHANNEL_NUMBER = 20,
	SECURITY_INFO_LEN = 1024,
} firm_enum_t;

typedef enum{
    HIS_VIDEO_LOSS = 1, 
    HIS_MOTION_DETECT = 2,
    HIS_VIDEO_SHELETER = 4,
    HIS_HIGH_TEMP_EQU = 256,
    HIS_LOW_TEMP_EQU = 512,
    HIS_FAN_FAULT = 1024,
    HIS_DISK_FAULT = 2048,
    HIS_STATUS_EVENT = 65536,
} firm_his_alarm_type_t;

typedef enum{
    VIDEO_LOSS_TYPE = 1,
    MOTION_DETECT_TYPE = 2,
    VIDEO_SHELETER_TYPE = 4,
    HIGH_TEMP_EQU_TYPE = 256,
    LOW_TEMP_EQU_TYPE = 512,
    FAN_FAULT_TYPE = 1024,
    DISK_FAULT_TYPE = 2048,
    STATUS_EVENT_TYPE = 65536,
    TIME_REC_VIDEO_TYPE = 1048576,
    USER_REC_VIDEO_TYPE = 2097152,
} firm_video_type_t;

typedef enum{
    FIRM_LOGIN = 100,
    FIRM_GET_RESOURCE,
    FIRM_RECORD_QUERY,
    FIRM_PTZ_OPERATE,
    FIRM_START_VIDEO,
    FIRM_STOP_VIDEO,
    FIRM_START_AUDIO,
    FIRM_STOP_AUDIO,
    FIRM_HISTORY_ALARM_QUERY,
    FIRM_GET_SIM_INFO,
	FIRM_EVENT_IND,
	FIRM_SECURITY_CHIP_ENCRYPTION,
	FIRM_SECURITY_CHIP_DECRYPTION,
	FIRM_UPLOAD_IMAGE_EVT,
	FIRM_GET_BATTERY_INFO,
	FIRM_CONFIG_BIFACE_REQ,
	FIRM_CONFIG_BIFACE_IND,
    FIRM_TAKE_PHOTO_REQ,
    FIRM_TAKE_PHOTO_IND,
    FIRM_GET_TIME_REQ,
    FIRM_SET_TIME_REQ,
}firm_msg_id_t;

typedef struct{
    u32 Valid;
    char Description[FIRM_DESC_LEN];
}firm_result_t;

typedef struct{
    u32 family;
    union{
        char ipv4[FIRM_IP_LEN];
        char ipv6[FIRM_IP_LEN];
    }addr;
}firm_ip_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    char UserName[FIRM_USERNAME_LEN];
    char Password[FIRM_PASSWORD_LEN];
    firm_ip_t SourceIP;
    u32 Expires;
} firm_login_req_t;

typedef struct{
    char Name[FIRM_NAME_LEN];
    char Code[FIRM_CODE_LEN];
    u32 Status;
    u32 SubNum;
    u32 DecoderTag;
    char Longitude[FIRM_NAME_LEN];
    char Latitude[FIRM_NAME_LEN];
}firm_video_info_t;

typedef struct{
    char Desc[FIRM_DESC_LEN];
}firm_audio_info_t;

typedef struct{
    char Desc[FIRM_DESC_LEN];
}firm_alarmin_info_t;

typedef struct{
    char Desc[FIRM_DESC_LEN];
}firm_alarmout_info_t;

typedef struct{
    char Desc[FIRM_DESC_LEN];
}firm_analogin_info_t;

typedef struct{
    char Desc[FIRM_DESC_LEN];
}firm_serial_info_t;

typedef struct{
    char Desc[FIRM_DESC_LEN];
}firm_storedev_info_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    char Code[FIRM_CODE_LEN];
    u32 VideoNumber;
    firm_video_info_t Videos[FIRM_VIDEO_NUMBER];
    u32 AudioNumber;
    firm_audio_info_t Audios[FIRM_AUDIO_NUMBER];
    u32 AlarmInNumber;
    firm_alarmin_info_t AlarmIn[FIRM_ALARMIN_NUMBER];
    u32 AlarmOutNumber;
    firm_alarmout_info_t AlarmOut[FIRM_ALARMOUT_NUMBER];
    u32 AnalogInNumber;
    firm_analogin_info_t AnalogIn[FIRM_ANALOGIN_NUMBER];
    u32 SerialNumber;
    firm_serial_info_t Serial[FIRM_SERIAL_NUMBER];
    u32 StoreDevNumber;
    firm_storedev_info_t StoreDev[FIRM_STOREDEV_NUMBER];
}firm_get_resource_req_t;

typedef struct{
    char FileName[FIRM_NAME_LEN];
    char FilePath[FIRM_PATH_LEN];
    char BeginTime[FIRM_TIME_LEN];
    char EndTime[FIRM_TIME_LEN];
    u32 Size;
    firm_video_type_t Type;
}firm_record_item_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 VideoID;
    u32 Type;
    char StartTime[FIRM_TIME_LEN];
    char EndTime[FIRM_TIME_LEN];
    u32 FromIndex;
    u32 ToIndex;
    u32 RealNum;
    u32 SubNum;
    firm_record_item_t RecordItems[FIRM_RECOURDITEM_NUMBER];
}firm_record_query_req_t;

typedef struct{
    u32 DevID;
    u32 DevType;
    char Code[FIRM_CODE_LEN];
    firm_his_alarm_type_t EventType;
    char StartTime[FIRM_TIME_LEN];
    char EndTime[FIRM_TIME_LEN];
    u32 EventStatus;
}firm_event_ind_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    char StartTime[FIRM_TIME_LEN];
    char EndTime[FIRM_TIME_LEN];
    u32 FromIndex;
    u32 ToIndex;
    u32 RealNum;
    u32 SubNum;
	u32 VideoID;
    firm_event_ind_t EventItems[FIRM_EVENTITEM_NUMBER];
}firm_history_alarm_query_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 VideoID;
    u32 Command;
    u32 CommandParam1;
    u32 CommandParam2;
    u32 CommandParam3;
}firm_ptz_operate_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 VideoID;
    u32 VideoFlag;
    u32 TranType;
    firm_ip_t DstIPAddr;
    u32 DstPort;
    u32 VideoCodec;
    firm_ip_t SrcIPAddr;
    u32 SrcPort;
}firm_start_video_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 VideoID;
    u32 VideoFlag;
    u32 TranType;
    firm_ip_t SrcIPAddr;
    u32 SrcPort;
}firm_stop_video_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 AudioID;
    u32 TalkType;
    u32 TranType;
    firm_ip_t SrcIPAddr;
    u32 SrcPort;
    firm_ip_t DstIPAddr;
    u32 DstPort;
    u32 AudioCodec;
}firm_start_audio_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 AudioID;
    firm_ip_t SrcIPAddr;
    u32 SrcPort;
    firm_ip_t DstIPAddr;
    u32 DstPort;
}firm_stop_audio_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    char Code[FIRM_CODE_LEN];
    char Name[FIRM_NAME_LEN];
    char Longitude[FIRM_NAME_LEN];
    char Latitude[FIRM_NAME_LEN];
    char Supplier[FIRM_NAME_LEN];
    char APN[FIRM_NAME_LEN];
    char SIMCARDNo[FIRM_SIMCARDNUMBER_LEN];
    char GPS[FIRM_NAME_LEN];
    u32 RSSI;
    firm_ip_t IPADDR;
    u32 MonConsum;
    char SIMStatus[FIRM_NAME_LEN];
}firm_get_sim_info_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    char Code[FIRM_CODE_LEN];
    char Name[FIRM_NAME_LEN];
    char SolarPanelStatus[FIRM_NAME_LEN];
    double PVArrayVoltage;
    double PVArrayCurrent;
    double BatteryVoltage;
    double BatteryCurrent;
    double DeviceVoltage;
    double DeviceCurrent;
    double BatteryLevel;
    double BatteryTemp;
} firm_get_battery_info_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 VideoID;
    char Code[FIRM_CODE_LEN];
    char Type[8];
    char Time[64];
    char FileUrl[64];
    char FileSize[64];
    char Verfiy[32];
}firm_take_picture_ind_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    u32 VideoID;
    char Code[FIRM_CODE_LEN];
    char PicServer[64];
    u32 SnapType;
    char Range[64];
    u32 Interval;
}firm_take_picture_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    char Code[FIRM_CODE_LEN];
    char Time[64];
}firm_get_time_req_t;

typedef struct{
    firm_msg_id_t id;
    firm_result_t result;
    char Code[FIRM_CODE_LEN];
    char Time[64];
    u32 flag;
}firm_set_time_req_t;

typedef struct{
	firm_msg_id_t id;
	u32 DataLength;
	char Data[SECURITY_INFO_LEN];
} firm_security_frame_packet_t;

typedef struct{
	char Code[20];
	char type;
	char FileUrl[16];
	u32 FileSize;	
}firm_snap_image_t;

typedef struct{
	char channelID[FIRM_NAME_LEN];
} firm_dev_channel;

typedef struct{
	firm_msg_id_t id;
    firm_result_t result;
	char ServerID[FIRM_NAME_LEN];
	char ServerIP[FIRM_IP_LEN];
	u32  ServerPort;
	char DeviceID[FIRM_NAME_LEN];
	char DeviceIP[FIRM_IP_LEN];
	firm_dev_channel Channels[FIRM_CHANNEL_NUMBER];
	char DevicePassword[FIRM_PASSWORD_LEN];
	u32 ActChannelNum;
	u32 EnableB; 
}firm_enable_b_interface_req_t;

#endif

