/*
    System Main Callback

    System Callback for System Module.

    @file       SysMain_Exe.c
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
#if (UI_STYLE==UI_STYLE_DEMO)
#include "DialogWarn.h"
#endif

#if (UI_STYLE==UI_STYLE_DRIVER)
#include "SoundData.h"
#include "UIModeCalibration.h"
#include "IrRx.h"
#if (GPS_FUNCTION == ENABLE)
#include "GPS.h"
#endif
#endif
#include "rtc.h"
#include "DCF.h"
#include "UsbDevDef.h"
//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         1 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysMainExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
#include "UIConfig.h"

///////////////////////////////////////////////////////////////////////////////

extern void GxSystem_BootStart(void);
extern void GxSystem_BootEnd(void);
extern void GxSystem_ShutDownStart(void);
extern void GxSystem_ShutDownEnd(void);

extern VControl SystemObjCtrl;
extern VControl UISetupObjCtrl;
extern VControl* gUxCtrlTypeList[];
extern VControl* gAppCtrlTypeList[];

//#NT#2015/01/21#KS Hung -begin
//#NT#Fast boot recording.
#if (_FAST_BOOT_REC_FUNC_)
int *g_piChangeModeTo = &PRIMARY_MODE_MOVIE;
#endif
//#NT#2015/01/21#KS Hung -end

void Install_AppObj(void)
{
    //Init object list for UIControl event process

    //register UI control type (from UIControl Lib)
    Ux_SetUITypeList(gUxCtrlTypeList, CTRL_EX_TYPE_MAX - CTRL_TYPE_MIN);

    //register App control type (from AppControl Lib)
    gAppCtrlTypeList[APP_BASE - APP_TYPE_MIN] = &SystemObjCtrl; //App base type = SystemObjCtrl
    gAppCtrlTypeList[APP_SETUP - APP_TYPE_MIN] = &UISetupObjCtrl; //App setup type = UISetupObjCtrl
    Ux_SetAppTypeList(gAppCtrlTypeList, APP_TYPE_MAX - APP_TYPE_MIN);

    //set default active app = current system object
    Ux_SetActiveApp(&UISetupObjCtrl);
}

///////////////////////////////////////////////////////////////////////
#include "GxTimer.h"

int SX_TIMER_DET_TIMER_ID = -1;
void Timer_CB(UINT32 event, UINT32 param1, UINT32 param2);

SX_TIMER_ITEM(System_CntTimer, GxTimer_CountTimer, 1, FALSE)

void System_OnTimerInit(void)
{
    //PHASE-1 : Init & Open Drv or DrvExt
    {
    GxTimer_RegCB(Timer_CB);         //Register CB function of GxTimer
    SX_TIMER_DET_TIMER_ID = SxTimer_AddItem(&Timer_System_CntTimer);
    //5.���USxCmd�A�� ---------> Cmd Function
    //HwClock_InstallCmd();
    }
    {
    //PHASE-2 : Init & Open Lib or LibExt
    }
}

void System_OnTimerExit(void)
{
    //PHASE-2 : Close Lib or LibExt
    {
    }
    //PHASE-1 : Close Drv or DrvExt
    {
    }
}

///////////////////////////////////////////////////////////////////////
//System flow event

#include "GxPower.h"

extern UINT32 Input_GroupStatus2Event(UINT32 status);
extern BOOL g_bConsumeStatus;
extern BOOL gbIsPlayBackModePowerOn;


#if (POWERON_TESTFLOW == ENABLE)
BOOL System_Is_USBPC_DeadBattery(void)
{
    if(GxPower_GetControl(GXPWR_CTRL_IS_BATT_INSERT))
        return FALSE;
    /*
    if(!GxPower_GetControl(GXPWR_CTRL_IS_BATT_INSERT))
        return FALSE;
    if(!GxPower_GetControl(GXPWR_CTRL_IS_DEAD_BATT))
        return FALSE;
    */
    if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBUS)
    {
        if(GxUSB_GetIsUSBPlug())
        {
            GxUSB_UpdateConnectType();
            if(GxUSB_GetConnectType() == USB_CONNECT_PC)
            {
                debug_msg("^GUSBPC+DeadBatt\r\n");
                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif

INT32 System_OnBoot(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    if(paramNum == 1 && paramArray[0] == 0) //power-on start
    {
        //GxSystem_BootStart();
    }
    if(paramNum == 1 && paramArray[0] == 1) //power-on end
    {
        UINT32 paramNum;
        UINT32 paramArray[MAX_MESSAGE_PARAM_NUM];
        #if (UI_STYLE==UI_STYLE_DRIVER)
        //#NT#2012/10/18#Philex Lin - begin
        UINT32 uiChangeTo;
        UINT32 uiFSStatus;
        //#NT#2012/10/18#Philex Lin - end
        #if (_IR_REMOTE_ != _IR_REMOTE_NONE_)
        IRRX_DEVICE_OBJ IrRxDevice;
        #endif
        #endif

        GxSystem_BootEnd();

        //FilesysWaitFinish(); //wait strg init finish
        //#NT#2015/01/21#KS Hung -begin
        //#NT#Fast boot recording.
        #if (_FAST_BOOT_REC_FUNC_ == ENABLE)
        if (*g_piChangeModeTo != PRIMARY_MODE_MOVIE)
        {
            UserWaitEvent(NVTEVT_STRG_ATTACH, &paramNum, paramArray); //(paramNum == 1 && paramArray[0] == 0) //power-on begin
        }
        #else
        UserWaitEvent(NVTEVT_STRG_ATTACH, &paramNum, paramArray); //(paramNum == 1 && paramArray[0] == 0) //power-on begin
        #endif
        //#NT#2015/01/21#KS Hung -end

        #if (UI_STYLE==UI_STYLE_DRIVER)
        uiFSStatus = UI_GetData(FL_FSStatus);
        #endif

        //wait disp init finish
        //wait audio init finish
        //wait usb init finish
        #if PST_FUNC
            //#NT#2015/01/21#KS Hung -begin
            //#NT#Fast boot recording.
            #if ( (_DUAL_CARD_FUNC_ == DISABLE) && (_FAST_BOOT_REC_FUNC_ == DISABLE) )
            //Load_MenuInfo(); //jacky lan 
            #endif
            //#NT#2015/01/21#KS Hung -end
        Init_MenuInfo();
        #endif
        //#NT#2014/5/15#Philex Lin - Begin
        // set FL_DualDisp flag first depending on project requirements
        #if (DUALDISP_FUNC==DUALDISP_OFF)
        SysSetFlag(FL_DualDisp,DUALDISP_OFF);
        #elif (DUALDISP_FUNC==DUALDISP_ONXOR)
        SysSetFlag(FL_DualDisp,DUALDISP_ONXOR);
        #else
        SysSetFlag(FL_DualDisp,DUALDISP_ONBOTH);
        #endif
        //#NT#2014/5/15#Philex Lin - End

        // init Unifont table
        #if (MULTILINGUAL_FUNCTION == ENABLE)
        UniFont_Init();
        #endif
        SxTimer_SetFuncActive(SX_TIMER_DET_TIMER_ID,TRUE);
        BKG_SetExecFuncTable(gBackgroundExtFuncTable);

#if (POWERON_TESTFLOW == ENABLE)
        //init GxUSB module
        if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBUS)
        {
            //GxUSB_Init(TRUE);
        }
        else
        {
            GxUSB_Init(FALSE);
        }
#else
        //init GxUSB module
        if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBUS)
        {
            GxUSB_Init(TRUE);
        }
        else
        {
            GxUSB_Init(FALSE);
        }
#endif
        //get first app
        #if (UI_STYLE==UI_STYLE_DRIVER)

        #if (_IR_REMOTE_ != _IR_REMOTE_NONE_)
        // Open IrRx driver
        irrx_open((PIRRX_DEVICE_OBJ)&IrRxDevice);
        #endif
        //#NT#2012/10/23#Philex Lin-begin
        // init Volume to max,100
        GxSound_SetVolume(100);
        // add power up sound
        //#NT#2015/01/21#KS Hung -begin
        //#NT#Fast boot recording.
        #if (_FAST_BOOT_REC_FUNC_ == DISABLE)
        UISound_Play(DEMOSOUND_SOUND_POWERON_TONE);
        #endif
        //#NT#2015/01/21#KS Hung -end
        //#NT#2012/10/23#Philex Lin-end

        //#NT#2015/01/21#KS Hung -begin
        //#NT#Fast boot recording.
        //#NT#2014/05/19#KS Hung -begin
        //#NT#This will affect dual card function.
        //#NT#Why does it need to detect again here?
        //#NT#2012/10/29#Philex Lin-begin
        // update card status again
        #if ((_DUAL_CARD_FUNC_ == DISABLE) && (_FAST_BOOT_REC_FUNC_ == DISABLE))
        if (GxStrg_GetDeviceCtrl((UINT32)GxStrg_GetDevice(0), CARD_READONLY))
        {
            UI_SetData(FL_CardStatus, CARD_LOCKED);
        }
        else if (GxStrg_GetDeviceCtrl((UINT32)GxStrg_GetDevice(0), CARD_INSERT))
        {
            UI_SetData(FL_CardStatus, CARD_INSERTED);
        }
        else
        {
            UI_SetData(FL_CardStatus, CARD_REMOVED);
        }
        #endif
        //#NT#2012/10/29#Philex Lin-end
        //#NT#2014/05/19#KS Hung -end
        //#NT#2015/01/21#KS Hung -end
        UI_SetData(FL_FSStatus,uiFSStatus);
        //#NT#2013/1/6#Philex Lin-begin
        #if (GPS_FUNCTION == ENABLE)
	 #if(_MODEL_DSC_==_MODEL_KERNAL_BOARD_880_)
        GPSRec_Open(UART_BAUDRATE_115200);
	 #else
        GPSRec_Open(UART_BAUDRATE_9600);
	 #endif	 
        #endif
        //#NT#2013/1/6#Philex Lin-end

        //#NT#2015/01/21#KS Hung -begin
        //#NT#Fast boot recording.
        #if (_FAST_BOOT_REC_FUNC_)
        if (0)
        #else
        if (TRUE==ModeCalibration_CheckEng())
        #endif
        //#NT#2015/01/21#KS Hung -end
        {
            uiChangeTo = PRIMARY_MODE_CALIBRATION;
            Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, uiChangeTo);
        } else {
            // Init LED_
            GxLED_SetCtrl(KEYSCAN_LED_GREEN,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT);
            GxLED_SetCtrl(KEYSCAN_LED_RED,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT/5);
            GxLED_SetCtrl(KEYSCAN_LED_FCS,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT/10);
            SxTimer_SetFuncActive(SX_TIMER_DET_LED_ID,TRUE);
            //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,TRUE);

            //open first app mode
            //#NT#2015/01/21#KS Hung -begin
            //#NT#Fast boot recording.
            #if (_FAST_BOOT_REC_FUNC_)
            uiChangeTo = *g_piChangeModeTo;
            #else
            uiChangeTo = PRIMARY_MODE_MOVIE;
            #endif
            //#NT#2015/01/21#KS Hung -end
            //Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MAIN);
            //Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_PLAYBACK);
 #if(LOW_POWER_TIMELAPSE_REC == ENABLE)

            DBG_IND("rtc_isPWRAlarm() = %d--%d =====\r\n",rtc_isPWRAlarm(),UI_GetData(FL_LOW_POWER_TIMELAPSE_REC));

            if(UI_GetData(FL_LOW_POWER_TIMELAPSE_REC)&& rtc_isPWRAlarm())
            {
                //SysSetFlag(FL_PHOTO_SIZE,                   PHOTO_SIZE_2MHD);
                uiChangeTo = PRIMARY_MODE_PHOTO;
            }
#endif
            Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, uiChangeTo);
            //Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MOVIE);
        }
        UI_SetData(FL_NextMode,uiChangeTo);
        #else
        {
            UINT32 uiDscMode = DSC_MODE_PLAYBACK; //default mode
            #if (POWERONSOUND_FUNCTION ==  ENABLE)
            UISound_RegTable_Fast(); //install power-on sound table
            if(System_GetState(SYS_STATE_POWERON) == SYS_POWERON_NORMAL)
            {
                //if(!gIsUSBChargePreCheck)
                {
                    UISound_Play_Fast(PLAYSOUND_SOUND_POWERON_TONE);
                }

            }
            #endif
            #if (MODEKEY_FUNCTION == ENABLE)
            //open first app mode
            UI_Switch_DscMode(uiDscMode,DSCMODE_SWITCH_FORCE,DSCMODE_SWITCHDIR_DONT_CARE);
            #else
            if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_SW || GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_ALM)
            {
                debug_msg("^GPowerOn Src = PWR key\r\n");
                #if (STATUSKEY_FUNCTION == ENABLE)
                UINT32 status = GxKey_GetData(GXKEY_STS_KEY1);
                UINT32 key = Input_GroupStatus2Event(status);
                gbIsPlayBackModePowerOn = FALSE;
                uiDscMode = Input_Key2Mode(key);
                DBG_IND("^YDscMode=0x%x\r\n", uiDscMode);
                UI_Switch_DscMode(uiDscMode,DSCMODE_SWITCH_FORCE,DSCMODE_SWITCHDIR_DONT_CARE);
                g_bConsumeStatus = TRUE;
                #else
                gbIsPlayBackModePowerOn = TRUE;
                DBG_IND("^YDscMode=0x%x\r\n", uiDscMode);
                UI_Switch_DscMode(uiDscMode,DSCMODE_SWITCH_FORCE,DSCMODE_SWITCHDIR_DONT_CARE);
                g_bConsumeStatus = TRUE;
                #endif
            }
            else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_SW2)
            {
                debug_msg("^GPowerOn Src = PB Key\r\n");
                gbIsPlayBackModePowerOn = TRUE;
                DBG_IND("^YDscMode=0x%x\r\n", uiDscMode);
                UI_Switch_DscMode(uiDscMode,DSCMODE_SWITCH_FORCE,DSCMODE_SWITCHDIR_DONT_CARE);
                g_bConsumeStatus = TRUE;
            }
            else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBUS)
            {
                debug_msg("^GPowerOn Src = USB plug\r\n");
                if(!GxUSB_GetIsUSBPlug())
                {
                    Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
                }
                else
                {
                    UINT32 uiUsbType;
                    GxUSB_UpdateConnectType();
                    uiUsbType = GxUSB_GetConnectType();
                    switch (uiUsbType)
                    {
                    case USB_CONNECT_PC:
                        DBG_DUMP("USB is CONNECT to PC\r\n");
                        //resume queue,event can be executed
                        UI_UnlockEvent();
                        break;
                    case USB_CONNECT_CHARGING_PC:
                        DBG_DUMP("USB is CONNECT to Charging PC\r\n");
                        //resume queue,event can be executed
                        UI_UnlockEvent();
                        break;
                    case USB_CONNECT_CHARGER:
                        Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
                        DBG_DUMP("USB is CONNECT to CHARGER\r\n");
                        break;
                    default:
                        Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
                        DBG_ERR("USB is CONNECTION UNKNOWN!\r\n");
                        break;
                    }
                }
                g_bConsumeStatus = TRUE;
            }
            else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_VBAT)
            {
                debug_msg("^GPowerOn Src = DC plug\r\n");
                Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
                g_bConsumeStatus = TRUE;
            }
            /*
            else if(GxSystem_GetPowerOnSource() == GX_PWRON_SRC_PWR_ALM)
            {
                debug_msg("^GPowerOn Src = Alarm\r\n");
                Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
                g_bConsumeStatus = TRUE;
            }
            */
            #endif
            #if (POWERONSOUND_FUNCTION ==  ENABLE)
            UISound_RegTable();
            #endif
        }
        #endif
    }
    return NVTEVT_CONSUME;
}

extern void UI_Quit(void);

INT32 System_OnShutdown(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 uiJpgAddr, uiJpgSize;

    if(paramNum == 1 && paramArray[0] == 0) //power-off begin
    {
        GxSystem_ShutDownStart();		
	 
        #if (UI_STYLE==UI_STYLE_DRIVER)
        //#NT#2012/10/23#Philex Lin-begin
        // add power off sound
        UISound_Play(DEMOSOUND_SOUND_POWERON_TONE);
        //#NT#2012/10/23#Philex Lin-end
        #endif

        //debug_msg("^Gq0\r\n");
        //close last app mode
        Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, SYS_MODE_UNKNOWN);
        //debug_msg("^Gq1\r\n");

        //#NT#2013/01/23#Lincy Lin -begin
        //#NT#Add low battery, lens error power off tone and screen
        if(System_GetState(SYS_STATE_POWEROFF) == SYS_POWEROFF_BATTEMPTY)
        {
            DBG_ERR("low battery **********\r\n");
            #if(_USR_WND_==ENABLE)
            #if (UI_STYLE==UI_STYLE_DEMO)
            if(!gIsUSBChargePreCheck)
            {
                UISound_Play(PLAYSOUND_SOUND_BATT_WARNING_TONE);
                SwTimer_DelayMs(1000);
                Ux_OpenWindow(&DialogWarnCtrl, 2, WARN_BATT_EMPTY, WARN_ALWAYS);
                Ux_Redraw();
                SwTimer_DelayMs(2000);
            }
            #endif
            #endif
        }
        else if(System_GetState(SYS_STATE_POWEROFF) == SYS_POWEROFF_LENSERROR)
        {
            #if(_USR_WND_==ENABLE)
            #if (UI_STYLE==UI_STYLE_DEMO)
            Ux_OpenWindow(&DialogWarnCtrl, 2, WARN_LENS_ERROR, WARN_ALWAYS);
            Ux_Redraw();
            GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,TRUE);
            UISound_Play(PLAYSOUND_SOUND_WARNING_TONE);
            SwTimer_DelayMs(1500);
            UISound_Play(PLAYSOUND_SOUND_WARNING_TONE);
            SwTimer_DelayMs(1500);
            GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,FALSE);
            #endif
            #endif
        }
        //#NT#2013/01/23#Lincy Lin -end
        else
        {
            #if (POWEROFFLOGO_FUNCTION == ENABLE)
            GxDisplay_Set(LAYER_OSD1, LAYER_STATE_ENABLE, 0); //turn off OSD1
            GxDisplay_Set(LAYER_OSD2, LAYER_STATE_ENABLE, 0); //turn off OSD2
            GxDisplay_Flush(LAYER_OSD1);
            GxDisplay_Flush(LAYER_OSD2);
            GxDisplay_Set(LAYER_VDO1, LAYER_STATE_ENABLE, 0); //turn off VDO1
            GxDisplay_Flush(LAYER_VDO1);
	     if(SysGetFlag(FL_OPENING_LOGO)==OPENING_LOGO_ON)
	     {
		  	if(FALSE==UI_ShowLogoFile(UI_LOGO_POWEROFF,&uiJpgAddr,&uiJpgSize))
		  	{
            			Display_ShowSplash(SPLASH_POWEROFF);
			}
	      }
	     else
	     {
            		Display_ShowSplash(SPLASH_POWEROFF);
	     }
  	   if((GPIOMap_IsLCDBacklightOn()==FALSE)&&
  	      (KeyScan_GetPlugDev() != 1)&&(KeyScan_GetPlugDev() != 2))
  	    {
  		GPIOMap_TurnOnLCDBacklight();
  		System_ResetPowerSaveCount();		    			
  	    }			 
            SwTimer_DelayMs(500);
            #endif
        }
        //#NT#2014/06/13#KS Hung -begin
        //#NT#For NT96650, SDIO2 pin is shared with SPI flash.
        #if ( (_DUAL_CARD_FUNC_ == ENABLE) && (_CHIP_ == _CHIP_650_) )
        if (SysGetFlag(FL_DualCardMain) == MAIN_CARD_2)
        {
            GxStrg_CloseDevice(0); // Release bus to SPI using
        }
        System_OnStrgInit_PS();
        #endif
        //#NT#2014/06/13#KS Hung -end
        Save_MenuInfo();
        //debug_msg("^Gq2\r\n");
        #if (UI_STYLE==UI_STYLE_DEMO)
        //debug_msg("^Gq3\r\n");
            #if (POWEROFFSOUND_FUNCTION ==  ENABLE)
            {
                UISound_Play(PLAYSOUND_SOUND_POWEROFF_TONE);
            }
            #endif
        #endif
        //debug_msg("^Gq4\r\n");
        UI_LockEvent();

        UI_Quit(); //quit from event loop
        //debug_msg("^Gq5\r\n");
    }
    if(paramNum == 1 && paramArray[0] == 1) //power-off end
    {
        //GxSystem_ShutDownEnd();
        //bQuit = TRUE;
    }
    return NVTEVT_CONSUME;
}

INT32 System_OnMode(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    int mode_id;
    if(paramNum < 1)
        return NVTEVT_CONSUME;

    mode_id = paramArray[0];
    System_ChangeMode(mode_id);

    return NVTEVT_CONSUME;
}

INT32 System_OnSleepLevel(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    int sleep_lv;
    if(paramNum < 1)
        return NVTEVT_CONSUME;

    sleep_lv = paramArray[0];
    System_ChangeSleep(sleep_lv);

    return NVTEVT_CONSUME;
}

INT32 System_OnAppOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //call USER_MODE close
    //  1. Ux_CloseWindow();
    //  2. Ux_SendEvent(0,NVTEVT_EXE_CLOSE,paramNum,paramArray);
    //         System_ChangeMode(NULL, FALSE); //close mode
    //  3. Ux_SetActiveApp(0);
    //  4. wait mode close finish
    //         DevCtrl();

    return NVTEVT_CONSUME;
}

INT32 System_OnAppClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //call USER_MODE open
    //  1. DevCtrl();
    //  2. Ux_SetActiveApp(&AppObj);
    //  3. Ux_SendEvent(0,NVTEVT_EXE_OPEN,paramNum,paramArray);
    //         System_ChangeMode(mode, FALSE); //open mode
    //             wait device control finish
    //  4. Ux_OpenWindow();
    //         wait mode open finish

    return NVTEVT_CONSUME;
}

void Timer_CB(UINT32 event, UINT32 param1, UINT32 param2)
{
    Ux_PostEvent(NVTEVT_TIMER, 1, event);
}

///////////////////////////////////////////////////////////////////////////////
//Device flow event
INT32 System_OnVideoInsert(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnVideoRemove(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnVideoAttach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnVideoDetach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnAudioInsert(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnAudioRemove(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnStrgChange(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnAudioAttach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnAudioDetach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnStrgInsert(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnStrgRemove(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnStrgAttach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnStrgDetach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnUsbInsert(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnUsbRemove(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnUsbChargeCurrent(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnBattEmpty(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnBattChange(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnPwrChargeOK(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnPwrChargeSuspend(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnPwrChargeResume(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);

INT32 System_OnVideoDir(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);
INT32 System_OnVideoMode(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray);

/*
INT32 System_OnDefAttach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //  PWR-KEY:
    //  NORMAL-KEY:
    //  STATUS-KEY:
    //  TOUCH:
    //  LED:
    //  BATTERY:
    return NVTEVT_CONSUME;
}

INT32 System_OnSensorAttach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //  SENSOR
    return NVTEVT_CONSUME;
}

INT32 System_OnLensAttach(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //  LENS
    return NVTEVT_CONSUME;
}
*/

EVENT_ENTRY SystemObjCmdMap[] =
{
////////////////////////////////////////////////////////////

    //System flow event
    {NVTEVT_SYSTEM_BOOT,             System_OnBoot              },
    {NVTEVT_SYSTEM_SHUTDOWN,         System_OnShutdown          },
    {NVTEVT_SYSTEM_MODE,             System_OnMode              },
    {NVTEVT_SYSTEM_SLEEP,            System_OnSleepLevel        },
    //App common event
    {NVTEVT_EXE_OPEN,                System_OnAppOpen           },
    {NVTEVT_EXE_CLOSE,               System_OnAppClose          },

    //Video device event
    {NVTEVT_VIDEO_INSERT,            System_OnVideoInsert       },
    {NVTEVT_VIDEO_REMOVE,            System_OnVideoRemove       },
    {NVTEVT_VIDEO_ATTACH,            System_OnVideoAttach       },
    {NVTEVT_VIDEO_DETACH,            System_OnVideoDetach       },
    {NVTEVT_VIDEO_MODE,              System_OnVideoMode         },
    {NVTEVT_VIDEO_DIR,               System_OnVideoDir          },
    //Audio device event
    {NVTEVT_AUDIO_INSERT,            System_OnAudioInsert       },
    {NVTEVT_AUDIO_REMOVE,            System_OnAudioRemove       },
    {NVTEVT_AUDIO_ATTACH,            System_OnAudioAttach       },
    {NVTEVT_AUDIO_DETACH,            System_OnAudioDetach       },
    //Storage device event
    {NVTEVT_STRG_INSERT,             System_OnStrgInsert        },
    {NVTEVT_STRG_REMOVE,             System_OnStrgRemove        },
    {NVTEVT_STRG_CHANGE,             System_OnStrgChange        },
    {NVTEVT_STRG_ATTACH,             System_OnStrgAttach        },
    {NVTEVT_STRG_DETACH,             System_OnStrgDetach        },
    //Usb device event
#if (UVC_RECORD_FUNC != ENABLE)
    {NVTEVT_USB_INSERT,              System_OnUsbInsert         },
    {NVTEVT_USB_REMOVE,              System_OnUsbRemove         },
#endif
    {NVTEVT_USB_CHARGE_CURRENT,      System_OnUsbChargeCurrent  },
    //power device event
    {NVTEVT_PWR_BATTEMPTY,           System_OnBattEmpty         },
    {NVTEVT_PWR_BATTCHANGE,          System_OnBattChange        },
    {NVTEVT_PWR_CHARGE_OK,           System_OnPwrChargeOK       },
    {NVTEVT_PWR_CHARGE_SUSPEND,      System_OnPwrChargeSuspend  },
    {NVTEVT_PWR_CHARGE_RESUME,       System_OnPwrChargeResume   },

    //Other device event
    //{NVTEVT_DEFAULT_ATTACH,        System_OnDefAttach         },
    //{NVTEVT_SENSOR_ATTACH,         System_OnSensorAttach      },
    //{NVTEVT_LENS_ATTACH,           System_OnLensAttach        },

    {NVTEVT_NULL,                    0},  //End of Command Map
};

CREATE_APP(SystemObj, 0)

