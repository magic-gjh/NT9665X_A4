#include "Gpio.h"
#include "Pad.h"

///-----------BIT1612M-----------------------
#define LCD_NONE    0XFF
#define LCD220_PANEL4_3   0x0
#define LCD65X_PANEL4_3     0x1
#define LCD65X_PANEL5_0     0x2

#define LCD_TPYE   LCD65X_PANEL4_3//LCD65X_PANEL5_0


#define RGB_SERIAL_MODE_     1
#define CCIR656_MODE_     2

#define DATA_FOMAT    RGB_SERIAL_MODE_

///---------------BIT1612M-------------------


extern unsigned char BIT1612M_CVBS_BANK0[252];
extern unsigned char BIT1612M_BANK1[8];
extern unsigned char BIT1612M_ITU656_BANK0[252];
//extern unsigned char BIT1216M_CVBS_REG[][2];
//extern unsigned char BIT1216M_ITU656_REG[][2];

//typedef bit                 BOOL;       /* Boolean */

typedef unsigned char       UB8;        /* 8-bit data */
typedef unsigned short int  UW16;       /*16-bit data */
typedef unsigned long       UW32;       /*32-bit data */

typedef signed char         SB8;        /* 8-bit data */
typedef signed short int    SW16;       /*16-bit data */
typedef signed long         SW32;       /*32-bit data */

#define VP_MAD 0xc0

#define I2C_SM          0
#define I2C_FM          1
#define I2C_FMPLUS      2
#define I2C_CFG         I2C_FM


/* ::::::::::::::::::::::::::::::::::::
    [3]37
    I2C Standard Mode   (Sm, 100 kbit/s)
    I2C Fast-mode       (Fm, 400 kbit/s)
    I2C Fast-mode Plus  (Fm+, 1 Mbit/s)
   :::::::::::::::::::::::::::::::::::: */
/*
¡E  Standard-mode (Sm), with a bool rate up to 100 kbit/s
¡E  Fast-mode (Fm), with a bool rate up to 400 kbit/s
¡E  Fast-mode Plus (Fm+), with a bool rate up to 1 Mbit/s
¡E  High-speed mode (Hs-mode), with a bool rate up to 3.4 Mbit/s.

                             Sm             Fm          Fm+
Symbol  Parameter Conditions Standard-mode  Fast-mode   Fast-mode Plus  Unit
                               Min  Max     Min Max     Min Max
fSCL    SCL clock frequency     0   100     0   400     0   1000        kHz


tHD;STA hold time (repeated)    4.0 -       0.6 -       0.26 -          us
        START condition
        After this period, the
        first clock pulse is
        generated.

tLOW    LOW period of the SCL   4.7 -       1.3 -       0.5  -          us
        clock

tHIGH   HIGH period of the SCL  4.0 -       0.6 -       0.26 -          us
        clock

tSU;STA set-up time for a       4.7 -       0.6 -       0.26 -          us
        repeatedSTART condition

tHD;DAT I2C-bus devices         0[3] -[4]   0[3] -[4]   0    -          us

        data hold time[2]       5.0 -       -   -       -    -          us
        CBUS compatible masters
        (see Remark in Section 4.1)

tSU;DAT data set-up time        250 -       100[5] -    50   -          ns

tr      rise time of both SDA   -   1000    20+ 300     -    120        ns
        and SCL signals

tf      fall time of both SDA   -   300     20+ 300     -    120        ns
        and SCL signals

tSU;STO set-up time for STOP    4.0 -       0.6 -       0.26 -          us
        condition

tBUF    bus free time between   4.7 -       1.3 -       0.5  -          us
        a STOP and START
        condition

tVD;DAT data valid time[10]     -   3.45[4] -   0.9[4]  -    0.45[4]    us

tVD;ACK data valid acknowledge  -   3.45[4] -   0.9[4]  -    0.45[4]    us
        time[11]
*/


// [1]117,125 Internal Oscillator = 12.0 +/- 0.18 MHz = 12.0 +/- 1.5% MHz
// SYSCLK = 1 / 12.000MHz = 83.33 ns = 0.08333 us
//#define SYSCLK_PERIOD           83

// SYSCLK = 1 / 24.000MHz = 41.66 ns = 0.04166 us
#define SYSCLK_PERIOD           20//10//7///10



#if (I2C_CFG == I2C_SM)

    #define I2C_SU_STA          (4700 / SYSCLK_PERIOD)
    #define I2C_HD_STA          (4000 / SYSCLK_PERIOD)
    #define I2C_SU_DAT          ( 250 / SYSCLK_PERIOD)
    #define I2C_HD_DAT          (   0 / SYSCLK_PERIOD)
    #define I2C_HIGH            (4000 / SYSCLK_PERIOD)
    #define I2C_LOW             (4700 / SYSCLK_PERIOD)
    #define I2C_SU_STO          (4000 / SYSCLK_PERIOD)
    #define I2C_BUF             (4700 / SYSCLK_PERIOD)
    #define I2C_VD_DAT          (3450 / SYSCLK_PERIOD)

#elif (I2C_CFG == I2C_FM)

    #define I2C_SU_STA          ( 600 / SYSCLK_PERIOD)
    #define I2C_HD_STA          ( 600 / SYSCLK_PERIOD)
    #define I2C_SU_DAT          ( 100 / SYSCLK_PERIOD)
    #define I2C_HD_DAT          (   0 / SYSCLK_PERIOD)////no use
    #define I2C_HIGH            ( 600 / SYSCLK_PERIOD)
    #define I2C_LOW             (1300 / SYSCLK_PERIOD)
    #define I2C_SU_STO          ( 600 / SYSCLK_PERIOD)////I2C_Stop
    #define I2C_BUF             (1300 / SYSCLK_PERIOD)////no use
    #define I2C_VD_DAT          ( 900 / SYSCLK_PERIOD)////no use

#elif (I2C_CFG == I2C_FMPLUS)

    #define I2C_SU_STA          ( 260 / SYSCLK_PERIOD)
    #define I2C_HD_STA          ( 260 / SYSCLK_PERIOD)
    #define I2C_SU_DAT          (  50 / SYSCLK_PERIOD)
    #define I2C_HD_DAT          (   0 / SYSCLK_PERIOD)
    #define I2C_HIGH              ( 260 / SYSCLK_PERIOD)
    #define I2C_LOW               ( 500 / SYSCLK_PERIOD)
    #define I2C_SU_STO          ( 260 / SYSCLK_PERIOD)
    #define I2C_BUF                ( 500 / SYSCLK_PERIOD)
    #define I2C_VD_DAT          ( 450 / SYSCLK_PERIOD)

#else
    #error I2C.H - invalid I2C_CFG !
#endif


#ifdef NOT_JUNK
/* ::::::::::::::::::::::::::::::::::::
    I2C Mode Configuration
   :::::::::::::::::::::::::::::::::::: */
#define I2C_MODE_SLOW               1           // < 100 kbit/s
#define I2C_MODE_STANDARD           2           // 100 kbit/s       108K Hz for CLK HIGH interval
#define I2C_MODE_FAST               3           // 400 kbit/s       400K Hz for CLK HIGH interval
#define I2C_MODE_HIGHSPEED          4           // 3.4 Mbit/s       1.19M Hz for CLK HIGH interval

#define I2C_MODE_CFG                I2C_MODE_STANDARD
//#define I2C_MODE_CFG                I2C_MODE_SLOW
#endif


/* ::::::::::::::::::::::::::::::::::::
    I2C Configuration
   :::::::::::::::::::::::::::::::::::: */
#define I2C_RX_BURST        ON
#define I2C_RX_BYTE         ON
#define I2C_RX_WORD         OFF

#define I2C_SET_ACK         ON

#define I2C_TX_BURST        ON
#define I2C_TX_BYTE         ON
#define I2C_TX_REPEAT       ON
#define I2C_TX_WORD         OFF

#define HIGH 1
#define LOW 1
/* ::::::::::::::::::::::::::::::::::::
    BITEK Configuration
   :::::::::::::::::::::::::::::::::::: */
#define BITEK_RxByte(wREG)                  \
    I2C_RxByte((VP_MAD) | (((wREG) >> 7) & 0x1E), (wREG))


#define BITEK_TxBurst(wREG, wCNT, pbDATA)   \
    I2C_TxBurst((VP_MAD) | (((wREG) >> 7) & 0x1E), (wREG), (wCNT), (pbDATA))


#define BITEK_TxByte(wREG, bDATA)           \
    I2C_TxByte((VP_MAD) | (((wREG) >> 7) & 0x1E), (wREG), (bDATA))


#define BITEK_TxRepeat(wREG, wCNT, bDATA)   \
    I2C_TxRepeat((VP_MAD) | (((wREG) >> 7) & 0x1E), (wREG), (wCNT), (bDATA))


/* ------------------------------------
    Type Definitions
   ------------------------------------ */

/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
void I2C_Delay(UW16 Time);

extern BOOL I2C_GetACK(void);
extern void BIT_I2C_Init(void);


#if (I2C_RX_BURST)
extern void I2C_RxBurst(UB8 bSLA, UB8 bREG, UB8 bCNT, UB8 *pbDATA);
#endif

#if (I2C_RX_BYTE)
extern UB8  I2C_RxByte(UB8 bSLA, UB8 bREG);
#endif

extern UB8  I2C_RxData(void);

#if (I2C_RX_WORD)
extern UW16 I2C_RxWord(UB8 bSLA, UB8 bREG);
#endif


#if (I2C_SET_ACK)
extern void I2C_SetACK(void);
#endif

extern void I2C_SetNAK(void);

extern void I2C_Start(void);
extern void I2C_Stop(void);


#if (I2C_TX_BURST)
extern void I2C_TxBurst(UB8 bSLA, UB8 bREG, UB8 bCNT, UB8 *pbDATA);
#endif

#if (I2C_TX_BYTE)
extern void I2C_TxByte(UB8 bSLA, UB8 bREG, UB8 bDATA);
#endif

extern void I2C_TxData(UB8 bData);


#if (I2C_TX_REPEAT)
extern void I2C_TxRepeat(UB8 bSLA, UW16 wREG, UB8 bCNT, UB8 bDATA);
#endif


#if (I2C_TX_WORD)
extern void I2C_TxWord(UB8 bSLA, UB8 bREG, UW16 wDATA);
#endif

#define GPIO_LCD_CS   L_GPIO_20
#define GPIO_LCD_SCK      L_GPIO_21//D_GPIO_1//P_GPIO_0
#define GPIO_LCD_SDA     L_GPIO_22 //D_GPIO_0//P_GPIO_1

//I2C control pin
#define I2C_SCL_OUT				gpio_setDir(GPIO_LCD_SCK, GPIO_DIR_OUTPUT)
#define I2C_SCL_IN				gpio_setDir(GPIO_LCD_SCK, GPIO_DIR_INPUT)
#define I2C_SCL_HIGH			gpio_setPin(GPIO_LCD_SCK)
#define I2C_SCL_LOW				gpio_clearPin(GPIO_LCD_SCK)
#define I2C_SCL_DETECT			gpio_getPin(GPIO_LCD_SCK)

#define I2C_SDA_OUT				gpio_setDir(GPIO_LCD_SDA, GPIO_DIR_OUTPUT)
#define I2C_SDA_IN				gpio_setDir(GPIO_LCD_SDA, GPIO_DIR_INPUT)
#define I2C_SDA_HIGH 			gpio_setPin(GPIO_LCD_SDA)
#define I2C_SDA_LOW  			gpio_clearPin(GPIO_LCD_SDA)
#define I2C_SDA_DETECT			gpio_getPin(GPIO_LCD_SDA)


typedef struct _BIT1216M_PIN_DEF
{
    UINT32  uigpio_sen;                 ///< GPIO pin number for SEN, valid when uimode == ENDE_CTRL_MODE_GPIO
    UINT32  uigpio_clk;                 ///< GPIO pin number for Clock, valid when uimode == ENDE_CTRL_MODE_GPIO
    UINT32  uigpio_data;                ///< GPIO pin number for Data, valid when uimode == ENDE_CTRL_MODE_GPIO
} BIT1216M_PIN_DEF, *PBIT1216M_PIN_DEF;


typedef struct _BIT1216M_DEVICE_OBJ
{
    BIT1216M_PIN_DEF  BIT1216MCtrl_Pin;///
    void          (*fpPostAdjusement)(void);///
} BIT1216M_DEVICE_OBJ, *PBIT1216M_DEVICE_OBJ;
/*
typedef struct
{
    BOOL        (*GsensorInit)(void);
    void        (*GetGsensorData)(Gsensor_Data *GS_Data);
    void        (*GetGsensorStatus)(UINT32 status,UINT32 *uiData);
    void        (*GsensorOpenInterface)(void);
    void        (*GsensorCloseInterface)(void);
} GSENSOR_FUNC, *PGSENSOR_FUNC;
*/
extern BIT1216M_DEVICE_OBJ BIT1216M_DeviceObj;

#define BIT1216M_I2C_SCL_OUT				gpio_setDir(GPIO_LCD_SCK, GPIO_DIR_OUTPUT)//gpio_setDir(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_clk, GPIO_DIR_OUTPUT)
#define BIT1216M_I2C_SCL_IN				gpio_setDir(GPIO_LCD_SCK, GPIO_DIR_INPUT)//gpio_setDir(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_clk, GPIO_DIR_INPUT)
#define BIT1216M_I2C_SCL_HIGH			       gpio_setPin(GPIO_LCD_SCK)//gpio_setPin(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_clk)
#define BIT1216M_I2C_SCL_LOW				gpio_clearPin(GPIO_LCD_SCK)//gpio_clearPin(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_clk)
#define BIT1216M_I2C_SCL_DETECT			gpio_getPin(GPIO_LCD_SCK)//gpio_getPin(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_clk)

#define BIT1216M_I2C_SDA_OUT				gpio_setDir(GPIO_LCD_SDA, GPIO_DIR_OUTPUT)//gpio_setDir(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_data, GPIO_DIR_OUTPUT)
#define BIT1216M_I2C_SDA_IN				gpio_setDir(GPIO_LCD_SDA, GPIO_DIR_INPUT)//gpio_setDir(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_data, GPIO_DIR_INPUT)
#define BIT1216M_I2C_SDA_HIGH 			       gpio_setPin(GPIO_LCD_SDA)//gpio_setPin(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_data)
#define BIT1216M_I2C_SDA_LOW  			       gpio_clearPin(GPIO_LCD_SDA)//gpio_clearPin(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_data)
#define BIT1216M_I2C_SDA_DETECT			gpio_getPin(GPIO_LCD_SDA)//gpio_getPin(GSensor_DeviceObj.GSensorCtrl_Pin.uigpio_data)

#define GPIO_LCD_DATA0      L_GPIO_0
#define GPIO_LCD_DATA1      L_GPIO_1//D_GPIO_1//P_GPIO_0
#define GPIO_LCD_DATA2      L_GPIO_2 //D_GPIO_0//P_GPIO_1
#define GPIO_LCD_DATA3      L_GPIO_3//D_GPIO_1//P_GPIO_0
#define GPIO_LCD_DATA4      L_GPIO_4 //D_GPIO_0//P_GPIO_1
#define GPIO_LCD_DATA5      L_GPIO_5//D_GPIO_1//P_GPIO_0
#define GPIO_LCD_DATA6      L_GPIO_6 //D_GPIO_0//P_GPIO_1
#define GPIO_LCD_DATA7      L_GPIO_7//D_GPIO_1//P_GPIO_0
#define GPIO_LCD_CLK      L_GPIO_8 //D_GPIO_0//P_GPIO_1

#define GPIO_LCD_VD      L_GPIO_9//D_GPIO_1//P_GPIO_0
#define GPIO_LCD_HD      L_GPIO_10 //D_GPIO_0//P_GPIO_1

typedef enum _BIT1216M_MODE
{
    BIT1216M_CVBS_BANK0,
    BIT1216M_CVBS_BANK1,
    BIT1216M_ITU656_BANK0,
    BIT1216M_ITU656_BANK1,
    ENUM_DUMMY4WORD(BIT1216M_MODE)
}BIT1216M_MODE;

typedef struct _BIT1216M_CMD
{
    UINT32 uiAddr;       ///< address
    UINT32 uiData;       ///< data
} BIT1216M_CMD,*PBIT1216M_CMD;

extern unsigned char BIT1612M_ITU656_CVBS1[252];
extern unsigned char BIT1216M_ITU656_A[252];
extern unsigned char BIT1612M_BANKA[8];
extern unsigned char BIT1612M_BANK2[8];
extern void BIT1216M_CVBS_BANK0_OUT(void);
extern void BIT1216M_CVBS_BANK1_OUT(void);
extern void BIT1216M_ITU656_BANK0_OUT(void);
extern void BIT1216M_ITU656_BANK1_OUT(void);
	

extern void InitBIT1612M(void);

extern void BIT1612M_PullUpDown_None(void);

extern void BIT1612M_PullUpDown_Reset(void);


