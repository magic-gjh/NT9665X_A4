/*
    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.

    @file       AppCommon.h
    @ingroup    mIPRJAPUIFlow

    @brief      App Common Interfaces
                This file is the user interface ( for LIB callback function).

    @note       Nothing.

    @date       2005/04/01
*/

#ifndef _APPCOMMON_H
#define _APPCOMMON_H

//Built-in App Object
#include "AppLib.h"         //include EVENT of Common Obj
#include "UIPlayObj.h"      //include EVENT of Play Obj
#include "UIMovieObj.h"     //include EVENT of Movie Obj
#include "UIPhotoObj.h"     //include EVENT of Photo Obj
#include "UIPrinterObj.h"   //include EVENT of Printer Obj

//Mode
#include "UIMode.h"

//M (Info)
//#include "UIInfo.h"
#if(UI_RESOLUTION==UI_RESOLUTION_D800x480)
#include "../UIWnd/D800x480/UIInfo/UIInfo.h"
#elif(UI_RESOLUTION==UI_RESOLUTION_D480x272)
#include "../UIWnd/D480x272/UIInfo/UIInfo.h"
#elif(UI_RESOLUTION==UI_RESOLUTION_TOUCH_800x480)
#include "../UIWnd/T800x480/UIInfo/UIInfo.h"
#else
#include "../UIWnd/DRIVER/UIInfo/UIInfo.h" 
#endif
#include "ProjectInfo.h"
#include "UIMovieInfo.h"
#include "UIPhotoInfo.h"
#include "UIPlayInfo.h"
#include "UIUsbPrintInfo.h"
#include "UIMovieMapping.h"
#include "UIPhotoMapping.h"
#include "UIPlayMapping.h"
#include "DateTimeInfo.h"
#include "TPInfo.h"
#include "GSInfo.h"
//V (App)
#include "UIAppMovie.h"      //include EVENT of Movie App
#include "UIAppPhoto.h"      //include EVENT of Photo App
#include "UIAppPlay.h"       //include EVENT of Play App
#include "UIAppUsbCam.h"
#include "UIAppUsbCharge.h"
#include "UIAppUsbDisk.h"
#include "UIAppUsbPrint.h"   //include EVENT of Print App
#include "UIBackgroundObj.h" //include EVENT of Worker Task
#include "UISetup.h"         //include EVENT of System Setup

#endif //_APPCOMMON_H_

