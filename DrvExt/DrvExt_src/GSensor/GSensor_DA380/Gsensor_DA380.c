#include "GSensor.h"
#include "GSensor_Int.h"
#include "Gsensor_DA380.h"
#include "SwTimer.h"
#include "Debug.h"
#include <stdlib.h>
#include "ErrorNo.h"
#include "Delay.h"

//********************************************************************************
//
//
//********************************************************************************
static INT32 GsXData = 0,OldGsXData = 0;
static INT32 GsYData = 0,OldGsYData = 0;
static INT32 GsZData = 0,OldGsZData = 0;
static BOOL  g_bGsensorOpened = FALSE;
static GSENSOR_SENSITIVITY g_GSensorSensitivity = GSENSOR_SENSITIVITY_OFF;
static BOOL gbCrashMode=FALSE;

static GSENSOR_OBJ g_GsensorDA380Obj = {GSensor_DA380_open, GSensor_DA380_close, GSensor_DA380_GetStatus, 
	GSensor_DA380_ParkingMode, GSensor_DA380_CrashMode, GSensor_DA380_SetSensitivity,
	GSensor_DA380_ClearCrashMode,GSensor_DA380_SetInterrupt};


/*return value: 0: is ok    other: is failed*/
int     i2c_read_byte_data( unsigned char addr, unsigned char *data){
	*data =  (unsigned char)GSensor_I2C_ReadReg(addr);
	return 0;
}

/*return value: 0: is ok    other: is failed*/
int     i2c_write_byte_data( unsigned char addr, unsigned char data){
	GSensor_I2C_WriteReg(addr,data);
	return 0;
}

/*return value: count: is ok    other: is failed*/
int     i2c_read_block_data( unsigned char base_addr, unsigned char count, unsigned char *data){
	int i = 0;
		
	for(i = 0; i < count;i++)
	{
		*data =  (unsigned char)GSensor_I2C_ReadReg(base_addr+i);
               data ++;
	}	
		
	return count;
}

int mir3da_register_read( unsigned char addr, unsigned char *data){
    int     res = 0;

    res = i2c_read_byte_data(addr, data);
    if(res != 0) {
          return res;
    }	

    return res;
}

int mir3da_register_write( unsigned char addr, unsigned char data){
    int     res = 0;

    res = i2c_write_byte_data(addr, data);
    if(res != 0) {
         return res;
    }

    return res;
}

int mir3da_register_read_continuously( unsigned char addr, unsigned char count, unsigned char *data)
{
    int     res = 0;

    res = (count==i2c_read_block_data(addr, count, data)) ? 0 : 1;
    if(res != 0) {
         return res;
    }

    return res;
}

int mir3da_register_mask_write(unsigned char addr, unsigned char mask, unsigned char data){
    int     res = 0;
    unsigned char      tmp_data;

    res = mir3da_register_read(addr, &tmp_data);
    if(res) {
        return res;
    }

    tmp_data &= ~mask; 
    tmp_data |= data & mask;
    res = mir3da_register_write(addr, tmp_data);

    return res;
}

int mir3da_i2c_init(void){
	 GSENSOR_INFO GsensorInfo;  
	 GsensorInfo.I2C_RegBytes = GSENSOR_I2C_REGISTER_1BYTE; 
	 GsensorInfo.I2C_PinMux = I2C_PINMUX_1ST;  
	 GsensorInfo.I2C_BusClock = I2C_BUS_CLOCK_400KHZ;    
	 GsensorInfo.I2C_Slave_WAddr = 0x4e;   
	 GsensorInfo.I2C_Slave_RAddr = 0x4f;   

	 if (GSensor_I2C_Init(GsensorInfo) == TRUE)    {     
	 	debug_msg("------mir3da i2c init OK-----\r\n"); 
		return 0;
	 }

	 return -1;
}

/*return value: 0: is ok    other: is failed*/
int mir3da_chip_init(void){
	int             res = 0;
	unsigned char data=0;

/*
	if(mir3da_i2c_init()){
		return -1;
		debug_msg("------mir3da i2c init error-----\r\n"); 
	}
*/
	mir3da_register_read(NSA_REG_WHO_AM_I,&data);
	if(data != 0x13){
	       debug_msg("------mir3da read chip id  error= %x-----\r\n",data); 
		return -1;
	}

	debug_msg("------mir3da chip id = %x-----\r\n",data); 

	res =  mir3da_register_mask_write(NSA_REG_SPI_I2C, 0x24, 0x24);

	Delay_DelayMs(5);

	res |= mir3da_register_mask_write(NSA_REG_G_RANGE, 0x03, 0x02);
	res |= mir3da_register_mask_write(NSA_REG_POWERMODE_BW, 0xFF, 0x1E);
	res |= mir3da_register_mask_write(NSA_REG_ODR_AXIS_DISABLE, 0xFF, 0x07);
	
	res |= mir3da_register_mask_write(NSA_REG_INT_LATCH, 0x8F, 0x8F);
	
	res |= mir3da_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x83);
	res |= mir3da_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x69);
	res |= mir3da_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0xBD);

	return res;	    	
}


int mir3da_open_interrupt(int num){
	int   res = 0;

	res = mir3da_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x03);
	res = mir3da_register_write(NSA_REG_ACTIVE_DURATION,0x01 );// 0x03
	res = mir3da_register_write(NSA_REG_ACTIVE_THRESHOLD,0x26 );////DEBUG//38//DF
			
	switch(num){

		case 0:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING1,0x04 );
			break;

		case 1:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING3,0x04 );
			break;
	}

	return res;
}

int mir3da_close_interrupt(int num){
	int   res = 0;

	res = mir3da_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x00 );
			
	switch(num){

		case 0:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING1,0x00 );
			break;

		case 1:
			res = mir3da_register_write(NSA_REG_INTERRUPT_MAPPING3,0x00 );
			break;
	}

	return res;
}

/*return value: 1:has intterupt     0: no intterupt*/
int mir3da_read_interrupt_statu(){

	unsigned char data=0;

	mir3da_register_read(NSA_REG_MOTION_FLAG,&data);
       debug_msg("-->DA380 get data:%x\r\n",data);
	if(data==0x04)
		return 1;

	return 0;
}

/*return value: 0: is ok    other: is failed*/
int mir3da_read_data(short *x, short *y, short *z)
{
    unsigned char    tmp_data[6] = {0};

    if (mir3da_register_read_continuously(NSA_REG_ACC_X_LSB, 6, tmp_data) != 0) {
        return -1;
    }
    
    *x = ((short)(tmp_data[1] << 8 | tmp_data[0]))>> 4;
    *y = ((short)(tmp_data[3] << 8 | tmp_data[2]))>> 4;
    *z = ((short)(tmp_data[5] << 8 | tmp_data[4]))>> 4;

     //debug_msg("------mir3da x= %4d y= %4d z= %4d-----\r\n",*x,*y,*z); 	

    return 0;
}

int rotate(char *rot,char *upd,short x,short y ,short z)
{
    static char old_rot = 0;

   *rot = old_rot;

    if(y > 200&&abs(z) <120&&abs(x)< 120)
    {
        *rot = 0;
    }
    if(x > 200&&abs(z) <120&&abs(y)< 120)
    {
        *rot = 1;
    }
    if(-y > 200&&abs(z) <120&&abs(x)< 120)
    {
        *rot = 2;
    }
    if(-x > 200&&abs(z) <120&&abs(y)< 120)
    {
        *rot = 3;
    }
    if(old_rot != *rot)
    {
        old_rot = *rot;
        *upd = 1;
    }
    else
    {
        *upd = 0;
    }
}

BOOL GSensor_DA380_CrashMode(void)
{
    return gbCrashMode;
}

void GSensor_DA380_ClearCrashMode(void)
{
    gbCrashMode=FALSE;
}

void GSensor_DA380_SetSensitivity(GSENSOR_SENSITIVITY GSensorSensitivity)
{
    g_GSensorSensitivity = GSensorSensitivity;
}

//**********************************************************************************
//
//
//**********************************************************************************
static GSENSOR_INFO2 g_GsensorInfo;
static UINT32 DA380Threshold=0xFF;/*1G= X/16;*/

static BOOL GSensor_Read_Interrupte();

static UINT32 GSensor_DA380_ReadReg_2Byte(UINT32 uiAddr, UINT32 uidata)
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

INT32 GSensor_DA380_GetAxisMsg(UINT32 uiInt)
{
    INT32 Data  = 0;

    if (uiInt >511) //0x3F
    {
        //negative number
        Data =-(uiInt-511);
        debug_msg("GSensor_DA380_GetAxisMsg1  = %d\r\n",Data);

    }
    else
    {
        Data = uiInt;
        debug_msg("GSensor_DA380_GetAxisMsg2  = %d\r\n",Data);
    }

    return Data;//(bzNegative)?(-1*Data):(Data);

}

static UINT32 GSensor_DA380_GetSensitivityLevel(void)
{
    switch(g_GSensorSensitivity)
   	{
		case GSENSOR_SENSITIVITY_OFF:
			DA380Threshold =32768;
			break;
		case GSENSOR_SENSITIVITY_LOW:
			DA380Threshold =1400;//25
			break;
		case GSENSOR_SENSITIVITY_MED:
			DA380Threshold =800;//20
			break;
		case GSENSOR_SENSITIVITY_HIGH:
			DA380Threshold =200;//16
			break;
		default:
			DA380Threshold =0xFF;
			break;
   	}
	return DA380Threshold;
}

static void GSensor_DA380_OpenInterface(void)
{

}

static void GSensor_DA380_CloseInterface(void)
{

}

PGSENSOR_OBJ GSensor_DA380_getGSensorObj(void)
{
    return &g_GsensorDA380Obj;
}

static BOOL GSensor_DA380_Init(void)
{
    mir3da_chip_init();
    return TRUE;
}

//for  Gensor POWER UP
void GSensor_DA380_PowerUp(INT32 sen)
{}

void GSensor_DA380_PowerDown(void)
{}

BOOL GSensor_DA380_open(void)
{

   // open Gsensor interface
   GSensor_DA380_OpenInterface();

   gbCrashMode=GSensor_Read_Interrupte();
    debug_msg("GSensor_DA380_open:%d..\r\n",gbCrashMode);

   // Gsensor init
   GSensor_DA380_Init();

   return TRUE;
}

BOOL GSensor_DA380_close(void)
{

    if (g_bGsensorOpened==FALSE)
    {
        debug_msg("I2C close already\r\n");
        return TRUE;
    }

    // close Gsensor interface
    GSensor_DA380_CloseInterface();

    g_bGsensorOpened = FALSE;

    return TRUE;
}


BOOL GSensor_DA380_GetStatus(Gsensor_Data *GS_Data)
{
    short x = 0,y = 0,z = 0;
    static short prev_x= 0,prev_y= 0,prev_z= 0;
    char upd = 0, rot = 0, prev_rot = 0x0F;	
    static UINT32 updCount = 0;	
    INT32 xyz;
    int ret=FALSE;	
	
       ret = mir3da_read_data(&x,&y,&z);    
       rotate(&rot,&upd,x,y,z);
	xyz = GSensor_DA380_GetSensitivityLevel();
        if ((abs(x-prev_x )> xyz)||
            (abs(y-prev_y )> xyz)||
            (abs(z-prev_z )> xyz))
        {
            debug_err((">>>> DA380 gsensor-move-----\r\n"));
            	ret=TRUE;//return TRUE;				
        }	       
        prev_x = x;
        prev_y = y;
        prev_z = z;	
	  return ret;//FALSE;
	  
}

BOOL GSensor_DA380_ParkingMode(void)
{
      mir3da_open_interrupt(0);//add auto start
}

BOOL GSensor_Read_Interrupte()
{
	if(mir3da_read_interrupt_statu())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void GSensor_DA380_SetInterrupt(BOOL En)
{
       debug_err((">>>>GSensor_DA380_SetInterrupt:%d-----\r\n",En));
	if(En==TRUE)
		mir3da_open_interrupt(0);		
	else
		mir3da_close_interrupt(0);				
}

