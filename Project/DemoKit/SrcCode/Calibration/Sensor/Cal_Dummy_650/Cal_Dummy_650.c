#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    "kernel.h"
#include    "sensor.h"
#include    "debug.h"
#include    "utility.h"
#include    "CalibrationInt.h"
#include    "Cal_Dummy_650.h"
#include    "Syscfg.h"
//#include    "UIFlow.h"
#if(UI_RESOLUTION==UI_RESOLUTION_D800x480)
#include "../../../UIWnd/D800x480/UIFlow.h"
#elif(UI_RESOLUTION==UI_RESOLUTION_D480x272)
#include "../../../UIWnd/D480x272/UIFlow.h"
#elif(UI_RESOLUTION==UI_RESOLUTION_TOUCH_800x480)
#include "../../../UIWnd/T800x480/UIFlow.h"
#else
#include "../../../UIWnd/DRIVER/UIFlow.h"
#endif
#include    "Phototask.h"

ER Cal_PS2MemPool(void)
{
    return E_OK;
}

UINT32 Cal_WiteDebug_Info(UINT32 Addr)
{
    return Addr;
}


ER Cal_SenISO(void)
{
    return E_OK;
}

ER Cal_SenMShutter(void)
{
    return E_OK;
}

ER Cal_SenDP(void)
{
    return E_OK;
}


ER Cal_SenDP_Bright(void)
{
    return E_OK;
}

ER Cal_SenDP_Dark(void)
{
    return E_OK;
}


ER Cal_SenAWB(void)
{
    return E_OK;
}

ER Cal_SenAWBGS(void)
{
    return E_OK;
}

ER Cal_SenECS(void)
{
    return E_OK;
}

ER Cal_Get_DNP_Status(void)
{
    return E_OK;
}

