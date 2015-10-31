//This source code is generated by UI Designer Studio.

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "UIMenuWndPlayDelRes.c"
#include "UIMenuWndPlayDel.h"
#include "UIMenuWndPlayConfirmDel.h"
#include "MenuCommonItem.h"
//#include "UIFlow.h"
#include "../../../D800x480/UIFlow.h"

//---------------------UIMenuWndPlayDelCtrl Control List---------------------------
CTRL_LIST_BEGIN(UIMenuWndPlayDel)
CTRL_LIST_ITEM(UIMenuWndPlayDel_Panel)
CTRL_LIST_END

//----------------------UIMenuWndPlayDelCtrl Event---------------------------
INT32 UIMenuWndPlayDel_OnOpen(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndPlayDel_OnClose(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndPlayDel_OnKeyLeft(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndPlayDel_OnKeyMenu(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndPlayDel_OnKeyMode(VControl *, UINT32, UINT32 *);
//INT32 UIMenuWndPlayDel_OnKeyShutter2(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(UIMenuWndPlayDel)
EVENT_ITEM(NVTEVT_OPEN_WINDOW,UIMenuWndPlayDel_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW,UIMenuWndPlayDel_OnClose)
EVENT_ITEM(NVTEVT_KEY_LEFT,UIMenuWndPlayDel_OnKeyLeft)
EVENT_ITEM(NVTEVT_KEY_MENU,UIMenuWndPlayDel_OnKeyMenu)
EVENT_ITEM(NVTEVT_KEY_MODE,UIMenuWndPlayDel_OnKeyMode)
//EVENT_ITEM(NVTEVT_KEY_SHUTTER2,UIMenuWndPlayDel_OnKeyShutter2)
EVENT_END

INT32 UIMenuWndPlayDel_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UxMenu_SetData(&UIMenuWndPlayDel_MenuCtrl, MNU_CURITM, 0);
    Ux_DefaultEvent(pCtrl,NVTEVT_OPEN_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_DefaultEvent(pCtrl,NVTEVT_CLOSE_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_OnKeyLeft(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_CloseWindow((VControl *)(&UIMenuWndPlayDelCtrl), 0);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_OnKeyMenu(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_CloseWindow(&MenuCommonItemCtrl,0); // close whole tab menu
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_OnKeyMode(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(&UISetupObjCtrl,NVTEVT_EXE_CHANGEDSCMODE,1,DSCMODE_CHGTO_NEXT);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_OnKeyShutter2(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(&UISetupObjCtrl,NVTEVT_FORCETO_LIVEVIEW_MODE,0);
    return NVTEVT_CONSUME;
}
//---------------------UIMenuWndPlayDel_PanelCtrl Control List---------------------------
CTRL_LIST_BEGIN(UIMenuWndPlayDel_Panel)
CTRL_LIST_ITEM(UIMenuWndPlayDel_Menu)
CTRL_LIST_END

//----------------------UIMenuWndPlayDel_PanelCtrl Event---------------------------
EVENT_BEGIN(UIMenuWndPlayDel_Panel)
EVENT_END

//----------------------UIMenuWndPlayDel_MenuCtrl Event---------------------------
INT32 UIMenuWndPlayDel_Menu_OnKeyUp(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndPlayDel_Menu_OnKeyDown(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndPlayDel_Menu_OnKeyEnter(VControl *, UINT32, UINT32 *);
INT32 UIMenuWndPlayDel_Menu_OnKeyShutter2(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(UIMenuWndPlayDel_Menu)
EVENT_ITEM(NVTEVT_KEY_UP,UIMenuWndPlayDel_Menu_OnKeyUp)
EVENT_ITEM(NVTEVT_KEY_DOWN,UIMenuWndPlayDel_Menu_OnKeyDown)
EVENT_ITEM(NVTEVT_KEY_ENTER,UIMenuWndPlayDel_Menu_OnKeyEnter)
#if((_MODEL_DSC_ == _MODEL_CARDV_B50_)||(_MODEL_DSC_== _MODEL_DUAL_NAZHIDA_))
EVENT_ITEM(NVTEVT_KEY_CUSTOM1,UIMenuWndPlayDel_Menu_OnKeyShutter2)
#else
EVENT_ITEM(NVTEVT_KEY_SHUTTER2,UIMenuWndPlayDel_Menu_OnKeyShutter2)
#endif
EVENT_END

INT32 UIMenuWndPlayDel_Menu_OnKeyUp(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(pCtrl,NVTEVT_PREVIOUS_ITEM,0);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_Menu_OnKeyDown(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(pCtrl,NVTEVT_NEXT_ITEM,0);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_Menu_OnKeyEnter(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32   curItem;

    curItem = UxMenu_GetData(pCtrl,MNU_CURITM);

    Ux_OpenWindow((VControl *)(&UIMenuWndPlayConfirmDelCtrl), 1, curItem);
    return NVTEVT_CONSUME;
}
INT32 UIMenuWndPlayDel_Menu_OnKeyShutter2(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    // the same behavior as ENTER key!
    return UIMenuWndPlayDel_Menu_OnKeyEnter(pCtrl, paramNum, paramArray);
}
