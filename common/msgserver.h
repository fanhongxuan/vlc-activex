#ifndef _MSG_SERVER_H_
#define _MSG_SERVER_H_
#include "msg.h"
#define CTRL_METADATA_VALUE_LEN 256
#define CTRL_METADATA_POINT_LIST_MAX 256
#define CTRL_METADATA_DESC_LEN 256

enum{
    /* the object type define */
    ctrl_metadata_object_type_text = 0,
    ctrl_metadata_object_type_cycle,
    ctrl_metadata_object_type_rect,
    ctrl_metadata_object_type_comp, /*  */

    /* the operation type define */
    ctrl_metadata_get_obj = 0,
    ctrl_metadata_add_obj,
    ctrl_metadata_del_obj,

    /* the msgq msg id define */
    ctrl_metadata_req  = 20000,
    ctrl_metadata_resp,
};

typedef struct{
    u32 X;
    u32 Y;
} ctrl_metadata_point_t;

typedef struct{
    u32 CodePage;               /* Value Code page */
    u32 Width;
    u32 Height;
    u32 Font;
    u32 Colour;
    ctrl_metadata_point_t pos;  /* position of this text */
    char Value[CTRL_METADATA_VALUE_LEN];
} ctrl_metadata_text_t;

typedef struct{
    ctrl_metadata_point_t Center;
    u32 Radius;
    u32 Angle;
    u32 StartAngle;
    u32 Colour;
    u32 LineWidth;
} ctrl_metadata_cycle_t;

typedef struct{
    u32 Colour;
    u32 LineWidth;
    ctrl_metadata_point_t TopLeft;
    ctrl_metadata_point_t BottomRight;
} ctrl_metadata_rect_t;

typedef struct{
    u32 Colour;
    u32 LineWidth;
    u32 Count;
    u32 SubNum;
    u32 Index;
    ctrl_metadata_point_t PointList[CTRL_METADATA_POINT_LIST_MAX];
} ctrl_metadata_comp_t;

typedef struct{
    u32 Type;
    u32 Id;
    u32 TTL;
    char Desc[CTRL_METADATA_DESC_LEN];
    union{
        ctrl_metadata_text_t Text;
        ctrl_metadata_cycle_t Cycle;
        ctrl_metadata_rect_t Rect;
        ctrl_metadata_comp_t Comp;
    }Object;
} ctrl_metadata_object_t;

typedef struct{
    ctrl_metadata_object_t obj;  
} ctrl_metadata_add_obj_req_t;

typedef struct{
    u32 Id;
    u32 Type;
    ctrl_metadata_object_t obj;
} ctrl_metadata_get_obj_req_t;

typedef struct{
    u32 Id;                     /* object id to delete */
    u32 Type;                   /* object type */
} ctrl_metadata_del_obj_req_t;

typedef struct{
    u32 MsgId;
    u32 Result;
    char Description[CTRL_METADATA_DESC_LEN];
    u32 type;                   /* add/del/get */
    union{
        ctrl_metadata_add_obj_req_t addObj;
        ctrl_metadata_del_obj_req_t delObj;
        ctrl_metadata_get_obj_req_t getObj;
    }data;
} ctrl_metadata_req_t;


/* api used on the server */
u32 ctrl_metadata_start_server(const char *pServerIp, const char *pServerPort, const char *pTimeOut);

u32 ctrl_metadata_add_object(ctrl_metadata_object_t *pObj);
u32 ctrl_metadata_del_object(u32 objId, u32 objType);
u32 ctrl_metadata_get_object(u32 objId, u32 objType, ctrl_metadata_object_t *pObj);

u32 ctrl_metadata_get_objects(u32 objType,
                          u32 startIndex,
                          ctrl_metadata_object_t *pObjectList,
                          u32 *pObjectListSize);
u32 ctrl_metadata_clr_objects(u32 objType);
u32 ctrl_metadata_clr_allobj();

#endif
