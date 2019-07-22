#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msg.h"
#include "metadata.h"
#include "serial.h"

u32 metadata_h2n(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **ppOutputMsg, u32 *pOutputSize)
{
    if (ulMsgId != ctrl_metadata_req){
        return -1;
    }
    MB_BEGIN(ctrl_metadata_req_t);
    MB_U32(MsgId);
    MB_U32(Result);
    MB_STR(Description);
    MB_U32(Type);
    if (pReq->Type == ctrl_metadata_add_obj){
        MB_U32(Data.AddObj.Obj.Type);
        MB_U32(Data.AddObj.Obj.Id);
        MB_U32(Data.AddObj.Obj.TTL);
        MB_U32(Data.AddObj.Obj.Timestamp);
        MB_STR(Data.AddObj.Obj.Desc);
        if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_text){
            MB_U32(Data.AddObj.Obj.Data.Text.CodePage);
            MB_U32(Data.AddObj.Obj.Data.Text.Width);
            MB_U32(Data.AddObj.Obj.Data.Text.Height);
            MB_U32(Data.AddObj.Obj.Data.Text.Font);
            MB_U32(Data.AddObj.Obj.Data.Text.Colour);
            MB_U32(Data.AddObj.Obj.Data.Text.Pos.X);
            MB_U32(Data.AddObj.Obj.Data.Text.Pos.Y);
            MB_STR(Data.AddObj.Obj.Data.Text.Value);
        }
        else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_cycle){
            MB_U32(Data.AddObj.Obj.Data.Cycle.Center.X);
            MB_U32(Data.AddObj.Obj.Data.Cycle.Center.Y);
            MB_U32(Data.AddObj.Obj.Data.Cycle.Radius);
            MB_U32(Data.AddObj.Obj.Data.Cycle.Angle);
            MB_U32(Data.AddObj.Obj.Data.Cycle.StartAngle);
            MB_U32(Data.AddObj.Obj.Data.Cycle.Colour);
            MB_U32(Data.AddObj.Obj.Data.Cycle.LineWidth);
        }
        else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_rect){
            MB_U32(Data.AddObj.Obj.Data.Rect.Colour);
            MB_U32(Data.AddObj.Obj.Data.Rect.LineWidth);
            MB_U32(Data.AddObj.Obj.Data.Rect.TopLeft.X);
            MB_U32(Data.AddObj.Obj.Data.Rect.TopLeft.Y);
            MB_U32(Data.AddObj.Obj.Data.Rect.BottomRight.X);
            MB_U32(Data.AddObj.Obj.Data.Rect.BottomRight.Y);
        }
        else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_comp){
            // todo:fanhongxuan@gmail.com
        }
    }
    else if (pReq->Type == ctrl_metadata_del_obj){
        MB_U32(Data.DelObj.Id);
        MB_U32(Data.DelObj.Type);
    }
    else if (pReq->Type == ctrl_metadata_get_obj){
        // todo:fanhongxuan@gmail.com
    }
    MB_END();
    return 0;
}

u32 metadata_n2h(u32 ulMsgId, void *pInputMsg, u32 inputSize, void **ppOutputMsg, u32 *pOutputSize)
{
    if (ulMsgId != ctrl_metadata_req){
        return -1;
    }
    MP_BEGIN(ctrl_metadata_req_t);
    MP_U32(MsgId);
    MP_U32(Result);
    MP_STR(Description, CTRL_METADATA_DESC_LEN);
    MP_U32(Type);
    if (pReq->Type == ctrl_metadata_add_obj){
        MP_U32(Data.AddObj.Obj.Type);
        MP_U32(Data.AddObj.Obj.Id);
        MP_U32(Data.AddObj.Obj.TTL);
        MP_U32(Data.AddObj.Obj.Timestamp);
        MP_STR(Data.AddObj.Obj.Desc, CTRL_METADATA_DESC_LEN);
        if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_text){
            MP_U32(Data.AddObj.Obj.Data.Text.CodePage);
            MP_U32(Data.AddObj.Obj.Data.Text.Width);
            MP_U32(Data.AddObj.Obj.Data.Text.Height);
            MP_U32(Data.AddObj.Obj.Data.Text.Font);
            MP_U32(Data.AddObj.Obj.Data.Text.Colour);
            MP_U32(Data.AddObj.Obj.Data.Text.Pos.X);
            MP_U32(Data.AddObj.Obj.Data.Text.Pos.Y);
            MP_STR(Data.AddObj.Obj.Data.Text.Value, CTRL_METADATA_VALUE_LEN);
        }
        else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_cycle){
            MP_U32(Data.AddObj.Obj.Data.Cycle.Center.X);
            MP_U32(Data.AddObj.Obj.Data.Cycle.Center.Y);
            MP_U32(Data.AddObj.Obj.Data.Cycle.Radius);
            MP_U32(Data.AddObj.Obj.Data.Cycle.Angle);
            MP_U32(Data.AddObj.Obj.Data.Cycle.StartAngle);
            MP_U32(Data.AddObj.Obj.Data.Cycle.Colour);
            MP_U32(Data.AddObj.Obj.Data.Cycle.LineWidth);
        }
        else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_rect){
            MP_U32(Data.AddObj.Obj.Data.Rect.Colour);
            MP_U32(Data.AddObj.Obj.Data.Rect.LineWidth);
            MP_U32(Data.AddObj.Obj.Data.Rect.TopLeft.X);
            MP_U32(Data.AddObj.Obj.Data.Rect.TopLeft.Y);
            MP_U32(Data.AddObj.Obj.Data.Rect.BottomRight.X);
            MP_U32(Data.AddObj.Obj.Data.Rect.BottomRight.Y);
        }
        else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_comp){
            // todo:fanhongxuan@gmail.com
        }
    }
    else if (pReq->Type == ctrl_metadata_del_obj){
        MP_U32(Data.DelObj.Id);
        MP_U32(Data.DelObj.Type);
    }
    else if (pReq->Type == ctrl_metadata_get_obj){
        // todo:fanhongxuan@gmail.com
    }
    MP_END();
    return 0;
}
