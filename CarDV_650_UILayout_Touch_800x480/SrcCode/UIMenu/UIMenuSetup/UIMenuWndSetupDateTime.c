//This source code is generated by UI Designer Studio.

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "UIMenuWndSetupDateTimeRes.c"
#include "UIMenuWndSetupDateTime.h"
#include "PrjCfg.h"

//---------------------UIMenuWndSetupDateTimeCtrl Debug Definition -----------------------------
#define _UIMENUWNDSETUPDATETIME_ERROR_MSG        1
#define _UIMENUWNDSETUPDATETIME_TRACE_MSG        0

#if (_UIMENUWNDSETUPDATETIME_ERROR_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_ERR))
#define UIMenuWndSetupDateTimeErrMsg(...)            debug_msg ("^R UIMenuWndSetupDateTime: "__VA_ARGS__)
#else
#define UIMenuWndSetupDateTimeErrMsg(...)
#endif

#if (_UIMENUWNDSETUPDATETIME_TRACE_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_TRC))
#define UIMenuWndSetupDateTimeTraceMsg(...)          debug_msg ("^B UIMenuWndSetupDateTime: "__VA_ARGS__)
#else
#define UIMenuWndSetupDateTimeTraceMsg(...)
#endif

//---------------------UIMenuWndSetupDateTimeCtrl Global Variables -----------------------------

//---------------------UIMenuWndSetupDateTimeCtrl Prototype Declaration  -----------------------

//---------------------UIMenuWndSetupDateTimeCtrl Public API  ----------------------------------

//---------------------UIMenuWndSetupDateTimeCtrl Private API  ---------------------------------
//---------------------UIMenuWndSetupDateTimeCtrl Control List---------------------------
CTRL_LIST_BEGIN(UIMenuWndSetupDateTime)
CTRL_LIST_ITEM(UIMenuWndSetupDateTime_Tab)
CTRL_LIST_END

//----------------------UIMenuWndSetupDateTimeCtrl Event---------------------------
INT32 UIMenuWndSetupDateTime_OnOpen(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndSetupDateTime_OnClose(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(UIMenuWndSetupDateTime)
EVENT_ITEM(NVTEVT_OPEN_WINDOW,UIMenuWndSetupDateTime_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW,UIMenuWndSetupDateTime_OnClose)
EVENT_END

INT32 UIMenuWndSetupDateTime_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_DefaultEvent(pCtrl,NVTEVT_OPEN_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndSetupDateTime_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_DefaultEvent(pCtrl,NVTEVT_CLOSE_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
//----------------------UIMenuWndSetupDateTime_TabCtrl Event---------------------------
INT32 UIMenuWndSetupDateTime_Tab_OnKeyUp(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndSetupDateTime_Tab_OnKeyDown(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndSetupDateTime_Tab_OnKeyLeft(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndSetupDateTime_Tab_OnKeyRight(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(UIMenuWndSetupDateTime_Tab)
EVENT_ITEM(NVTEVT_KEY_UP,UIMenuWndSetupDateTime_Tab_OnKeyUp)
EVENT_ITEM(NVTEVT_KEY_DOWN,UIMenuWndSetupDateTime_Tab_OnKeyDown)
EVENT_ITEM(NVTEVT_KEY_LEFT,UIMenuWndSetupDateTime_Tab_OnKeyLeft)
EVENT_ITEM(NVTEVT_KEY_RIGHT,UIMenuWndSetupDateTime_Tab_OnKeyRight)
EVENT_END

INT32 UIMenuWndSetupDateTime_Tab_OnKeyUp(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndSetupDateTime_Tab_OnKeyDown(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndSetupDateTime_Tab_OnKeyLeft(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(pCtrl,NVTEVT_PREVIOUS_ITEM,0);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndSetupDateTime_Tab_OnKeyRight(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(pCtrl,NVTEVT_NEXT_ITEM,0);
    return NVTEVT_CONSUME;
}
//----------------------UIMenuWndSetupDateTime_YMD_YCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_Y)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_MCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_M)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_DCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_D)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_HRCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_HR)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_MINCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_MIN)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_SECCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_SEC)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_SwitchCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_Switch)
EVENT_END

//----------------------UIMenuWndSetupDateTime_Tab_YMD_VALUECtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_Tab_YMD_VALUE)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_VALUE_Other0Ctrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_VALUE_Other0)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_VALUE_Other1Ctrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_VALUE_Other1)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_VALUE_Other2Ctrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_VALUE_Other2)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_VALUE_Other3Ctrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_VALUE_Other3)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_VALUE_Other4Ctrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_VALUE_Other4)
EVENT_END

//----------------------UIMenuWndSetupDateTime_YMD_AMPMCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndSetupDateTime_YMD_AMPM)
EVENT_END
