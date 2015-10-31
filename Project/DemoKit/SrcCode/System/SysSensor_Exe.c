/*
    System Sensor Callback

    System Callback for Sensor Module.

    @file       SysSensor_Exe.c
    @ingroup    mIPRJSYS

    @note

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/

////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "UIFrameworkExt.h"
#include "AppLib.h"
#if (AUTO_BACKLIGHT_FUNCTION == ENABLE)
#include "UIAppPhoto.h" //for CURR_EV()
#include "MenuSetupLCDBrt.h" //for MenuSetup_GetV2BacklightValue()
#endif
#include "GxFlash.h"
#include "DxSensor.h"
#include "IPL_CALRW.h"

//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         1 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysSensorExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

#if (AUTO_BACKLIGHT_FUNCTION == ENABLE)
int SX_TIMER_DET_EV_ID = -1;
void Sensor_DetEV(void);
SX_TIMER_ITEM(Sensor_DetEV, Sensor_DetEV, 50, TRUE)
#endif
int SX_TIMER_DET_FLASH_ID = -1;
int SX_TIMER_DET_RECHARGE_ID = -1;
SX_TIMER_ITEM(GxFlash_DetFlash, GxFlash_DetFlash, 5, TRUE)
SX_TIMER_ITEM(GxFlash_DetReCharge, GxFlash_DetReCharge, 50, TRUE)
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
int SX_TIMER_DET_EXTSENSOR_ID = -1;
void Sensor_DetExtSensor(void);
SX_TIMER_ITEM(Sensor_DetExtSensor, Sensor_DetExtSensor, 10, TRUE)
#endif
extern BOOL gbNeedToRecordMovie;
//void DscCommon_Open(void);
void IPLCtrl_Open(void);
void IPLCtrl_Close(void);

//void Sensor_RegCmdTab(void);

void System_OnSensorInit(void)
{
    //PHASE-1 : Init & Open Drv or DrvExt
    {
        //for command table
        //Sensor_RegCmdTab();
        //read calibration data
        {
            CAL_DMA_RST ps_rst = {0};
            ps_rst = CAL_RW()->fpSectionAllRead(OS_GetMempoolAddr(POOL_ID_CAL),POOL_SIZE_CAL);
        }
    }
    //PHASE-2 : Init & Open Lib or LibExt
    {
        //DscCommon_Open();
        //IPLCtrl_Open();
        #if (AUTO_BACKLIGHT_FUNCTION == ENABLE)
        SX_TIMER_DET_EV_ID = SxTimer_AddItem(&Timer_Sensor_DetEV);
        #endif
        SX_TIMER_DET_FLASH_ID = SxTimer_AddItem(&Timer_GxFlash_DetFlash);
        SX_TIMER_DET_RECHARGE_ID = SxTimer_AddItem(&Timer_GxFlash_DetReCharge);
        #if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
        SX_TIMER_DET_EXTSENSOR_ID = SxTimer_AddItem(&Timer_Sensor_DetExtSensor);
        #endif
    }
}

void System_OnSensorExit(void)
{
    //PHASE-2 : Close Lib or LibExt
    {
        //IPLCtrl_Close();
    }
    //PHASE-1 : Close Drv or DrvExt
    {
    }
}

#if (AUTO_BACKLIGHT_FUNCTION == ENABLE)
extern BOOL g_bIsInitSystemFinish;

void Sensor_DetEV(void)
{
    //check is enable to AUTO adjust?
    if(UI_GetData(FL_BrightAdjIndex) !=  LCDBRT_ADJ_AUTO)
        return;
    //Sleep not control backlight
    if(GxPower_GetControl(GXPWR_CTRL_SLEEP_LEVEL) > 0)
        return;
    //Playback mode can not get LV value
    if(UI_GetData(FL_ModeIndex) == DSC_MODE_PLAYBACK)
        return;

    {
    UINT32 uiLV;
    uiLV = CURR_EV()/10;
    //#NT#2011/03/31#Lincy Lin -begin
    //#NT#
    if (g_bIsInitSystemFinish )
    {
        //#NT#2011/04/09#Lincy Lin -begin
        //#NT#
        //DetBLcount++;
        //if (DetBLcount > 2)
        //#NT#2011/04/09#Lincy Lin -end
        {
            //debug_msg("CURR_EV()=%d\r\n", CURR_EV());
            //debug_msg("LV=%d\r\n", uiLV);
            //debug_msg(" set bright=%d\r\n",MenuSetup_GetV2BacklightValue(uiLV));
            Ux_SendEvent(&UISetupObjCtrl,NVTEVT_EXE_BRIGHTLVL,1,MenuSetup_GetV2BacklightValue(uiLV));
        }
    }
    //#NT#2011/03/31#Lincy Lin -end
    }
}
#endif

#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
extern void Movie_SetDualRec(BOOL bOn);

#define DET_EXTSENSOR_DEBOUNCE_COUNT    1

static BOOL g_bDetExtSensor = FALSE;    // ext. sensor detected or not

BOOL Sensor_CheckExtSensor(void)
{
    static BOOL bFirstTime = TRUE;
    BOOL bDetExtSensor = DrvSensor_Det2ndSensor() ? TRUE : FALSE;

    if (bFirstTime)
    {
        bFirstTime = FALSE;
        g_bDetExtSensor = bDetExtSensor;
    }

    DBG_DUMP("^YSensor_CheckExtSensor: %d\r\n", bDetExtSensor);
    return bDetExtSensor;
}

void Sensor_DetExtSensor(void)
{
    static UINT32 bDebounceCount = 0;
	static UINT8 SecondSensorflag = 0;
#if 0//(_MODEL_DSC_ == _MODEL_DUAL_790S_)
    if (System_GetState(SYS_STATE_CURRMODE) != PRIMARY_MODE_MOVIE)
        return;
#endif
    // check if external sensor is connected
    if (g_bDetExtSensor == FALSE)
    {
        if (DrvSensor_Det2ndSensor())
        {
            bDebounceCount++;
            if (bDebounceCount > DET_EXTSENSOR_DEBOUNCE_COUNT)
            {
            	SecondSensorflag++;
                g_bDetExtSensor = TRUE;
				if(SecondSensorflag == 1)
				{
					gbNeedToRecordMovie = TRUE;
				}
				else
				{
					SecondSensorflag = 2;		
				}
                bDebounceCount = 0;

                // re-enter movie mode
#if 1//(_MODEL_DSC_ == _MODEL_DUAL_790S_)                
    		Ux_SendEvent(&UISetupObjCtrl,NVTEVT_EXE_CHANGEDSCMODE,1,DSCMODE_CHGTO_CURR);
#else
                Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MOVIE);
#endif
            }
        }
        else
        {
            bDebounceCount = 0;
        }
    }
    // check if external sensor is disconnected
    else
    {
        if (DrvSensor_Det2ndSensor() == FALSE)
        {
            bDebounceCount++;
            if (bDebounceCount > DET_EXTSENSOR_DEBOUNCE_COUNT)
            {
                g_bDetExtSensor = FALSE;
                bDebounceCount = 0;
				SecondSensorflag = 0;

                // re-enter movie mode
#if 1//(_MODEL_DSC_ == _MODEL_DUAL_790S_)                
    		Ux_SendEvent(&UISetupObjCtrl,NVTEVT_EXE_CHANGEDSCMODE,1,DSCMODE_CHGTO_CURR);
#else
                Ux_PostEvent(NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_MOVIE);
#endif
            }
        }
        else
        {
            bDebounceCount = 0;
        }
    }
}
#endif
