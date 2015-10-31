#include "GSensor.h"
#include "GSensor_Int.h"
#include "Gsensor_GMA301.h"
#include "SwTimer.h"
#include "Debug.h"
#include <stdlib.h>
#include "ErrorNo.h"
#include "Delay.h"
#include "Rtc.h"
//********************************************************************************
//
//GSENSOR DRA07
//
//********************************************************************************
static INT32 GsXData = 0,OldGsXData = 0;
static INT32 GsYData = 0,OldGsYData = 0;
static INT32 GsZData = 0,OldGsZData = 0;
static BOOL  g_bGsensorOpened = FALSE;
static GSENSOR_SENSITIVITY g_GSensorSensitivity = GSENSOR_SENSITIVITY_OFF;
static BOOL gbCrashMode=FALSE;

static GSENSOR_OBJ g_GsensorGMA301Obj = {GSensor_GMA301_open, GSensor_GMA301_close, GSensor_GMA301_GetStatus, 
	GSensor_GMA301_ParkingMode, GSensor_GMA301_CrashMode, GSensor_GMA301_SetSensitivity,
	GSensor_GMA301_ClearCrashMode,GSensor_GMA301_SetInterrupt};


BOOL GSensor_GMA301_CrashMode(void)
{
    //debug_msg("GSensor_GMA301_CrashMode:%d..\r\n",gbCrashMode);
    return gbCrashMode;
}

void GSensor_GMA301_ClearCrashMode(void)
{
    debug_msg("GSensor_GMA301_ClearCrashMode..\r\n");
    GSensor_I2C_ReadReg(0x1C);
    GSensor_I2C_ReadReg(0x1D);	
    gbCrashMode=FALSE;
}

void GSensor_GMA301_SetSensitivity(GSENSOR_SENSITIVITY GSensorSensitivity)
{
    g_GSensorSensitivity = GSensorSensitivity;
}

//**********************************************************************************
//
//
//**********************************************************************************
static GSENSOR_INFO2 g_GsensorInfo;
static UINT32 GMA301Threshold=0xFF;/*1G= X/16;*/

static BOOL GSensor_Read_Interrupte();

static UINT32 GSensor_GMA301_ReadReg_2Byte(UINT32 uiAddr, UINT32 uidata)
{
	UINT32      ulWriteAddr, ulReadAddr, ulReg1;
	static UINT32 ulData1,ulData2;

	ulWriteAddr =0x30;
	ulReadAddr  =0x31;
	ulReg1      =(uiAddr&0x000000ff);
	ulData1     = 0;
	ulData2 = 0;
	//debug_err((">>> read register\r\n"));
    if(i2c_lock(g_GsensorInfo.I2C_Channel) != E_OK)
    {
        debug_err(("GSensor: readReg I2C Lock Fail\r\n"));
        return ulData1;
    }

    if (GSensor_I2C_Transmit(ulWriteAddr, 1, 0) != I2C_STS_OK)
    {
        debug_err(("Error transmit data1!!\r\n"));
        i2c_unlock(g_GsensorInfo.I2C_Channel);
        return  ulData1;
    }

    if (GSensor_I2C_Transmit(ulReg1, 0, 0) != I2C_STS_OK)
    {
        debug_err(("Error transmit data2!!\r\n"));
        i2c_unlock(g_GsensorInfo.I2C_Channel);
        return  ulData1;
    }

    if (GSensor_I2C_Transmit(ulReadAddr, 1, 0) != I2C_STS_OK)
    {
        debug_err(("Error transmit data3!!\r\n"));
        i2c_unlock(g_GsensorInfo.I2C_Channel);
        return  ulData1;
    }

    if (GSensor_I2C_Receive(&ulData1, 0, 0) != I2C_STS_OK)
    {
        debug_err(("Error Receive data!!\r\n"));
        i2c_unlock(g_GsensorInfo.I2C_Channel);
        return  ulData1;
    }

    if (GSensor_I2C_Receive(&ulData2, 1, 1) != I2C_STS_OK)
    {
        debug_err(("Error Receive data!!\r\n"));
        i2c_unlock(g_GsensorInfo.I2C_Channel);
        return  ulData1;
    }
    if(i2c_unlock(g_GsensorInfo.I2C_Channel) != E_OK)
    {
        debug_err(("GSensor: readReg I2C UnLock Fail\r\n"));
        return  ulData1;
    }
   // debug_err((">>>>>uidata = %d %d\r\n",ulData1,ulData2));

    uidata =  (ulData1<<8) + ulData2;
   //debug_err((">>>uidata = %d\r\n",uidata));
    return  uidata;
}

INT32 GSensor_GMA301_GetAxisMsg(UINT32 uiInt)
{
    INT32 Data  = 0;

    if (uiInt >511) //0x3F
    {
        //negative number
        Data =-(uiInt-511);
        debug_msg("GSensor_GMA301_GetAxisMsg1  = %d\r\n",Data);

    }
    else
    {
        Data = uiInt;
        debug_msg("GSensor_GMA301_GetAxisMsg2  = %d\r\n",Data);
    }

    return Data;//(bzNegative)?(-1*Data):(Data);

}

static UINT32 GSensor_GMA301_GetSensitivityLevel(void)
{
    switch(g_GSensorSensitivity)
   	{
		case GSENSOR_SENSITIVITY_OFF:
			GMA301Threshold =0xFF;
			break;
		case GSENSOR_SENSITIVITY_LOW:
			GMA301Threshold =0xB5;//25
			break;
		case GSENSOR_SENSITIVITY_MED:
			GMA301Threshold =0xA5;//20
			break;
		case GSENSOR_SENSITIVITY_HIGH:
			GMA301Threshold =0x85;//16
			break;
		default:
			GMA301Threshold =0xFF;
			break;
   	}
	return GMA301Threshold;
}

static void GSensor_GMA301_OpenInterface(void)
{

}

static void GSensor_GMA301_CloseInterface(void)
{

}

PGSENSOR_OBJ GSensor_GMA301_getGSensorObj(void)
{
    return &g_GsensorGMA301Obj;
}

static BOOL GSensor_GMA301_Init(void)
{
	UINT32 value = 0;

		//init
		GSensor_I2C_WriteReg(0x21,0x52);//
		GSensor_I2C_WriteReg(0x00,0x02);//
		GSensor_I2C_WriteReg(0x00,0x12);//
		GSensor_I2C_WriteReg(0x00,0x02);//
		GSensor_I2C_WriteReg(0x00,0x82);//
		GSensor_I2C_WriteReg(0x00,0x02);//
		GSensor_I2C_WriteReg(0x1F,0x28);//
		GSensor_I2C_WriteReg(0x0C,0x8F);//
		GSensor_I2C_WriteReg(0x00,0x06);//
		GSensor_I2C_WriteReg(0x0D,0x70);//
	
		//interrupt setup
#if 1		
		GSensor_I2C_WriteReg(0x11,0x04);//IIC 0X07 for no pullup //0x06 High active  0x40 low active
		//Gsensor_WriteReg(0x11,0x06);//IIC 0X06 for no pullup 
		//set Gsensor Level 
		//0x08-->0.5G 
		//0X10-->1G
		GSensor_I2C_WriteReg(0x38,0Xff);//
		GSensor_I2C_WriteReg(0x39,0Xff);//10 1g 20 2g 30 3g 40 4g 50 5g 60 6g  (8 : 0.5g)
		
		GSensor_I2C_WriteReg(0x0F,0x00);//
		GSensor_I2C_WriteReg(0x0E,0x00);//0x1C//0x00 // 0x00:disable interrupt
		GSensor_I2C_WriteReg(0x1F,0x28);//To disable micro motion interrupt.
		//TimerDelayMs(10);
		//Latched reference data of micro motion.
		GSensor_I2C_ReadReg(0x12);
		GSensor_I2C_ReadReg(0x13);
		GSensor_I2C_ReadReg(0x14);
		GSensor_I2C_ReadReg(0x15);
		GSensor_I2C_ReadReg(0x16);
		GSensor_I2C_ReadReg(0x17);
		GSensor_I2C_ReadReg(0x18);
		GSensor_I2C_ReadReg(0x19);
		//GSensor_I2C_ReadReg(0x1A);
		//GSensor_I2C_ReadReg(0x1B);	
        /*
        debug_msg("reg 0x14:%x\r\n",GSensor_I2C_ReadReg(0x14));//X-H
        debug_msg("reg 0x15:%x\r\n",GSensor_I2C_ReadReg(0x15));//X-L
        debug_msg("reg 0x16:%x\r\n",GSensor_I2C_ReadReg(0x16));//Y-H
        debug_msg("reg 0x17:%x\r\n",GSensor_I2C_ReadReg(0x17));//Y-L
        debug_msg("reg 0x18:%x\r\n",GSensor_I2C_ReadReg(0x18));//Z-H
	 debug_msg("reg 0x19:%x\r\n",GSensor_I2C_ReadReg(0x19));//Z-L
*/
#endif
		GSensor_I2C_WriteReg(0x1F,0x38);//To enable micro motion interrupt.
		Delay_DelayMs(1); //2014_0819 added 1ms delay for micro motion setup itself.
		GSensor_I2C_ReadReg(0x1C);
		GSensor_I2C_ReadReg(0x1D);
		GSensor_I2C_WriteReg(0x0E,0x00);//To disable interrupt.//parking monitor
		//GSensor_GMA301_ReadReg_2Byte(0x1C,value);
	debug_msg("GMA301 init ,get value:%x..\r\n",value);

    return TRUE;
}

//for  Gensor POWER UP
void GSensor_GMA301_PowerUp(INT32 sen)
{
	UINT32 value = 0;

	debug_msg("InitGensor_POWERDOWN 111!!\r\n");

	//interrupt setup
	GSensor_I2C_WriteReg(0x11,0x06);//IIC 0X07 for no pullup //0x06 High active  0x04 low active
	//Gsensor_WriteReg(0x11,0x06);//IIC 0X06 for no pullup
	//set Gsensor Level
	//0x08-->0.5G
	//0X10-->1G
	GSensor_I2C_WriteReg(0x38,0X00);//
	GSensor_I2C_WriteReg(0x39,sen);//10 1g 20 2g 30 3g 40 4g 50 5g 60 6g

	GSensor_I2C_WriteReg(0x0F,0x00);//
	GSensor_I2C_WriteReg(0x0E,0x00);//0x1C//0x00 // 0x00:disable interrupt
	GSensor_I2C_WriteReg(0x1F,0x28);//To disable micro motion interrupt.

	GSensor_I2C_ReadReg(0x12);
	GSensor_I2C_ReadReg(0x13);
	GSensor_I2C_ReadReg(0x14);
	GSensor_I2C_ReadReg(0x15);
	GSensor_I2C_ReadReg(0x16);
	GSensor_I2C_ReadReg(0x17);
	GSensor_I2C_ReadReg(0x18);
	GSensor_I2C_ReadReg(0x19);

	GSensor_I2C_WriteReg(0x1F,0x28);//0x38);//To enable micro motion interrupt.
	Delay_DelayMs(2); //2014_0819 added 1ms delay for micro motion setup itself.

	GSensor_I2C_WriteReg(0x0E,0x9C);//0x1C);//To enable interrupt.
	GSensor_GMA301_ReadReg_2Byte(0x1C,value);

	debug_msg("InitGensor_POWERDOWN 22222!!=%d\r\n",value);

}

void GSensor_GMA301_PowerDown(void)
{
	UINT32 value = 0;
	GSensor_I2C_WriteReg(0x21,0x52);//
	GSensor_I2C_WriteReg(0x00,0x02);//
	GSensor_I2C_WriteReg(0x00,0x12);//
	GSensor_I2C_WriteReg(0x00,0x02);//
	GSensor_I2C_WriteReg(0x00,0x82);//
	GSensor_I2C_WriteReg(0x00,0x02);//
	GSensor_I2C_WriteReg(0x1F,0x28);//
	GSensor_I2C_WriteReg(0x0C,0x8F);//
	GSensor_I2C_WriteReg(0x00,0x06);//

	//interrupt setup
	GSensor_I2C_WriteReg(0x11,0x06);//IIC 0X07 for no pullup //0x06 High active  0x40 low active
	//Gsensor_WriteReg(0x11,0x06);//IIC 0X06 for no pullup
	//set Gsensor Level
	//0x08-->0.5G
	//0X10-->1G

	GSensor_I2C_WriteReg(0x38,0x00);//
	GSensor_I2C_WriteReg(0x39,0X00);//
	GSensor_I2C_WriteReg(0x0E,0x00);//To enable interrupt.
	GSensor_GMA301_ReadReg_2Byte(0x1C,value);

}

BOOL GSensor_GMA301_open(void)
{

   // open Gsensor interface
   GSensor_GMA301_OpenInterface();

   gbCrashMode=GSensor_Read_Interrupte();

   // Gsensor init
   GSensor_GMA301_Init();

   return TRUE;
}

BOOL GSensor_GMA301_close(void)
{

    if (g_bGsensorOpened==FALSE)
    {
        debug_msg("I2C close already\r\n");
        return TRUE;
    }

    // close Gsensor interface
    GSensor_GMA301_CloseInterface();

    g_bGsensorOpened = FALSE;

    return TRUE;
}


BOOL GSensor_GMA301_GetStatus(Gsensor_Data *GS_Data)
{
	UINT8  abs_cal_x=0,abs_cal_y=0,abs_cal_z=0;
	UINT8 G_INT_COUNT=0;
	UINT8 Threshold = 0xFF;
       UINT32 g_X_Data_L=0,g_X_Data_H=0,g_Y_Data_L=0,g_Y_Data_H=0,g_Z_Data_L=0,g_Z_Data_H=0;

	Threshold = GSensor_GMA301_GetSensitivityLevel();

	GSensor_I2C_ReadReg(0x12);
	GSensor_I2C_ReadReg(0x13);

	g_X_Data_L=GSensor_I2C_ReadReg(OUT_X_L_REG);
	g_Y_Data_L=GSensor_I2C_ReadReg(OUT_Y_L_REG);
	g_Z_Data_L=GSensor_I2C_ReadReg(OUT_Z_L_REG);

	g_X_Data_H=GSensor_I2C_ReadReg(OUT_X_H_REG);
	g_Y_Data_H=GSensor_I2C_ReadReg(OUT_Y_H_REG);
	g_Z_Data_H=GSensor_I2C_ReadReg(OUT_Z_H_REG);


      if((g_X_Data_H&0x03)>=0x02)
		GsXData =-(1023-((g_X_Data_L +((g_X_Data_H&0x03)<<8))));
	else
		GsXData =(g_X_Data_L +((g_X_Data_H&0x03)<<8));

      if((g_Y_Data_H&0x3)>=0x02)
		GsYData =-(1023-(g_Y_Data_L +((g_Y_Data_H&0x03)<<8)));
	else
		GsYData =(g_Y_Data_L +((g_Y_Data_H&0x03)<<8));

      if((g_Z_Data_H&0x3)>=0x02)
		GsZData =-(1023-(g_Z_Data_L +((g_Z_Data_H&0x03)<<8)));
	else
		GsZData =(g_Z_Data_L +((g_Z_Data_H&0x03)<<8));

/*
	debug_msg("@@reg 0x12:%x\r\n",GSensor_I2C_ReadReg(0x12));
	debug_msg("@@reg 0x13:%x\r\n",GSensor_I2C_ReadReg(0x13));
	debug_msg("@@reg 0x14:%x %d\r\n",GSensor_I2C_ReadReg(0x14),g_X_Data_L);//X-L
	debug_msg("@@reg 0x15:%x %d\r\n",GSensor_I2C_ReadReg(0x15),g_X_Data_H);//X-H
	debug_msg("@@reg 0x16:%x %d\r\n",GSensor_I2C_ReadReg(0x16),g_Y_Data_L);//Y-L
	debug_msg("@@reg 0x17:%x %d\r\n",GSensor_I2C_ReadReg(0x17),g_Y_Data_H);//Y-H
	debug_msg("@@reg 0x18:%x %d\r\n",GSensor_I2C_ReadReg(0x18),g_Z_Data_L);//Z-H
	debug_msg("@@reg 0x19:%x %d\r\n",GSensor_I2C_ReadReg(0x19),g_Z_Data_H);//Z-L
*/
	abs_cal_x =abs(GsXData-OldGsXData);
	if(abs_cal_x>Threshold)   //30
	    G_INT_COUNT++;

	abs_cal_y =abs(GsYData-OldGsYData);
	if(abs_cal_y>Threshold) //20
	    G_INT_COUNT++;

	abs_cal_z =abs(GsZData-OldGsZData);
	if(abs_cal_z>Threshold) //20
	    G_INT_COUNT++;

	OldGsXData = GsXData;
	OldGsYData = GsYData;
	OldGsZData = GsZData;

	//debug_msg("@@reg 0x1c:%x\r\n",GSensor_I2C_ReadReg(0x1c));//X-H
	//debug_msg("@@reg 0x1d:%x\r\n",GSensor_I2C_ReadReg(0x1d));//X-L

/*
	debug_msg("@@reg 0x12:%x\r\n",GSensor_I2C_ReadReg(0x12));//X-H
	debug_msg("@@reg 0x13:%x\r\n",GSensor_I2C_ReadReg(0x13));//X-L
	debug_msg("@@reg 0x14:%x\r\n",GSensor_I2C_ReadReg(0x14));//X-H
	debug_msg("@@reg 0x15:%x\r\n",GSensor_I2C_ReadReg(0x15));//X-L
	debug_msg("@@reg 0x16:%x\r\n",GSensor_I2C_ReadReg(0x16));//Y-H
	debug_msg("@@reg 0x17:%x\r\n",GSensor_I2C_ReadReg(0x17));//Y-L
	debug_msg("@@reg 0x18:%x\r\n",GSensor_I2C_ReadReg(0x18));//Z-H
	debug_msg("@@reg 0x19:%x\r\n",GSensor_I2C_ReadReg(0x19));//Z-L
	*/
     //debug_msg("..........X = %d,Y = %d,Z = %d\r\n",GsXData,GsYData,GsZData);
	//debug_msg("..........DX = %d,DY = %d,DZ = %d th=%d \r\n",abs_cal_x,abs_cal_y,abs_cal_z,Threshold);

    if(G_INT_COUNT>=2)
    {
        debug_msg("----------GMA301_isr1 EMG----------\n\r");
        return TRUE;
    }
    else
       return FALSE;
}

BOOL GSensor_GMA301_ParkingMode(void)
{
		//init
		GSensor_I2C_WriteReg(0x21,0x52);//
		GSensor_I2C_WriteReg(0x00,0x02);//
		GSensor_I2C_WriteReg(0x00,0x12);//
		GSensor_I2C_WriteReg(0x00,0x02);//
		GSensor_I2C_WriteReg(0x00,0x82);//

		GSensor_I2C_WriteReg(0x00,0x02);//
		GSensor_I2C_WriteReg(0x1F,0x28);//
		GSensor_I2C_WriteReg(0x0C,0x8F);//
		GSensor_I2C_WriteReg(0x0D,0x60);//		
		GSensor_I2C_WriteReg(0x00,0x06);//
		Delay_DelayMs(3);

		//interrupt setup
		GSensor_I2C_WriteReg(0x11,0x04);//IIC 0X07 for no pullup //0x06 High active  0x40 low active

		GSensor_I2C_WriteReg(0x38,0xFF);//
		GSensor_I2C_WriteReg(0x39,0x18);//10 1g 20 2g 30 3g 40 4g 50 5g 60 6g  (8 : 0.5g)
		
		GSensor_I2C_WriteReg(0x0F,0x00);//
		GSensor_I2C_WriteReg(0x0E,0x00);//0x1C//0x00 // 0x00:disable interrupt
		GSensor_I2C_WriteReg(0x1F,0x28);//To disable micro motion interrupt.
		//Delay_DelayMs(3);
		//Latched reference data of micro motion. 
		GSensor_I2C_ReadReg(0x12);
		GSensor_I2C_ReadReg(0x13);
		GSensor_I2C_ReadReg(0x14);
		GSensor_I2C_ReadReg(0x15);
		GSensor_I2C_ReadReg(0x16);
		GSensor_I2C_ReadReg(0x17);
		GSensor_I2C_ReadReg(0x18);
		GSensor_I2C_ReadReg(0x19);
		GSensor_I2C_WriteReg(0x1F,0x38);//To enable micro motion interrupt.
		Delay_DelayMs(2);
		GSensor_I2C_ReadReg(0x1C);
		GSensor_I2C_ReadReg(0x1D);		
		
		GSensor_I2C_WriteReg(0x0E,0x1C);//0x1C//0x00 // 0x00:disable interrupt
		
		debug_msg("GMA301 Enter parking mode..\r\n");		
}

#define moving_step 4
#define SENSOR_DATA_SIZE 		3 
INT32	bufferave[3][moving_step];	// 16 buffer
INT32	sum[3];		//  sum of moving_step 
INT32	avg[3];		// moving average result

BOOL GSensor_Read_Interrupte()
{
	int  abs_cal_x=0,abs_cal_y=0,abs_cal_z=0;
	UINT8 G_INT_COUNT=0;
	UINT8 Threshold = 0xFF;
       UINT32 Value,g_X_Data_L=0,g_X_Data_H=0,g_Y_Data_L=0,g_Y_Data_H=0,g_Z_Data_L=0,g_Z_Data_H=0;

	int i, j;
	static int	pointer = 0;				/* last update data */


	Threshold = GSensor_GMA301_GetSensitivityLevel();

	GSensor_I2C_ReadReg(0x12);
	GSensor_I2C_ReadReg(0x13);

	g_X_Data_L=GSensor_I2C_ReadReg(0x14);
	g_Y_Data_L=GSensor_I2C_ReadReg(0x16);
	g_Z_Data_L=GSensor_I2C_ReadReg(0x18);

	g_X_Data_H=GSensor_I2C_ReadReg(0x15);
	g_Y_Data_H=GSensor_I2C_ReadReg(0x17);
	g_Z_Data_H=GSensor_I2C_ReadReg(0x19);


	//debug_msg("X:%d  %d\r\n",g_X_Data_H,g_X_Data_L);
	//debug_msg("Y:%d  %d\r\n",g_Y_Data_H,g_Y_Data_L);
	//debug_msg("Z:%d  %d\r\n",g_Z_Data_H,g_Z_Data_L);

      if((g_X_Data_H&0x03)>=0x02)
		GsXData =-(512 - ((g_X_Data_L +((g_X_Data_H&0x01)<<8))));
	else
		GsXData =(g_X_Data_L +((g_X_Data_H&0x01)<<8));

      if((g_Y_Data_H&0x3)>=0x02)
		GsYData =-(521 - (g_Y_Data_L +((g_Y_Data_H&0x01)<<8))) ;
	else
		GsYData =(g_Y_Data_L +((g_Y_Data_H&0x01)<<8));

      if((g_Z_Data_H&0x3)>=0x02)
		GsZData =-(512 - (g_Z_Data_L +((g_Z_Data_H&0x01)<<8)));
	else
		GsZData =(g_Z_Data_L +((g_Z_Data_H&0x01)<<8));

/* for Simple Moving Average */

	/* init gma->sum */
	for(i = 0; i < SENSOR_DATA_SIZE; ++i)
		sum[i] = 0;

	pointer++;
	pointer %= moving_step;
	
		bufferave[0][pointer] = GsXData;
		bufferave[1][pointer] = GsYData;
		bufferave[2][pointer] = GsZData;

	OldGsXData	= avg[0];
	OldGsYData = avg[1];
	OldGsZData = avg[2];

    for(i = 0; i < SENSOR_DATA_SIZE; ++i)
		for(j = 1; j < moving_step; ++j)
			sum[i] += bufferave[i][j];

	for(i = 0; i < SENSOR_DATA_SIZE; ++i)
		avg[i] = sum[i] / ((moving_step)-1);


	//debug_msg("OLD Average X = %d,Y = %d,Z = %d\r\n",OldGsXData, OldGsYData, OldGsZData);
	//debug_msg("Moving Average X = %d,Y = %d,Z = %d\r\n",avg[0], avg[1], avg[2]);

	abs_cal_x = OldGsXData -avg[0];
	abs_cal_y = OldGsYData -avg[1];
	abs_cal_z = OldGsZData -avg[2];
	
	if(abs_cal_x > Threshold || abs_cal_x < -Threshold)
		G_INT_COUNT++;
	if(abs_cal_y > Threshold || abs_cal_y < -Threshold)
		G_INT_COUNT++;
	if(abs_cal_z > Threshold || abs_cal_z < -Threshold)
		G_INT_COUNT++;

	
	//debug_msg("==== DX = %d,DY = %d,DZ = %d\r\n",abs_cal_x, abs_cal_y, abs_cal_z);
      Value=GSensor_I2C_ReadReg(0x13);
	debug_msg("====interrupt status = %x..\r\n",Value);
	

	if((Value&0x20)==0x20)
	{
		debug_msg("----------GMA301_isr EMG----------\n\r");
		return TRUE;
	}
	else
	{
		debug_msg("----------GMA301_isr--22--EMG----------\n\r");	
		return FALSE;
	}
}

void GSensor_GMA301_SetInterrupt(BOOL En)
{
	if(En==TRUE){
		GSensor_I2C_WriteReg(0x0E,0x1C);//0x1C//0x00 // 0x00:disable interrupt		
		GSensor_I2C_WriteReg(0x1F,0x38);//To enable micro motion interrupt.
	}
       else	
       {
		GSensor_I2C_WriteReg(0x0E,0x00);//0x1C//0x00 // 0x00:disable interrupt       
		GSensor_I2C_WriteReg(0x1F,0x28);//To enable micro motion interrupt.
       }
}

