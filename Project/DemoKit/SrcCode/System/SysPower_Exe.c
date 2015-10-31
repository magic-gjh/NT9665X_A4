/*
    System Power Callback

    System Callback for Power Module.

    @file       SysPower_Exe.c
    @ingroup    mIPRJSYS

    @note

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/

////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "UIFrameworkExt.h"
#include "UICommon.h"
#include "AppLib.h"
#include "GxInput.h"
#include "DxPower.h"
#include "DxLens.h"
#include "GxFlash.h"
#include "GxPower.h"
#include "DbgUtApi.h"
#include "Adc.h"
#if (UI_STYLE==UI_STYLE_DRIVER)
#include "UsbDevDef.h"
#endif

//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         1 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysPwrExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"

#include "NVTUserCommand.h"
#include "UIFlowMovieFuncs.h"
#include "MediaRecAPI.h"
#include "Gsensor.h"
#include "UIConfig.h"

extern INT32 Get_GSensorSensitivity(void);
extern BOOL UIFlowWndMovie_IsGsensorTrig(void);
extern void UIFlowWndMovie_SetGsensorTrigFlag(BOOL En);


#define KEYSCAN_AUTOLCDOFF_DISABLED   0
#define KEYSCAN_DELAYOFF_DISABLED   0
#define KEYSCAN_GPS_DISABLED  0

#define KEYSCAN_BAT_LOW_VAL   745
#define BAT_ADC_CHANNAL   ADC_CHANNEL_0

static volatile UINT32  g_uiKeyScanAutoLcdoffCnt;
static volatile BOOL    g_bKeyScanAutoLcdoffEn = FALSE;
static volatile UINT32  g_uiKeyScanDelayoffCnt;
static volatile BOOL    g_bKeyScanDelayoffEn = TRUE;

static volatile UINT32  g_uiKeyScanGpsoffCnt;
static volatile BOOL    g_bKeyScanGpsoffEn = FALSE;

static UINT32   uiSystemAutoLcdoffTime     = KEYSCAN_AUTOLCDOFF_DISABLED;
static UINT32   uiSystemDelayoffTime     = KEYSCAN_DELAYOFF_DISABLED;
static UINT32   uiSystemGpsoffTime     = KEYSCAN_GPS_DISABLED;
static UINT32 	uiDetReverseGearflag;
BOOL bCarGuideLineMode  = FALSE;

#if (_MODEL_DSC_ == _MODEL_CARDV_B50_)      
static volatile UINT32  g_uiKeyScanLedOnOffCnt;
#define LED_ONOFF_COUNT       30
#endif	

int SX_TIMER_DET_SLEEP_ID = -1;
int SX_TIMER_DET_AUTOPOWEROFF_ID = -1;
int SX_TIMER_DET_BATT_ID = -1;
int SX_TIMER_DET_CHARGE_ID = -1;
int SX_TIMER_DET_SHOWADC = -1;
int SX_TIMER_DET_BATTINS_ID = -1;
int SX_TIMER_DET_AUTOLCDOFF_ID = -1;
int SX_TIMER_DET_DELAYOFF_ID = -1;
int SX_TIMER_DET_GSENSOR_ID = -1;
#if(ACC_DET_FUNC==ENABLE)	     
int SX_TIMER_DET_ACC_ID = -1;
#endif
int SX_TIMER_DET_REVERSEGEAR=-1;

void UI_DetAutoSleep(void);
void UI_DetAutoPoweroff(void);
void UI_DetBattery(void);
void UI_DetBatteryInsert(void);
void UI_DetAutoLCDOff(void);
void UI_DetDelayPowerOff(void);
void UI_DetGsensor(void);
void System_DetShowADC(void);
#if(ACC_DET_FUNC==ENABLE)	     
void System_DetACC(void);
#endif
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
void System_DetReverseGear(void);
#endif

UINT32  GxCustom_GetControl(GXCUSTOM_CTRL CustomCtrl);
void GxCustom_SetControl(GXCUSTOM_CTRL CustomCtrl, UINT32 value);
void DetBatteryLowToPowerOff(void);
UINT32 System_GetReverseGearFlag(void);

#if (AUTOSLEEP_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetSleep, UI_DetAutoSleep, 50, TRUE)
#endif
#if (AUTOPWROFF_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetAutoPoweroff, UI_DetAutoPoweroff, 50, TRUE)
#endif
#if (PWRLEVEL_FUNCTION == ENABLE)
#if(_MODEL_DSC_==_MODEL_KERNAL_BOARD_880_)	
SX_TIMER_ITEM(GxPower_DetBattery, UI_DetBattery, 25, FALSE)
#else
SX_TIMER_ITEM(GxPower_DetBattery, UI_DetBattery, 25, TRUE)
#endif
#endif
#if (USB_CHARGE_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetCharge, GxPower_DetCharge, 50, TRUE)
#endif
#if (SHOW_ADC_ON_SCREEN == ENABLE)
SX_TIMER_ITEM(System_DetShowADC, System_DetShowADC, 5, TRUE)
#endif
#if (USB_CHARGE_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetBatteryIns, UI_DetBatteryInsert, 25, TRUE)
#endif
SX_TIMER_ITEM(UI_DetAutoLCDOff, UI_DetAutoLCDOff, 50, TRUE)
SX_TIMER_ITEM(UI_DetDelayPowerOff, UI_DetDelayPowerOff, 50, FALSE)
SX_TIMER_ITEM(UI_DetGsensor, UI_DetGsensor, 1, FALSE)
#if(ACC_DET_FUNC==ENABLE)	     
SX_TIMER_ITEM(System_DetACC, System_DetACC, 5, TRUE)
#endif
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
SX_TIMER_ITEM(System_DetReverseGear, System_DetReverseGear, 50, TRUE)
#endif
#if (POWERON_TESTFLOW == ENABLE)
BOOL System_Is_USBPC_DeadBattery(void);
#endif

UINT32 DxPower_CB(UINT32 event, UINT32 param1, UINT32 param2)
{
    #if LENS_FUNCTION
    switch (event)
    {
        case DRVPWR_CB_IS_LENS_MOVING:
            return GxFlash_GetData(FLASH_SUSPEND_CALLER_CNT);
        case DRVPWR_CB_DUMMY_LOAD_START:
            {
                PMOTOR_TAB pMotorObj = motor_getObject();
                PLENS_DEVICE_OBJ pLensDevice =DrvLens_GetDevice();


                //first time,need to init motor dirver gpio
                pMotorObj->init(pLensDevice);
                pMotorObj->setSignal(MD_SIGNAL_STANDBY,ON);
                //Open shutter
                pMotorObj->shutter_setState(MOTOR_SHUTTER_DUMMYLOAD,SHUTTER_DUMMYLOAD_START);
            }
            return 0;
        case DRVPWR_CB_DUMMY_LOAD_END:
            {
                PMOTOR_TAB pMotorObj = motor_getObject();
                pMotorObj->shutter_setState(MOTOR_SHUTTER_DUMMYLOAD,SHUTTER_DUMMYLOAD_END);
                pMotorObj->setSignal(MD_SIGNAL_STANDBY,OFF);
            }
            return 0;
        default:
            DBG_ERR("event(%d)\r\n",event);
            return 0;

    }
    #else
    return 0;
    #endif
}

void System_OnPowerPreInit(void)
{
    // register DxPower Callback function
    DrvPower_RegCB(DxPower_CB);

#if (POWERON_TESTFLOW == ENABLE)
    gIsUSBChargePreCheck = System_Is_USBPC_DeadBattery();
#endif

    if(GxKey_GetData(GXKEY_NORMAL_KEY) & FLGKEY_UP)
        GxPower_SetControl(GXPWR_CTRL_IGNORE_BATT_CHECK, TRUE);
    else
        GxPower_SetControl(GXPWR_CTRL_IGNORE_BATT_CHECK, FALSE);
#if (POWERON_TESTFLOW == ENABLE)
    if(gIsUSBChargePreCheck)
        GxPower_SetControl(GXPWR_CTRL_IGNORE_BATT_CHECK, TRUE);
#else
    //GxPower_OnSystem(SYSTEM_CMD_POWERON); //Do Dummy Load
    GxPower_PowerON();
#endif

    #if _MIPS_TODO
    HwClock_EnablePowerAlarm(FALSE);
    #endif

    //If press up key, enter "Battery Calibration" mode.
    #if 0
    if(GxKey_GetData(GXKEY_NORMAL_KEY) & FLGKEY_UP)
    #else
    if(GxPower_GetControl(GXPWR_CTRL_IGNORE_BATT_CHECK))
    #endif
    {
        //"battery calibration" path
        DBG_IND("USER force to ignore low-battery check.\r\n");

        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, FALSE);
    }
    else
    {
        //normal path
        DBG_IND("Start low-battery check (dummyload).\r\n");

        //if under low-battery condition
        if(!GxPower_DummyLoad())
        {
            System_SetState(SYS_STATE_POWERON, SYS_POWERON_LOWPOWER);
            System_SetState(SYS_STATE_POWEROFF, SYS_POWEROFF_LOWPOWER);
        }
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    }
}

extern void Power_CB(UINT32 event, UINT32 param1, UINT32 param2);

void System_OnPowerInit(void)
{
     //PHASE-1 : Init & Open Drv or DrvExt
    {
    GxPower_RegCB(Power_CB);     //Register CB function of GxPower
    }
    //PHASE-2 : Init & Open Lib or LibExt
    {
    //1.砞﹚init
    //2.砞﹚CB,

    //3.爹SxJob狝叭 ---------> System Job
    //4.爹SxTimer狝叭 ---------> Detect Job
    #if (AUTOSLEEP_FUNCTION == ENABLE)
    SX_TIMER_DET_SLEEP_ID = SxTimer_AddItem(&Timer_GxPower_DetSleep);
    #endif
    #if (AUTOPWROFF_FUNCTION == ENABLE)
    SX_TIMER_DET_AUTOPOWEROFF_ID = SxTimer_AddItem(&Timer_GxPower_DetAutoPoweroff);
    #endif
    #if (PWRLEVEL_FUNCTION == ENABLE)
    SX_TIMER_DET_BATT_ID = SxTimer_AddItem(&Timer_GxPower_DetBattery);
    #endif
    #if (USB_CHARGE_FUNCTION == ENABLE)
    SX_TIMER_DET_CHARGE_ID = SxTimer_AddItem(&Timer_GxPower_DetCharge);
    #endif
    #if (SHOW_ADC_ON_SCREEN == ENABLE)
    SX_TIMER_DET_SHOWADC = SxTimer_AddItem(&Timer_System_DetShowADC);
    #endif
    #if (USB_CHARGE_FUNCTION == ENABLE)
    SX_TIMER_DET_BATTINS_ID = SxTimer_AddItem(&Timer_GxPower_DetBatteryIns);
    #endif
 //LCD Auto OFF
    SX_TIMER_DET_AUTOLCDOFF_ID = SxTimer_AddItem(&Timer_UI_DetAutoLCDOff);

    SX_TIMER_DET_DELAYOFF_ID = SxTimer_AddItem(&Timer_UI_DetDelayPowerOff);

    SX_TIMER_DET_GSENSOR_ID = SxTimer_AddItem(&Timer_UI_DetGsensor);

#if(ACC_DET_FUNC==ENABLE)	     
    SX_TIMER_DET_ACC_ID = SxTimer_AddItem(&Timer_System_DetACC);
#endif
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
    SX_TIMER_DET_REVERSEGEAR = SxTimer_AddItem(&Timer_System_DetReverseGear);
#endif
    //5.爹SxCmd狝叭 ---------> Cmd Function
    //GxPower_InstallCmd();
    //DbgUt_InstallCmd();
    }
}

void System_OnPowerExit(void)
{
    //PHASE-2 : Close Lib or LibExt
    {
    }
    //PHASE-1 : Close Drv or DrvExt
    {
    }
}

///////////////////////////////////////////////////////////////////////////////

#if (AUTOSLEEP_FUNCTION == ENABLE)
void UI_DetAutoSleep(void)
{
    if((!UI_IsForceLock()) && (!UI_IsForceLockAutoSleep()))
    {
        //debug_msg("DetAutoSleep\r\n");
        GxPower_DetSleep();
    }
}
#endif
#if (AUTOPWROFF_FUNCTION == ENABLE)
void UI_DetAutoPoweroff(void)
{
    //debug_msg("UI_DetAutoPoweroff..%d,%d,%d\r\n", GxPower_GetControl(GXPWR_CTRL_AUTOPOWEROFF_TIME),UI_IsForceLock(),UI_IsForceLockAutoPWROff());
    if((!UI_IsForceLock()) && (!UI_IsForceLockAutoPWROff()))
    {
        GxPower_DetAutoPoweroff();
    }
}
#endif
#if (PWRLEVEL_FUNCTION == ENABLE)
void UI_DetBattery(void)
{
    if(!UI_IsForceLock())
    {
        //#NT#2012/10/23#Philex Lin - begin
        // send NVTEVT_BATTERY event to flash power charging icon in power charging mode
        #if (UI_STYLE==UI_STYLE_DRIVER)
         // power charging case
         if (GxUSB_GetConnectType() == USB_CONNECT_CHARGER)
         {
            Ux_PostEvent(NVTEVT_BATTERY, 0);
         } else {
            // normal battery detect case
            GxPower_DetBattery();
			Ux_PostEvent(NVTEVT_BATTERY, 0);
         }
        #else
        GxPower_DetBattery();
        #endif
        //#NT#2012/10/23#Philex Lin - end
    }
}
#endif
#if (USB_CHARGE_FUNCTION == ENABLE)
BOOL gIsBattInsert = TRUE;
BOOL gIsBattDead = FALSE;
void UI_DetBatteryInsert(void)
{
    if(!UI_IsForceLock())
    {
        BOOL bInsert;
        #if (USB_CHARGE_FUNCTION == ENABLE)
        bInsert = GxPower_GetControl(GXPWR_CTRL_IS_BATT_INSERT);
        #else
        bInsert = TRUE;
        #endif
        if(bInsert != gIsBattInsert)
        {
            gIsBattInsert = bInsert;
            debug_msg("  Battery Insert = %s\r\n", gIsBattInsert?"YES":"NO");
        }
    }
}
#endif
#if (SHOW_ADC_ON_SCREEN == ENABLE)
static char ADCStr[64];
static DC**     pDCList;
void System_DetShowADC(void)
{
    IRECT  StringRect = { 20, 100, 200, 40 };
    IRECT*  pRect = &StringRect;
   // static char ADCStr[20];

    //UIDebug_Sys("aa  = %d\r\n",UI_GetData(UI_ShowADC));
    if(1)//(UI_GetData(UI_ShowADC) == TRUE)
    {
        UI_SetDisplayDirty(TRUE); //set TRUE to force dirty current begin/end

        pDCList = (DC**)UI_BeginScreen();
        sprintf(ADCStr, "AD1-%ld", adc_readData(1));
        //GxGfx_SetTextStroke((const FONT*)gDemoKit_Font, FONTSTYLE_NORMAL, SCALE_1X);
        GxGfx_SetTextColor(_OSD_INDEX_TRANSPART, _OSD_INDEX_TRANSPART, 0);
        GxGfx_FillRect(((DC**)pDCList)[GxGfx_OSD],pRect->x,pRect->y,pRect->x+pRect->w,pRect->y+pRect->h);
        GxGfx_SetShapeColor(_OSD_INDEX_TRANSPART, _OSD_INDEX_TRANSPART, 0);
        GxGfx_SetTextColor(_OSD_INDEX_RED, _OSD_INDEX_GRAY, 0);
        GxGfx_Text(((DC**)pDCList)[GxGfx_OSD], pRect->x, pRect->y, ADCStr);
        UI_EndScreen((UINT32)pDCList);
    }

}
#endif


///////////////////////////////////////////////////////////////////////////////

INT32 System_OnBattChange(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_PostEvent(NVTEVT_BATTERY, 0);
    debug_msg("battery changed %d**********\r\n",GxPower_GetControl(GXPWR_CTRL_BATTERY_LEVEL));

    return NVTEVT_CONSUME;
}

INT32 System_OnBattEmpty(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    #if (EMPTYPWROFF_FUNCTION == ENABLE)
    {
        #if (UI_STYLE==UI_STYLE_DRIVER)
        // set battery low event to ui window
        Ux_PostEvent(NVTEVT_BATTERY_LOW, 0);
        #else
        System_PowerOff(SYS_POWEROFF_BATTEMPTY);
        #endif
    }
    #endif
    return NVTEVT_CONSUME;
}

INT32 System_OnPwrChargeOK(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,FALSE);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,TRUE);
    return NVTEVT_CONSUME;
}

INT32 System_OnPwrChargeSuspend(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,FALSE);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,FALSE);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,FALSE);
    return NVTEVT_CONSUME;
}

INT32 System_OnPwrChargeResume(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,TRUE);
    return NVTEVT_CONSUME;
}

void System_ResetPowerSaveCount(void)
{
    //reset auto power off and sleep counter
    #if (AUTOPWROFF_FUNCTION == ENABLE)
    GxPower_SetControl(GXPWR_CTRL_AUTOPOWEROFF_RESET,0);
    #endif
    #if (AUTOSLEEP_FUNCTION == ENABLE)
    GxPower_SetControl(GXPWR_CTRL_SLEEP_RESET,0);
    #endif
    GxCustom_SetControl(GXCUSTOM_CTRL_AUTOLCDOFF_RESET,0);
}

#if (USB_CHARGE_FUNCTION == ENABLE)
void Power_StartUSBCharge(void)
{
    if (!gIsBattInsert)
    {
        DBG_DUMP("  NO Battery, Ignore START\r\n");
        return;
    }

    if (GxPower_GetControl(GXPWR_CTRL_BATTERY_CHARGE_EN))
    {
        DBG_DUMP("  Already UNDER Charge, Ignore START\r\n");
        return;
    }

    if((USB_GetSource() == USB_SRC_USB_ADAPTER))
    {
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_HIGH);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_HIGH);
    }
#if _MIPS_TODO
    //#NT#2010/12/10#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_USB_ADAPTER) && (Power_GetSource() != POWER_SRC_USB_ADAPTER))
    {
        //Do not charge, just stop check low-battery
        DBG_DUMP("GxPower: Stop low-battery check.\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, FALSE);
        return;
    }
    //#NT#2010/12/10#Jeah Yen -end
#endif
    //#NT#2010/12/10#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_USB_PC))
    {
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_MEDIUM);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_MEDIUM);
    }
    //#NT#2010/12/10#Jeah Yen -end

    //#NT#2010/12/10#Jeah Yen -begin
    DBG_DUMP("GxPower: Stop low-battery check.\r\n");
    GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, FALSE);
    DBG_DUMP("GxPower: Start to Charge Battery\r\n");
#if (USB_CHARGE_VERIFY == ENABLE)
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,TRUE);
#endif
    GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_EN,TRUE);
    //#NT#2010/12/10#Jeah Yen -end
#if (USB_CHARGE_VERIFY == ENABLE)
#else
    //GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,FALSE);
#endif
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,TRUE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,TRUE);

    // disable flash re-charge
    SxTimer_SetFuncActive(SX_TIMER_DET_RECHARGE_ID,FALSE);
}
void Power_StopUSBCharge(void)
{
    //DBG_MSG("  gIsUSBInsert=%d,gIsBattInsert=%d,gIsBattDead=%d,gIsUSBAdapter=%d\r\n",gIsUSBInsert,gIsBattInsert,gIsBattDead,gIsUSBAdapter);
    if (!gIsBattInsert)
    {
        DBG_DUMP("  NO Battery, Ignore STOP\r\n");
        return;
    }
#if _MIPS_TODO
    //#NT#2010/12/10#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_USB_ADAPTER) && (Power_GetSource() != POWER_SRC_USB_ADAPTER))
    {
        //Not charge yet, just start check low-battery
        DBG_DUMP("GxPower: Start low-battery check.\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    }
    //#NT#2010/12/10#Jeah Yen -end

    //#NT#2011/3/30#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_NONE) && (Power_GetSource() != POWER_SRC_USB_ADAPTER))
    {
        //Cannot charge, start check low-battery
        DBG_DUMP("GxPower: Start low-battery check.\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    }
    //#NT#2011/3/30#Jeah Yen -end

    if((USB_GetSource() == USB_SRC_USB_ADAPTER))
    {
        //DBG_MSG("  USB Src = Adaptor\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_LOW);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_LOW);
    }
#endif
    if((USB_GetSource() == USB_SRC_USB_PC))
    {
        //DBG_MSG("  USB Src = PC\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_LOW);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_LOW);
    }

    if (!GxPower_GetControl(GXPWR_CTRL_BATTERY_CHARGE_EN))
    {
        DBG_DUMP("  Already NOT Charge, Ignore STOP\r\n");
        return;
    }

    //#NT#2010/12/10#Jeah Yen -begin
    DBG_DUMP("GxPower: Stop to Charge Battery\r\n");
    GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_EN,FALSE);
#if _MIPS_TODO
    if(!gIsUSBChargePreCheck
        && (GxSystem_GetState(SYSTEM_STATE_POWERON) != SYSTEM_POWERON_CHARGE)
        )
    {
#if (USB_CHARGE_VERIFY == ENABLE)
#else
        GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,FALSE);
        GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,TRUE);
#endif
        //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,FALSE);
        //GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,FALSE);
    }
#endif
    DBG_DUMP("GxPower: Start low-battery check.\r\n");
    GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    // enable flash re-charge
    SxTimer_SetFuncActive(SX_TIMER_DET_RECHARGE_ID,TRUE);
    //#NT#2010/12/10#Jeah Yen -end

}
#endif



UINT32  GxCustom_GetControl(GXCUSTOM_CTRL CustomCtrl)
{
    UINT32 getv = 0;

    switch (CustomCtrl)
    {
        case GXCUSTOM_CTRL_AUTOLCDOFF_EN:
            getv = (UINT32)g_bKeyScanAutoLcdoffEn;
            break;
        case GXCUSTOM_CTRL_AUTOLCDOFF_TIME:
            getv = uiSystemAutoLcdoffTime;
            break;
        case GXCUSTOM_CTRL_DELAYOFF_EN:
            getv = (UINT32)g_bKeyScanDelayoffEn;
            break;
        case GXCUSTOM_CTRL_DELAYOFF_TIME:
            getv = (UINT32)uiSystemDelayoffTime;
            break;
        case GXCUSTOM_CTRL_GPS_TIME:
            getv = (UINT32)uiSystemGpsoffTime;
            break;
        default:
            DBG_ERR("CustomCtrl(%d)\r\n",CustomCtrl);
            getv = 0;
            break;
    }
    return getv;
}

void GxCustom_SetControl(GXCUSTOM_CTRL CustomCtrl, UINT32 value)
{
    DBG_IND("cmd=(%d), value=%d\r\n",CustomCtrl,value);
    switch (CustomCtrl)
    {
        case GXCUSTOM_CTRL_AUTOLCDOFF_EN:
            if (g_bKeyScanAutoLcdoffEn == FALSE && value == TRUE)
            {
                g_uiKeyScanAutoLcdoffCnt = 0;
            }
            g_bKeyScanAutoLcdoffEn = (INT32)value;
            break;
        case GXCUSTOM_CTRL_AUTOLCDOFF_TIME:
            uiSystemAutoLcdoffTime = value;
            DBG_IND("GXCUSTOM_CTRL_AUTOLCDOFF_TIME cnt = %d\r\n", uiSystemAutoLcdoffTime);
            break;
	 case GXCUSTOM_CTRL_AUTOLCDOFF_RESET:
	 	g_uiKeyScanAutoLcdoffCnt=0;
#if (_MODEL_DSC_ == _MODEL_CARDV_B50_)   
             g_uiKeyScanLedOnOffCnt=0;
		if(!GPIOMap_IsBGLedOn())
		{
			GPIOMap_TurnOnBGLed();
		}		
#endif		
		break;
        case GXCUSTOM_CTRL_DELAYOFF_EN:
            if (g_bKeyScanDelayoffEn == FALSE && value == TRUE)
            {
                g_uiKeyScanDelayoffCnt = 0;
            }
            g_bKeyScanDelayoffEn = value;
            DBG_IND("g_bKeyScanDelayoffEn cnt = %d\r\n", g_bKeyScanDelayoffEn);			
            break;
        case GXCUSTOM_CTRL_DELAYOFF_TIME:
            uiSystemDelayoffTime = value;
            DBG_IND("GXCUSTOM_CTRL_DELAYOFF_TIME cnt = %d\r\n", uiSystemDelayoffTime);
            break;

	 case GXCUSTOM_CTRL_DELAYOFF_RESET:
	 	g_uiKeyScanDelayoffCnt = 0;
	 	break;
        case GXCUSTOM_CTRL_GPS_EN:
            if (g_bKeyScanGpsoffEn == FALSE && value == TRUE)
            {
                g_uiKeyScanGpsoffCnt = 0;
            }
            g_bKeyScanGpsoffEn = (INT32)value;
            break;
        case GXCUSTOM_CTRL_GPS_TIME:
            uiSystemGpsoffTime = value;
            DBG_IND("GXCUSTOM_CTRL_GPS_TIME cnt = %d\r\n", uiSystemGpsoffTime);
            break;

        default:
            DBG_ERR("CustomCtrl(%d)\r\n",CustomCtrl);
            break;
    }
}

void UI_DetAutoLCDOff(void)
{
    if ((GxCustom_GetControl(GXCUSTOM_CTRL_AUTOLCDOFF_TIME) != KEYSCAN_AUTOLCDOFF_DISABLED)
        && (g_bKeyScanAutoLcdoffEn == TRUE))
    {
        DBG_IND("Auto Lcd Off Cnt = %d\r\n", g_uiKeyScanAutoLcdoffCnt);

        g_uiKeyScanAutoLcdoffCnt++;

        if (g_uiKeyScanAutoLcdoffCnt >= GxCustom_GetControl(GXCUSTOM_CTRL_AUTOLCDOFF_TIME))
        {
            DBG_IND("Do Auto Lcd Off!\r\n");
            //System_PowerOff(SYS_POWEROFF_NORMAL);
            if(GPIOMap_IsLCDBacklightOn()==TRUE)
            {
			GPIOMap_TurnOffLCDBacklight();
	      }     
        }
    }
#if (_MODEL_DSC_ == _MODEL_CARDV_B50_)      	
	 g_uiKeyScanLedOnOffCnt++;
       if(g_uiKeyScanLedOnOffCnt>LED_ONOFF_COUNT)
       {
		if(GPIOMap_IsBGLedOn())
		{
			GPIOMap_TurnOffBGLed();
		}
	 }
#endif	
}


void UI_DetDelayPowerOff(void)
{
    if ((GxCustom_GetControl(GXCUSTOM_CTRL_DELAYOFF_TIME) != KEYSCAN_DELAYOFF_DISABLED)
        && (g_bKeyScanDelayoffEn == TRUE))
    {
        //debug_msg("Auto DelayPwrOff Cnt = %d\r\n", g_uiKeyScanDelayoffCnt);
        g_uiKeyScanDelayoffCnt++;

        if (g_uiKeyScanDelayoffCnt >=GxCustom_GetControl(GXCUSTOM_CTRL_DELAYOFF_TIME))
        {
             DBG_IND("Do AutoDelayPwrOff!\r\n");
	        if (System_GetState(SYS_STATE_CURRMODE)==PRIMARY_MODE_MOVIE)
	        {
	        	switch(gMovData.State)	
	        	{
	        	     case MOV_ST_REC:
			     case MOV_ST_REC|MOV_ST_ZOOM:
				//MediaRec_Stop(MEDIAREC_WAIT_END);  	
                		FlowMovie_StopRec();				
			     break;
	        	}
	        }			 
		Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
            
        }
    }
}

void UI_DetGsensor(void)
{
#if 1
    Gsensor_Data GS_Data;
    INT32 gsensor_trig;
    static UINT32 X,Y,Z;
    static UINT32 counter=0;
    if(counter<5)
    {
	GSensor_GetStatus(&GS_Data);
	counter++;
    }
    else
    {
        if (System_GetState(SYS_STATE_CURRMODE)==PRIMARY_MODE_MOVIE)
        {
        	switch(gMovData.State)	
        	{
        	     case MOV_ST_REC:
		     case MOV_ST_REC|MOV_ST_ZOOM:
		        if(UI_GetData(FL_MOVIE_GSENSOR)!=GSENSOR_OFF)
			  {
				if(GSensor_GetStatus(&GS_Data)==TRUE)
		            {
		            #if 1
                        		Ux_PostEvent(NVTEVT_GSENSOR_TRIG, 0);
				#else					
		            		gsensor_trig=Get_GSensorSensitivity();
		                   if((abs(GS_Data.Axis.Xacc-X)>gsensor_trig)||(abs(GS_Data.Axis.Yacc-Y)>gsensor_trig)
						||(abs(GS_Data.Axis.Zacc-Z)>gsensor_trig))
		                   	{
		            			debug_msg("gsensor trig TRUE ...%d,%d,%d, threshold:%d\r\n",GS_Data.Axis.Xacc,GS_Data.Axis.Yacc,GS_Data.Axis.Zacc,gsensor_trig);
						if(UIFlowWndMovie_IsGsensorTrig()==FALSE)
						{
							//UIFlowWndMovie_SetGsensorTrigFlag(TRUE);
                        				Ux_PostEvent(NVTEVT_GSENSOR_TRIG, 0);
						}	
		                   	}
					X=GS_Data.Axis.Xacc;
					Y=GS_Data.Axis.Yacc;
					Z=GS_Data.Axis.Zacc;		
                         #endif					
		            }		
			    }
			break;
        	}
        }
    }
#endif	
}

#if(ACC_DET_FUNC==ENABLE)	     
#define ACC_ON   0x0A
#define ACC_OFF   0x0B

#define DELAY_POWEROFF   100

void System_DetACC()
{
   BOOL uiAccStatus;
   static BOOL bAccCountFlag=FALSE;   
   static UINT32 uiKeyScanDelayoffCnt;
   static UINT32 uiAccStatusLast=0xFF;
   static BOOL bFirstPowerOn=TRUE;

   if((GxSystem_GetPowerOnSource() == ACC_SOURCE_POWERON)&&(bFirstPowerOn==TRUE))
   {
		uiAccStatus=ACC_OFF;
		bFirstPowerOn=FALSE;
   }
   else if((GxSystem_GetPowerOnSource() == MANUALKEY_SOURCE_POWERON)&&(GPIOMap_ACCDet()==ACC_OFF)&&(bFirstPowerOn==TRUE))
   {
		uiAccStatus=ACC_OFF;
		uiAccStatusLast=uiAccStatus;
		bFirstPowerOn=FALSE;		
   }
   else
   {
   		uiAccStatus=GPIOMap_ACCDet();
   }
   if(uiAccStatusLast!=uiAccStatus)
   {
	if(uiAccStatus==ACC_OFF)
	{
		bAccCountFlag=TRUE;
	}
	else if(uiAccStatus==ACC_ON)
	{
		bAccCountFlag=FALSE;
		uiKeyScanDelayoffCnt=0;
	}		
   }
  // debug_msg("System_DetACC:%d,%d,bAccCountFlag:%d\r\n",uiAccStatus,uiAccStatusLast,bAccCountFlag);
   
   if(bAccCountFlag==TRUE)
   {
		uiKeyScanDelayoffCnt++;
		if (uiKeyScanDelayoffCnt >=DELAY_POWEROFF)
		{
			if (System_GetState(SYS_STATE_CURRMODE)==PRIMARY_MODE_MOVIE)
			{
				switch(gMovData.State)	
				{
					case MOV_ST_REC:
					case MOV_ST_REC|MOV_ST_ZOOM:
					FlowMovie_StopRec();				
					break;
				}
			}			 
			Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);	            
		}			      
   	}   
	uiAccStatusLast=uiAccStatus;  	
}
#endif
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
void System_DetReverseGear(void)
{
    static UINT32   uiReverseGearPrevSts = 0xFFFFFFFF;
    UINT32          uiReverseGearCurSts;
    static BOOL bChangeMode=FALSE;
    static BOOL bFirstPowerOn=TRUE;
    if(GPIOMap_IsReverseGear())
    {
        uiReverseGearCurSts =  REVERSEGEAR_ON ;
    }
    else
    {
        uiReverseGearCurSts =  REVERSEGEAR_OFF ;
    }

    //debug_err(("uiMacroPrevSts %x uiMacroCurSts %x  \r\n", uiMacroPrevSts, uiMacroCurSts));

    if(uiReverseGearPrevSts != uiReverseGearCurSts)
    {
        if(bFirstPowerOn==TRUE)
        {
		bFirstPowerOn=FALSE;
	       uiReverseGearPrevSts = uiReverseGearCurSts;		
	 }
	 else
	 {
		 if((System_GetState(SYS_STATE_CURRMODE)!=PRIMARY_MODE_MOVIE)&&(bChangeMode==FALSE))
		 {
		       bChangeMode=TRUE;
	    		//Ux_SendEvent(&UISetupObjCtrl,NVTEVT_EXE_CHANGEDSCMODE,1,DSCMODE_CHGTO_NEXT);
	        	Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MOVIE);			
		 }
		 else if(bChangeMode==TRUE)
		 {
			bChangeMode=FALSE;
		 }
		 else
		 {
		 		uiDetReverseGearflag = uiReverseGearCurSts;
	        	Ux_PostEvent(NVTEVT_REVERSEGEAR, 1,uiReverseGearCurSts);
	        	uiReverseGearPrevSts = uiReverseGearCurSts;
		 }
	 }
    }
}
#endif

/********************************************************************
* 函 数 名: DetBatteryLowToPowerOff
* 函数功能: 开机电量检测，检测到低电执行关机
* 入口参数: NULL
* 出口参数: NULL
********************************************************************/
void DetBatteryLowToPowerOff(void)
{
	UINT32 uiAdcCount = 0, uiAdcAvg = 0,uiAdcSum = 0; 
	UINT32 i;
		
	if (GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_SW)
	{
    	//read battery adc value
    	for (uiAdcCount = 0; uiAdcCount < 10; uiAdcCount++)
    	{
    		uiAdcSum += adc_readData(BAT_ADC_CHANNAL);		
        	Delay_DelayMs(50);	
    	}
            
     	uiAdcAvg = uiAdcSum / uiAdcCount;
		debug_msg("uiAdcAvg = %d\r\n",uiAdcAvg);
		debug_msg("uiAdcVal = %d\r\n",adc_readData(BAT_ADC_CHANNAL));
	        
     	if (uiAdcAvg <= KEYSCAN_BAT_LOW_VAL)
	 	{   						
			if (GxUSB_GetConnectType() == USB_CONNECT_CHARGER)
			{
				return;
			}
                
            Delay_DelayMs(10);
			debug_msg("----------DetBatteryLowToPowerOff------------");
		    rtc_poweroffPWR();						
	   	}
	 }

	return;
}

UINT32 System_GetReverseGearFlag(void)
{
	return uiDetReverseGearflag;	
}