/**
    Copyright   Novatek Microelectronics Corp. 2007.  All rights reserved.

    @file       BuildDateInfo.c
    @ingroup    mIPRJAPCfg

    @brief      Project source build time information
                Project source build time information

    @note       The project source build time info will be updated every time you build the image.

    @date       2007/11/06
*/

/** \addtogroup mIPRJAPCfg*/
//@{

#include <stdio.h>
#include "ProjectInfo.h"
#include "Type.h"
#include "PrjCfg.h"
#include <string.h>

//-----------------------------------------------------------------------------
// FW info
//-----------------------------------------------------------------------------
//FW updtae FW need access external fw version via versioninfo.s offset 0x000000A8
//char gFWExternalVersion[33]="PW84.TVP5150.2015.03.12";
//char gFWExternalVersion[33]="PW84.TVP5150.2015.03.23";
//char gFWExternalVersion[33]="PW8401.2015.04.15.V0.01";
//char gFWExternalVersion[33]="PW8401.2015.04.24.V0.01";
//char gFWExternalVersion[33]="PW8401.2015.04.28.V0.01";
//char gFWExternalVersion[33]="PW8401.2015.05.06.V0.01.IQ01";
#if (_MODEL_DSC_ == _MODEL_DUAL_V18_DT_)	     
char gFWExternalVersion[33]="V18.2015.0619.V0.1";
#elif (_MODEL_DSC_ == _MODEL_DUAL_V18_LYSZ_)	     
char gFWExternalVersion[33]="V18.2015.0709.V0.1";
#elif (_MODEL_DSC_ == _MODEL_CARDV_B50_)	
//char gFWExternalVersion[33]="B50.2015.0521.V0.3";
//char gFWExternalVersion[33]="B50.2015.0619.V0.3";
char gFWExternalVersion[33]="B50.2015.0722.V0.3";
#elif(_MODEL_DSC_ == _MODEL_DUAL_HLYD_)
char gFWExternalVersion[33]="HLYD.2015.0624.V0.1";
#elif(_MODEL_DSC_ == _MODEL_CARDV_CHINA_ELECTRONICS_)
char gFWExternalVersion[33]="H510.2015.0601.V0.1";
#elif(_MODEL_DSC_ == _MODEL_DUAL_G2800_)
//char gFWExternalVersion[33]="G2800.2015.0619.V0.2";
//char gFWExternalVersion[33]="G2800.2015.0727.V1.0";
//char gFWExternalVersion[33]="G2800.2015.0730.V1.0";
//char gFWExternalVersion[33]="G2800.2015.0731.V1.1";
//char gFWExternalVersion[33]="G2800.2015.0804.V1.1";
char gFWExternalVersion[33]="G2800.2015.0806.V1.5";

#elif (_MODEL_DSC_ == _MODEL_DUAL_BMWCABLE_)	
char gFWExternalVersion[33]="B50.2015.0521.V0.3";
#elif (_MODEL_DSC_ == _MODEL_DUAL_F5_)	
char gFWExternalVersion[33]="8401F5.2015.0716.V0.1";
#elif(_MODEL_DSC_ == _MODEL_CARDV_S650_)
char gFWExternalVersion[33]="SAST.2015.0720.V0.1";
#elif(_MODEL_DSC_ == _MODEL_DUAL_YITU_)
char gFWExternalVersion[33]="YITU.2015.0619.V0.1";
#elif(_MODEL_DSC_==_MODEL_DUAL_NAZHIDA_)
char gFWExternalVersion[33]="NAZHIDA.2015.0708.V0.1";
#elif(_MODEL_DSC_==_MODEL_CARDV_GT200_)
char gFWExternalVersion[33]="GT200.2015.0702.V0.1";
#elif(_MODEL_DSC_==_MODEL_DUAL_JIACHANGLING_)
char gFWExternalVersion[33]="WEIGA.2015.0619.V0.1";
#elif(_MODEL_DSC_ == _MODEL_DUAL_QIZHENG_M1_)
char gFWExternalVersion[33]="QZM1.2015.0624.V0.1";
#elif(_MODEL_DSC_ == _MODEL_DUAL_XWH_)
char gFWExternalVersion[33]="XWH.2015.0625.V0.2";
#elif(_MODEL_DSC_ == _MODEL_DUAL_ZHUOHONGCHUANG_)
char gFWExternalVersion[33]="8401.2015.0708.V0.1";
#elif(_MODEL_DSC_ == _MODEL_DUAL_790S_)
//char gFWExternalVersion[33]="SAST.2015.0803.V0.1";
char gFWExternalVersion[33]="SAST.2015.0804.V0.1";
#elif(_MODEL_DSC_ == _MODEL_CARDV_A8_)
//char gFWExternalVersion[33]="A8.2015.0717.V0.1";
char gFWExternalVersion[33]="A8.2015.0731.V0.1";
#elif(_MODEL_DSC_ == _MODEL_CARDV_A10_)
char gFWExternalVersion[33]="A10.IMX322.2015.0716.V0.1";
#elif(_MODEL_DSC_==_MODEL_DUAL_89M3_)
char gFWExternalVersion[33]="M3.2015.0803.V0.2";
#elif(_MODEL_DSC_==_MODEL_DUAL_D333_)
char gFWExternalVersion[33]="D333.2015.0730.V0.1";
#elif(_MODEL_DSC_ == _MODEL_CARDV_K12_)
char gFWExternalVersion[33]="k12.2015.0730.V0.1";
#elif(_MODEL_DSC_ == _MODEL_DUAL_SAIBOSHI_)
char gFWExternalVersion[33]="SAIBOSHI.2015.0731.V0.1";
#elif(_MODEL_DSC_ == _MODEL_DUAL_AONI328_)
char gFWExternalVersion[33]="328.2015.0731.V0.1";
#else
//char gFWExternalVersion[33]="8401.2015.0727.V0.1";
char gFWExternalVersion[33]="A4.2015.1109.V1.0";
#endif

char* Prj_GetVersionString()
{
    return gFWExternalVersion;
}


//-----------------------------------------------------------------------------
// User define info
//-----------------------------------------------------------------------------
static char gVersion[9];
static char gModelName[9];
static char gReleaseDate[9];
//extern UINT32 ProjectCode;
//extern UINT32 VersionNumber;
//extern UINT32 ReleaseDate;

//Please modify VersionInfo.s :: ProjectCode
char* Prj_GetModelInfo()
{
    //strncpy(gModelName, (char *)&ProjectCode, 9);
    gModelName[8] = 0;
    return gModelName;
}

//Please modify VersionInfo.s :: VersionNumber
char* Prj_GetVerInfo()
{
    //strncpy(gVersion, (char *)&VersionNumber, 9);
    gVersion[8] = 0;
    return gVersion;
}

//Please modify VersionInfo.s :: ReleaseDate
char* Prj_GetReleaseDate()
{
    //strncpy(gReleaseDate, (char *)&ReleaseDate, 9);
    gReleaseDate[8] = 0;
    return gReleaseDate;
}


//-----------------------------------------------------------------------------
// Compiler-time info
//-----------------------------------------------------------------------------
#include "ProjectInfo.cfg"

// C standard format: Mmm dd yyyy HH:MM:SS (ex. Nov 06 2007 11:15:11)
static CHAR gPrjBuildDateStd[] = __DATE__ ", " __TIME__;

// User defined format: yyyy/mm/dd HH:MM:SS (ex. 2007/11/06 11:15:11)
static CHAR gPrjBuildDateUser[32];

// Retrieve year info
#define OS_YEAR     ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
                        + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

// Retrieve month info
#define OS_MONTH    (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
                        : __DATE__ [2] == 'b' ? 2 \
                        : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
                        : __DATE__ [2] == 'y' ? 5 \
                        : __DATE__ [2] == 'l' ? 7 \
                        : __DATE__ [2] == 'g' ? 8 \
                        : __DATE__ [2] == 'p' ? 9 \
                        : __DATE__ [2] == 't' ? 10 \
                        : __DATE__ [2] == 'v' ? 11 : 12)

// Retrieve day info
#define OS_DAY      ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
                        + (__DATE__ [5] - '0'))

// Retrieve hour info
#define OS_HOUR     ((__TIME__ [0] - '0') * 10 + (__TIME__ [1] - '0'))

// Retrieve minute info
#define OS_MINUTE   ((__TIME__ [3] - '0') * 10 + (__TIME__ [4] - '0'))

// Retrieve second info
#define OS_SECOND   ((__TIME__ [6] - '0') * 10 + (__TIME__ [7] - '0'))




//-----------------------------------------------------------------------------
// Project build time info, C standard (US) format
//-----------------------------------------------------------------------------
char* Prj_GetBuildDateStd(void)
{
    return gPrjBuildDateStd;
}

//-----------------------------------------------------------------------------
// Project build time info, user defined format
//-----------------------------------------------------------------------------
char* Prj_GetBuildDateUser(void)
{
    sprintf(gPrjBuildDateUser, "%d/%02d/%02d, %s", OS_YEAR, OS_MONTH, OS_DAY, __TIME__);
    return gPrjBuildDateUser;
}

char* Prj_GetCheckinDate(void)
{
    return gPrjCheckInDate;
}




//@}
