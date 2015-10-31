#ifndef _UICONFIG_H
#define _UICONFIG_H

//#include "UIFlow.h"
#if(UI_RESOLUTION==UI_RESOLUTION_D800x480)
#include "UIWnd/D800x480/UIFlow.h"
#elif(UI_RESOLUTION==UI_RESOLUTION_D480x272)
#include "UIWnd/D480x272/UIFlow.h"
#elif(UI_RESOLUTION==UI_RESOLUTION_TOUCH_800x480)
#include "UIWnd/T800x480/UIFlow.h"
#else
#include "UIWnd/DRIVER/UIFlow.h"
#endif
#define PS_SYS_PARAM        "SYSP"
#define PS_BG_CFG           "CFGP"
#define PS_POWERON_LOGO     "ONLOGO"
#define PS_POWEROFF_LOGO    "OFFLOGO"

#define UICONFIG_STR_LEN    32

#define UI_LOGO_POWERON        0
#define UI_LOGO_POWEROFF       1


typedef struct _UIConfigInfo
{
    /*--- General ---*/
    CHAR        strMakerString[UICONFIG_STR_LEN];
    CHAR        strModelString[UICONFIG_STR_LEN];
    CHAR        strSoftwareVer[UICONFIG_STR_LEN];
    CHAR        strMakeNote[UICONFIG_STR_LEN];
    CHAR        strImageDescription[UICONFIG_STR_LEN];

    UINT32      uiSizeDefaultIndex;
    UINT32      uiBeepDefaultIndex;
    UINT32      uiMovieSizeDefaultIndex;
    UINT32      uiLanguageDefaultIndex;
    UINT32      uiFreqDefaultIndex;
    UINT32      uiVideoAudioDefaultIndex;
    UINT32      uiTVModeDefaultIndex;
    UINT32      uiDateImprintDefaultIndex;
} UIConfigInfo;

#if((_MODEL_DSC_ == _MODEL_DUAL_BMWCABLE_)||(_MODEL_DSC_==_MODEL_DUAL_G2800_)||(_MODEL_DSC_ == _MODEL_DUAL_JIACHANGLING_)\
	||(_MODEL_DSC_==_MODEL_DUAL_QIZHENG_M1_)||(_MODEL_DSC_ == _MODEL_DUAL_XWH_)||(_MODEL_DSC_ == _MODEL_DUAL_SAIBOSHI_))	     
#define   USB_SOURCE_POWERON                      GX_PWRON_SRC_PWR_VBUS//GX_PWRON_SRC_PWR_VBAT
#define   MANUALKEY_SOURCE_POWERON		GX_PWRON_SRC_PWR_SW
#define   GSENSOR_TRIGGER_SOURCE_POWERON   GX_PWRON_SRC_PWR_SW2
#define   ACC_SOURCE_POWERON   GX_PWRON_SRC_PWR_VBAT//GX_PWRON_SRC_PWR_VBUS
#elif (_MODEL_DSC_ == _MODEL_CARDV_GT200_)	     
#define   USB_SOURCE_POWERON                      GX_PWRON_SRC_PWR_VBAT
#define   MANUALKEY_SOURCE_POWERON		GX_PWRON_SRC_PWR_SW
#define   GSENSOR_TRIGGER_SOURCE_POWERON   GX_PWRON_SRC_PWR_SW2
#elif (_MODEL_DSC_ == _MODEL_KERNAL_BOARD_880_)	 
#define   USB_SOURCE_POWERON                      GX_PWRON_SRC_PWR_VBUS//GX_PWRON_SRC_PWR_VBAT
#define   MANUALKEY_SOURCE_POWERON		GX_PWRON_SRC_PWR_SW
#define   GSENSOR_TRIGGER_SOURCE_POWERON   GX_PWRON_SRC_PWR_SW2
#define   ACC_SOURCE_POWERON   GX_PWRON_SRC_PWR_VBAT//GX_PWRON_SRC_PWR_VBUS
#elif (_MODEL_DSC_ == _MODEL_CARDV_A8_)	 
#define   USB_SOURCE_POWERON                      GX_PWRON_SRC_PWR_VBAT
#define   MANUALKEY_SOURCE_POWERON		GX_PWRON_SRC_PWR_SW
#define   GSENSOR_TRIGGER_SOURCE_POWERON   GX_PWRON_SRC_PWR_SW2
#else
#define   USB_SOURCE_POWERON                      GX_PWRON_SRC_PWR_VBUS
#define   MANUALKEY_SOURCE_POWERON		GX_PWRON_SRC_PWR_SW
#define   GSENSOR_TRIGGER_SOURCE_POWERON   GX_PWRON_SRC_PWR_SW2
#endif

extern  BOOL UI_LoadCfgFile(void);
extern  void UI_UpdateCfgFile(void);
extern  void UI_UpdateLogoFile(void);
extern  BOOL UI_ShowLogoFile(UINT32 logoId,UINT32 *uiJpgAddr,UINT32 *uiJpgSize);

extern CHAR *GetMakerString(void);
extern CHAR *GetModelString(void);
extern CHAR *GetVersionString(void);
extern CHAR *GetUIConfigSoftwareVer(void);
extern CHAR *GetUIConfigMakerString(void);
extern CHAR *GetUIConfigModelString(void);
extern CHAR *GetUIConfigMakeNote(void);
extern CHAR *GetUIConfigImageDescription(void);


extern UINT32 UI_GetSizeDefaultIndex(void);
extern UINT32 UI_GetMovieSizeDefaultIndex(void);
extern UINT32 UI_GetLanguageDefaultIndex(void);
extern UINT32 UI_GetFreqDefaultIndex(void);
extern UINT32 UI_GetTVModeDefaultIndex(void);
extern UINT32 UI_GetDateImprintDefaultIndex(void);
extern UINT32 UI_GetBeepDefaultIndex(void);
extern UINT32 UI_GetVideoAudioDefaultIndex(void);

extern void Calibration_InstallID(void) _SECTION(".kercfg_text");
extern UINT32 _SECTION(".kercfg_data") FLG_ID_CALIBRATION;

#endif
