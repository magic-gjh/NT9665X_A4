/*
    Display object for driving DSI device

    @file       HX8379C_dsi.c
    @ingroup
    @note       This panel MUST select IDE clock to PLL1 ( 480 ). Once change to \n
                another frequence, the _IDE_fDCLK should be re calculated

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "debug.h"
#include "display.h"
#include "dispdev_ifDsi.h"
#include "dispdev_panlcomn.h"

// Default debug level
#ifndef __DBGLVL__
    #define __DBGLVL__  2       // Output all message by default. __DBGLVL__ will be set to 1 via make parameter when release code.
#endif

// Default debug filter
#ifndef __DBGFLT__
    #define __DBGFLT__  "*"     // Display everything when debug level is 2
#endif

#include "DebugModule.h"

#define PANEL_WIDTH     480//480
#define PANEL_HEIGHT    800//864//848


#define HX8379C_IND_MSG(...)       debug_msg(__VA_ARGS__)

#define HX8379C_ERR_MSG(...)       debug_msg(__VA_ARGS__)

#define HX8379C_WRN_MSG(...)       debug_msg(__VA_ARGS__)

#define HX8379C_TE_OFF              0
#define HX8379C_TE_ON               1

#define HX8379C_TE_PACKET           0
#define HX8379C_TE_PIN              1

/*
    RGB888 = 1 pixel = 3bytes packet
    If DSI src = 240MHz, internal clock = 30MHz, data rate = 30MHz x 1bytes = 30MB / sec per lane
    2 lane = 60MB = 20Mpixel ' IDE need > 20MHz
*/


#define DSI_FORMAT_RGB565          0    //N/A in HX8379C
#define DSI_FORMAT_RGB666P         1    //N/A in HX8379C
#define DSI_FORMAT_RGB666L         2    //N/A in HX8379C
#define DSI_FORMAT_RGB888          3    //IDE use 480 & DSI use 480

#define DSI_PACKET_FORMAT          DSI_FORMAT_RGB888



#define DSI_TARGET_CLK            480


#if(DSI_PACKET_FORMAT == DSI_FORMAT_RGB888)
#define _IDE_fDCLK      ((((DSI_TARGET_CLK / 8) * 2)/3))
#else
#error "Format not RGB888"
#endif

#define HVALIDST    0x39  //uiHSyncBackPorch(HPB) -> 53
#define VVALIDST    0x06//0x15  //uiVSyncBackPorchOdd/Even

#define HSYNCT      0x02
#define VSYNCT      0x02


/*
    Panel Parameters for TCON HX8379C
*/
//@{
/*Used in DSI*/
const tPANEL_CMD tCmdModeDSI[] =
#if 0
{
    {DSICMD_CMD,    0xFF}, 
    {DSICMD_DATA,   0xFF},
    {DSICMD_DATA,   0x98},
    {DSICMD_DATA,   0x06}, 
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x01},     // Change to Page 1

    {DSICMD_CMD,    0x08},
    {DSICMD_DATA,   0x10},   // output SDA

    {DSICMD_CMD,    0x21},
    {DSICMD_DATA,   0x01},   // DE = 1 Active

    {DSICMD_CMD,   0x30},
    {DSICMD_DATA,   0x00},   // 480 X 800

    {DSICMD_CMD,    0x31},
    {DSICMD_DATA,   0x02},   // 2dot Inversion

    {DSICMD_CMD,    0x60},
    {DSICMD_DATA,   0x07},   // SDTI

    {DSICMD_CMD,    0x61},
    {DSICMD_DATA,   0x06},   // CRTI

    {DSICMD_CMD,    0x62},
    {DSICMD_DATA,   0x06},   // EQTI

    {DSICMD_CMD,    0x63},
    {DSICMD_DATA,   0x04},   // PCTI

    {DSICMD_CMD,    0x40},
    {DSICMD_DATA,   0x15},   // BT  +2.5/-2.5 pump for DDVDH-L

    {DSICMD_CMD,    0x41},
    {DSICMD_DATA,   0x55},   //22 DVDDH DVDDL clamp  

    {DSICMD_CMD,    0x42},
    {DSICMD_DATA,   0x02},   // VGH/VGL 31.37ma 32.7ma  0.03

    {DSICMD_CMD,    0x43},
    {DSICMD_DATA,   0x0B},   // VGH/VGL 13 -9  5 -3.8 1.6 -1.5 -2.75

    {DSICMD_CMD,    0x44},
    {DSICMD_DATA,   0x0C},   // VGH/VGL 

    {DSICMD_CMD,    0x45},
    {DSICMD_DATA,   0x1B},   // VGL_REG  -10V 
    
   // {DSICMD_CMD,    0x46},
   // {DSICMD_DATA,   0x44},  // DDVDL_PK1 2 

  //  {DSICMD_CMD,    0x47},
 //   {DSICMD_DATA,   0x44},  // DDVDL_PK1 2 
    
    {DSICMD_CMD,    0x50},
    {DSICMD_DATA,   0x80},   // VGMP

    {DSICMD_CMD,    0x51},
    {DSICMD_DATA,   0x80},   // VGMN 

    {DSICMD_CMD,    0x52},
    {DSICMD_DATA,   0x00},   //Flicker

    {DSICMD_CMD,    0x53},
    {DSICMD_DATA,   0x5c},   //Flicker 5e  62

///==============Positive Gamma==============///
    {DSICMD_CMD,    0xA0},
    {DSICMD_DATA,   0x00},  // Gamma 0 /255

    {DSICMD_CMD,    0xA1},
    {DSICMD_DATA,   0x0B},  // Gamma 4 /251

    {DSICMD_CMD,    0xA2},
    {DSICMD_DATA,   0x0F},  // Gamma 8 /247

    {DSICMD_CMD,    0xA3},
    {DSICMD_DATA,   0x10},  // Gamma 16	/239

    {DSICMD_CMD,    0xA4},
    {DSICMD_DATA,   0x08},  // Gamma 24 /231

    {DSICMD_CMD,    0xA5},
    {DSICMD_DATA,   0x0A},  // Gamma 52 / 203

    {DSICMD_CMD,    0xA6},
    {DSICMD_DATA,   0x08},  // Gamma 80 / 175

    {DSICMD_CMD,    0xA7},
    {DSICMD_DATA,   0x01},  // Gamma 108 /147

    {DSICMD_CMD,    0xA8},
    {DSICMD_DATA,   0x03},  // Gamma 147 /108

    {DSICMD_CMD,    0xA9},
    {DSICMD_DATA,   0x0B},  // Gamma 175 / 80

    {DSICMD_CMD,    0xAA},
    {DSICMD_DATA,   0x11},  // Gamma 203 / 52

    {DSICMD_CMD,    0xAB},
    {DSICMD_DATA,   0x04},  // Gamma 231 / 24

    {DSICMD_CMD,    0xAC},
    {DSICMD_DATA,   0x06},  // Gamma 239 / 16

    {DSICMD_CMD,    0xAD},
    {DSICMD_DATA,   0x15},  // Gamma 247 / 8

    {DSICMD_CMD,    0xAE},
    {DSICMD_DATA,   0x0D},  // Gamma 251 / 4

    {DSICMD_CMD,    0xAF},
    {DSICMD_DATA,   0x00},  // Gamma 255 / 0

///==============Nagitive Gamma==============///

{DSICMD_CMD,    0xC0},
    {DSICMD_DATA,   0x00},  // Gamma 0 

{DSICMD_CMD,    0xC1},
    {DSICMD_DATA,   0x0B},  // Gamma 4

{DSICMD_CMD,    0xC2},
    {DSICMD_DATA,   0x13},  // Gamma 8

{DSICMD_CMD,    0xC3},
    {DSICMD_DATA,   0x0D},  // Gamma 16

{DSICMD_CMD,    0xC4},
    {DSICMD_DATA,   0x05},  // Gamma 24

{DSICMD_CMD,    0xC5},
    {DSICMD_DATA,   0x0A},  // Gamma 52

{DSICMD_CMD,    0xc6},
    {DSICMD_DATA,   0x06},  // Gamma 80

{DSICMD_CMD,    0xC7},
    {DSICMD_DATA,   0x0e},  // Gamma 108

{DSICMD_CMD,    0xC8},
    {DSICMD_DATA,   0x0c},  // Gamma 147

{DSICMD_CMD,    0xC9},
    {DSICMD_DATA,   0x0A},  // Gamma 175

{DSICMD_CMD,    0xCA},
    {DSICMD_DATA,   0x0a},  // Gamma 203

{DSICMD_CMD,    0xCB},
    {DSICMD_DATA,   0x08},  // Gamma 231

{DSICMD_CMD,    0xCC},
    {DSICMD_DATA,   0x0d},  // Gamma 239

{DSICMD_CMD,    0xCD},
    {DSICMD_DATA,   0x20},  // Gamma 247

{DSICMD_CMD,    0xCE},
    {DSICMD_DATA,   0x16},  // Gamma 251

{DSICMD_CMD,    0xCF},
    {DSICMD_DATA,   0x00},  // Gamma 255

    {CMDDELAY_MS,   20},
/////////////////////////////////

    {DSICMD_CMD,    0xFF},
    {DSICMD_DATA,   0xFF},
    {DSICMD_DATA,   0x98},
    {DSICMD_DATA,   0x06},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x07},    // Change to Page 7
    {CMDDELAY_MS,   10},

    {DSICMD_CMD,    0x18},
    {DSICMD_DATA,   0x1D},

    {DSICMD_CMD,    0x17},
    {DSICMD_DATA,   0x12},  // VGL_REG ON   

    {DSICMD_CMD,    0x02},
    {DSICMD_DATA,   0x77},  

    {DSICMD_CMD,    0xe1},
    {DSICMD_DATA,   0x79},  


//===========Page 6 Command=============//

{DSICMD_CMD,    0xFF},
    {DSICMD_DATA,   0xFF},
    {DSICMD_DATA,   0x98},
    {DSICMD_DATA,   0x06},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x06},     // Change to Page 6

{DSICMD_CMD,    0x00},
    {DSICMD_DATA,   0x20},

{DSICMD_CMD,    0x01},
    {DSICMD_DATA,   0x05},

{DSICMD_CMD,    0x02},
    {DSICMD_DATA,   0x00},    

{DSICMD_CMD,    0x03},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x04},
    {DSICMD_DATA,   0x01},

{DSICMD_CMD,    0x05},
    {DSICMD_DATA,   0x01},

{DSICMD_CMD,    0x06},
    {DSICMD_DATA,   0x88},  

{DSICMD_CMD,    0x07},
    {DSICMD_DATA,   0x04},

{DSICMD_CMD,    0x08},
    {DSICMD_DATA,   0x01},

 {DSICMD_CMD,    0x09},
    {DSICMD_DATA,   0x90},  

 {DSICMD_CMD,    0x0A},
    {DSICMD_DATA,   0x04},  

 {DSICMD_CMD,    0x0B},
    {DSICMD_DATA,   0x01},  

{DSICMD_CMD,    0x0C},
    {DSICMD_DATA,   0x01},

 {DSICMD_CMD,    0x0D},
    {DSICMD_DATA,   0x01},

{DSICMD_CMD,    0x0E},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x0F},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x10},
    {DSICMD_DATA,   0x55},

{DSICMD_CMD,    0x11},
    {DSICMD_DATA,   0x50},

{DSICMD_CMD,    0x12},
    {DSICMD_DATA,   0x01},

{DSICMD_CMD,    0x13},
    {DSICMD_DATA,   0x0c},

{DSICMD_CMD,    0x14},
    {DSICMD_DATA,   0x0d},

{DSICMD_CMD,    0x15},
    {DSICMD_DATA,   0x43},

{DSICMD_CMD,    0x16},
    {DSICMD_DATA,   0x0B},

{DSICMD_CMD,    0x17},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x18},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x19},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x1A},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x1B},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x1C},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x1D},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x20},
    {DSICMD_DATA,   0x01},

{DSICMD_CMD,    0x21},
    {DSICMD_DATA,   0x23},

{DSICMD_CMD,    0x22},
    {DSICMD_DATA,   0x45},

{DSICMD_CMD,    0x23},
    {DSICMD_DATA,   0x67},

{DSICMD_CMD,    0x24}, 
    {DSICMD_DATA,   0x01},

{DSICMD_CMD,    0x25},
    {DSICMD_DATA,   0x23},

{DSICMD_CMD,    0x26},
    {DSICMD_DATA,   0x45},

{DSICMD_CMD,    0x27},
    {DSICMD_DATA,   0x67},

{DSICMD_CMD,    0x30},
    {DSICMD_DATA,   0x02},

{DSICMD_CMD,    0x31},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x32},
    {DSICMD_DATA,   0x11},

{DSICMD_CMD,    0x33},
    {DSICMD_DATA,   0xAA},

{DSICMD_CMD,    0x34},
    {DSICMD_DATA,   0xBB},

{DSICMD_CMD,    0x35},
    {DSICMD_DATA,   0x66},

{DSICMD_CMD,    0x36},
    {DSICMD_DATA,   0x00},

{DSICMD_CMD,    0x37},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x38},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x39},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x3A},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x3B},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x3C},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x3D},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x3E},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x3F},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x40},
    {DSICMD_DATA,   0x22},

{DSICMD_CMD,    0x53},
    {DSICMD_DATA,   0x10},  //VGLO refer VGL_REG

//========== Page 0 Command ===============//

{DSICMD_CMD,    0xFF},
    {DSICMD_DATA,   0xFF},
    {DSICMD_DATA,   0x98},
    {DSICMD_DATA,   0x06},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x00},   // Change to Page 0    
    {CMDDELAY_MS,   10},
    
    {DSICMD_CMD,    0x36},
    {DSICMD_DATA,   0x03},    // rotate 180 degree
    
    {DSICMD_CMD,    0x11},     // sleep out
    {CMDDELAY_MS,   120},


    {DSICMD_CMD,    0x29},     // display on
    {CMDDELAY_MS,   10},
    {CMDDELAY_MS,   10},
    {CMDDELAY_MS,   10},
    {CMDDELAY_MS,   10},
};
#else
{

    {DSICMD_CMD,    0xFF},   //page 1                                
    	{DSICMD_DATA,  0xFF}, //PA1                                              
    	{DSICMD_DATA,  0x98}, //PA2                                              
    	{DSICMD_DATA,  0x06}, //PA3   
    	{DSICMD_DATA,  0x04}, //PA3   
    	{DSICMD_DATA,  0x01}, //PA3   
    
    {DSICMD_CMD,    0x08},                                     
    	{DSICMD_DATA,  0x10}, 	
    {DSICMD_CMD,    0x21},                                     
    {	DSICMD_DATA,  0x01}, //PA1   	
    {DSICMD_CMD,    0x30},                                   
    	{DSICMD_DATA,  0x02}, //PA1 01  
    {DSICMD_CMD,    0x31},                                      
    	{DSICMD_DATA,  0x02}, //PA1   

    {DSICMD_CMD,    0x40},                                    
    	{DSICMD_DATA,  0x15},//16   
    {DSICMD_CMD,    0x41},                                  
    	{DSICMD_DATA,  0x24}, //22	
    {DSICMD_CMD,    0x42},                                     
    	{DSICMD_DATA,  0x02}, //01 	
    {DSICMD_CMD,    0x43},                                     
    	{DSICMD_DATA,  0x02}, //84  	
    {DSICMD_CMD,    0x44},                                    
    	{DSICMD_DATA,  0x07},//8a

    {DSICMD_CMD,    0x50},                                    
    	{DSICMD_DATA,  0x78}, //65 	
    {DSICMD_CMD,    0x51},                                   
    	{DSICMD_DATA,  0x78}, //65 

    {DSICMD_CMD,    0x52},//54                                   
    	{DSICMD_DATA,  0x00},   
    {DSICMD_CMD,    0x53},//55                                   
    	{DSICMD_DATA,  0x87},   
 	
    {DSICMD_CMD,    0x60},                                  
    	{DSICMD_DATA,  0x07},   	
    {DSICMD_CMD,    0x61},                                  
    	{DSICMD_DATA,  0x04}, 	
    {DSICMD_CMD,    0x62},                                   
    	{DSICMD_DATA,  0x08},	
    {DSICMD_CMD,    0x63},                                  
    	{DSICMD_DATA,  0x04}, 	
    {DSICMD_CMD,    0xa0},                                  
    	{DSICMD_DATA,  0x00},  	
    {DSICMD_CMD,    0xa1},                                  
    	{DSICMD_DATA,  0x09}, 	
    {DSICMD_CMD,    0xa2},                              
    	{DSICMD_DATA,  0x0f}, 
    {DSICMD_CMD,    0xa3},                                 
    	{DSICMD_DATA,  0x0c}, 	
    {DSICMD_CMD,    0xa4},                                  
    	{DSICMD_DATA,  0x05},    	
    {DSICMD_CMD,    0xa5},                                  
    	{DSICMD_DATA,  0x09}, 	
    {DSICMD_CMD,    0xa6},                                  
    	{DSICMD_DATA,  0x07}, 	
    {DSICMD_CMD,    0xa7},                                  
    	{DSICMD_DATA,  0x05}, 	
    {DSICMD_CMD,    0xa8},                                  
    	{DSICMD_DATA,  0x08}, 	
    {DSICMD_CMD,    0xa9},                                  
    	{DSICMD_DATA,  0x0b}, 	
    {DSICMD_CMD,    0xaa},                                  
    	{DSICMD_DATA,  0x11}, 	
    {DSICMD_CMD,    0xab},                                  
    	{DSICMD_DATA,  0x07}, 	
    {DSICMD_CMD,    0xac},                                  
    	{DSICMD_DATA,  0x0d}, 	
    {DSICMD_CMD,    0xad},                                  
    	{DSICMD_DATA,  0x12}, 		
    {DSICMD_CMD,    0xae},                                  
    	{DSICMD_DATA,  0x0a}, 		
    {DSICMD_CMD,    0xaf},                                  
    	{DSICMD_DATA,  0x00}, 		
    {DSICMD_CMD,    0xc0},                                  
    	{DSICMD_DATA,  0x00}, 		
    {DSICMD_CMD,    0xc1},                                  
    	{DSICMD_DATA,  0x09}, 		
    {DSICMD_CMD,    0xc2},                                  
    	{DSICMD_DATA,  0x0f}, 		
    {DSICMD_CMD,    0xc3},                                  
    	{DSICMD_DATA,  0x0c}, 		
    {DSICMD_CMD,    0xc4},                                  
    	{DSICMD_DATA,  0x05}, 		
    {DSICMD_CMD,    0xc5},                                  
    	{DSICMD_DATA,  0x08}, 		
    {DSICMD_CMD,    0xc6},                                  
    	{DSICMD_DATA,  0x07}, 		
    {DSICMD_CMD,    0xc7},                                  
    	{DSICMD_DATA,  0x04}, 		
    {DSICMD_CMD,    0xc8},                                  
    	{DSICMD_DATA,  0x08}, 		
    {DSICMD_CMD,    0xc9},                                  
    	{DSICMD_DATA,  0x0c}, 		
    {DSICMD_CMD,    0xca},                                  
    	{DSICMD_DATA,  0x11}, 		
    {DSICMD_CMD,    0xcb},                                  
    	{DSICMD_DATA,  0x08}, 		
    {DSICMD_CMD,    0xcc},                                  
    	{DSICMD_DATA,  0x0d}, 		
    {DSICMD_CMD,    0xcd},                                  
    	{DSICMD_DATA,  0x13}, 		
    {DSICMD_CMD,    0xce},                                  
    	{DSICMD_DATA,  0x0b}, 		
    {DSICMD_CMD,    0xcf},                                  
    	{DSICMD_DATA,  0x00}, 		
    {CMDDELAY_MS,   20},
/////////////////////////////////

    {DSICMD_CMD,    0xFF},
    {DSICMD_DATA,   0xFF},
    {DSICMD_DATA,   0x98},
    {DSICMD_DATA,   0x06},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x06},    // Change to Page 6
    {CMDDELAY_MS,   10},

    {DSICMD_CMD,    0x00},
    	{DSICMD_DATA,   0x20},

    {DSICMD_CMD,    0x01},
    	{DSICMD_DATA,   0x04},  // VGL_REG ON   

    {DSICMD_CMD,    0x02},
   	 	{DSICMD_DATA,   0x00},  

    {DSICMD_CMD,    0x03},
    	{DSICMD_DATA,   0x08},  

	{DSICMD_CMD,    0x04},
   	 	{DSICMD_DATA,   0x01}, 
   	 	
   	 {DSICMD_CMD,    0x05},
   	 	{DSICMD_DATA,   0x01}, 

	{DSICMD_CMD,    0x06},
   	 	{DSICMD_DATA,   0x80}, 

	{DSICMD_CMD,    0x07},
   	 	{DSICMD_DATA,   0x02}, 

	{DSICMD_CMD,    0x08},
   	 	{DSICMD_DATA,   0x05}, 

	{DSICMD_CMD,    0x09},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x0a},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x0b},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x0c},
   	 	{DSICMD_DATA,   0x01},

	{DSICMD_CMD,    0x0d},
   	 	{DSICMD_DATA,   0x01},

	{DSICMD_CMD,    0x0e},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x0f},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x10},
   	 	{DSICMD_DATA,   0xff}, 

	{DSICMD_CMD,    0x11},
   	 	{DSICMD_DATA,   0xf2}, 

	{DSICMD_CMD,    0x12},
   	 	{DSICMD_DATA,   0x01}, 

	{DSICMD_CMD,    0x13},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x14},
   	 	{DSICMD_DATA,   0x00},

	{DSICMD_CMD,    0x15},
   	 	{DSICMD_DATA,   0x43}, 

	{DSICMD_CMD,    0x16},
   	 	{DSICMD_DATA,   0x0b}, 

	{DSICMD_CMD,    0x17},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x18},
   	 	{DSICMD_DATA,   0x00},

	{DSICMD_CMD,    0x19},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x1a},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x1b},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x1c},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x1d},
   	 	{DSICMD_DATA,   0x00}, 

	{DSICMD_CMD,    0x20},
   	 	{DSICMD_DATA,   0x01}, 

	{DSICMD_CMD,    0x21},
   	 	{DSICMD_DATA,   0x23}, 

	{DSICMD_CMD,    0x22},
   	 	{DSICMD_DATA,   0x45}, 
   	 	
   	 {DSICMD_CMD,    0x23},
   	 	{DSICMD_DATA,   0x67}, 

	{DSICMD_CMD,    0x24},
   	 	{DSICMD_DATA,   0x01}, 

	{DSICMD_CMD,    0x25},
   	 	{DSICMD_DATA,   0x23}, 

	{DSICMD_CMD,    0x26},
   	 	{DSICMD_DATA,   0x45},

	{DSICMD_CMD,    0x27},
   	 	{DSICMD_DATA,   0x67}, 

	{DSICMD_CMD,    0x30},
   	 	{DSICMD_DATA,   0x13}, 

	{DSICMD_CMD,    0x31},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x32},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x33},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x34},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x35},
   	 	{DSICMD_DATA,   0xbb}, 

	{DSICMD_CMD,    0x36},
   	 	{DSICMD_DATA,   0xaa}, 

	{DSICMD_CMD,    0x37},
   	 	{DSICMD_DATA,   0xdd},

	{DSICMD_CMD,    0x38},
   	 	{DSICMD_DATA,   0xcc},

	{DSICMD_CMD,    0x39},
   	 	{DSICMD_DATA,   0x22},

	{DSICMD_CMD,    0x3a},
   	 	{DSICMD_DATA,   0x66}, 

	{DSICMD_CMD,    0x3b},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x3c},
   	 	{DSICMD_DATA,   0x88}, 

	{DSICMD_CMD,    0x3d},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x3e},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x3f},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x40},
   	 	{DSICMD_DATA,   0x22}, 

	{DSICMD_CMD,    0x52},
   	 	{DSICMD_DATA,   0x10},

	{DSICMD_CMD,    0x53},
   	 	{DSICMD_DATA,   0x12}, 

    {DSICMD_CMD,    0xff}, //page 7                                 
    {DSICMD_DATA,  0xff}, 	
    {DSICMD_DATA,    0x98},                                  
    {DSICMD_DATA,  0x06}, 	   
    {DSICMD_DATA,    0x04},                                  
    {DSICMD_DATA,  0x07},

	{DSICMD_CMD,    0x18},
   	 	{DSICMD_DATA,   0x1d}, 

	{DSICMD_CMD,    0x17},
   	 	{DSICMD_DATA,   0x32}, 

	{DSICMD_CMD,    0x02},
   	 	{DSICMD_DATA,   0x77}, 

	{DSICMD_CMD,    0xe1},
   	 	{DSICMD_DATA,   0x79}, 

	{DSICMD_CMD,    0x26},
   	 	{DSICMD_DATA,   0xb2}, 
   	 	
	{DSICMD_CMD,    0xff}, //page 0                                 
    {DSICMD_DATA,  0xff}, 	
    {DSICMD_DATA,    0x98},                                  
    {DSICMD_DATA,  0x06}, 	   
    {DSICMD_DATA,    0x04},                                  
    {DSICMD_DATA,  0x00},

	{DSICMD_CMD,    0x36},
   	 	{DSICMD_DATA,   0x02}, //RGB
   	{DSICMD_CMD,    0x3a},
   	 	{DSICMD_DATA,   0x77},
    {DSICMD_CMD,    0x11},
    	//{DSICMD_DATA,   0x00},
    {CMDDELAY_MS,   120},
    {DSICMD_CMD,    0x29},
    	//{DSICMD_DATA,   0x00},
};
#endif

const tPANEL_CMD tCmdStandbyDSI[] =
{
    {DSICMD_CMD,     0x28},     // Display OFF
    {DSICMD_CMD,     0x10}      // Sleep in
};


const tLCD_PARAM tModeDSI[] =
{
    /***********       MI Serial Format 1      *************/
    {
        // tPANEL_PARAM
        {
            /* Old prototype */
            PINMUX_DSI_2_LANE_VDO_SYNC_EVENT_RGB888,
            _IDE_fDCLK,                             //!< fDCLK
            (0xF0+PANEL_WIDTH),             //!< uiHSyncTotalPeriod(HTOTAL)
            PANEL_WIDTH,                    //!< uiHSyncActivePeriod(HACT)
            0x06,                           //!< uiHSyncBackPorch(HBP)
            0x52 + PANEL_HEIGHT,            //!< uiVSyncTotalPeriod(VTOTAL)
            PANEL_HEIGHT,                           //!< uiVSyncActivePeriod
            VVALIDST,                       //!< uiVSyncBackPorchOdd
            VVALIDST,                       //!< uiVSyncBackPorchEven
            PANEL_HEIGHT,                   //!< uiBufferWidth
            PANEL_WIDTH,                    //!< uiBufferHeight
            PANEL_WIDTH,                    //!< uiWindowWidth
            PANEL_HEIGHT,                   //!< uiWindowHeight
            FALSE,                           //!< bYCbCrFormat
            /* New added parameters */
            HSYNCT,                         //!< uiHSyncSYNCwidth
            HSYNCT                          //!< uiVSyncSYNCwidth
        },

        // tIDE_PARAM
        {
            /* Old prototype */
            PINMUX_LCD_SEL_GPIO,            //!< pinmux_select_lcd;
            ICST_CCIR601,                   //!< icst;
            {TRUE, FALSE},                  //!< dithering[2];
            DISPLAY_DEVICE_MIPIDSI,         //!< **DONT-CARE**
            IDE_PDIR_RGB,                   //!< pdir;
            IDE_LCD_R,                      //!< odd;
            IDE_LCD_G,                      //!< even;
            TRUE,                           //!< hsinv;
            TRUE,                           //!< vsinv;
            FALSE,                          //!< hvldinv;
            FALSE,                          //!< vvldinv;
            TRUE,                           //!< clkinv;
            FALSE,                          //!< fieldinv;
            FALSE,                          //!< **DONT-CARE**
            FALSE,                          //!< interlace;
            FALSE,                          //!< **DONT-CARE**
            0x40,                           //!< ctrst;
            0x00,                           //!< brt;
            0x40,                           //!< cmults;
            FALSE,                          //!< cex;
            FALSE,                          //!< **DONT-CARE**
            TRUE,                           //!< **DONT-CARE**
            TRUE,                           //!< tv_powerdown;
            {0x00,0x00},                //!< **DONT-CARE**

            /* New added parameters */
            FALSE,                          //!< YCex
            FALSE,                          //!< HLPF
            {FALSE,FALSE,FALSE},            //!< subpixOdd[3]
            {FALSE,FALSE,FALSE},            //!< subpixEven[3]
            {IDE_DITHER_5BITS,IDE_DITHER_6BITS,IDE_DITHER_5BITS},//!< DitherBits[3]
            FALSE                           //!< clk1/2
        },

        (tPANEL_CMD*)tCmdModeDSI,                  //!< pCmdQueue
        sizeof(tCmdModeDSI)/sizeof(tPANEL_CMD),    //!< nCmd
    }
};

const tLCD_ROT *tRotDSI = NULL;

//@}

tLCD_ROT* dispdev_getLcdRotateDSICmd(UINT32 *ModeNumber)
{
    if(tRotDSI != NULL)
    {
        *ModeNumber = sizeof(tRotDSI)/sizeof(tLCD_ROT);
    }
    else
    {
        *ModeNumber = 0;
    }
    return (tLCD_ROT*)tRotDSI;
}

tLCD_PARAM* dispdev_getConfigModeDSI(UINT32 *ModeNumber)
{
    *ModeNumber = sizeof(tModeDSI)/sizeof(tLCD_PARAM);
    return (tLCD_PARAM*)tModeDSI;
}

tPANEL_CMD* dispdev_getStandbyCmdDSI(UINT32 *CmdNumber)
{
    *CmdNumber = sizeof(tCmdStandbyDSI)/sizeof(tPANEL_CMD);
    return (tPANEL_CMD*)tCmdStandbyDSI;
}


void dispdev_setDSIConfig(tDSI_CONFIG *pDsiConfig)
{
    dsi_setConfig(DSI_CONFIG_ID_FREQ, DSI_TARGET_CLK * 1000000);
#if(DSI_TARGET_CLK == 160)
    dsi_setConfig(DSI_CONFIG_ID_TLPX, 1);
    dsi_setConfig(DSI_CONFIG_ID_BTA_TA_GO, 7);

    dsi_setConfig(DSI_CONFIG_ID_THS_PREPARE, 1);
    dsi_setConfig(DSI_CONFIG_ID_THS_ZERO, 4);
    dsi_setConfig(DSI_CONFIG_ID_THS_TRAIL, 2);
    dsi_setConfig(DSI_CONFIG_ID_THS_EXIT, 3);

    dsi_setConfig(DSI_CONFIG_ID_TCLK_PREPARE, 1);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_ZERO, 7);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_POST, 8);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_PRE, 1);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_TRAIL, 1);
#elif(DSI_TARGET_CLK == 480)
    dsi_setConfig(DSI_CONFIG_ID_TLPX, 3);
    dsi_setConfig(DSI_CONFIG_ID_BTA_TA_GO, 15);

    dsi_setConfig(DSI_CONFIG_ID_THS_PREPARE, 4);
    dsi_setConfig(DSI_CONFIG_ID_THS_ZERO, 5);
    dsi_setConfig(DSI_CONFIG_ID_THS_TRAIL, 4);
    dsi_setConfig(DSI_CONFIG_ID_THS_EXIT, 7);

    dsi_setConfig(DSI_CONFIG_ID_TCLK_PREPARE, 3);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_ZERO, 14);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_POST, 10);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_PRE, 1);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_TRAIL, 3);

#elif(DSI_TARGET_CLK == 240)
    dsi_setConfig(DSI_CONFIG_ID_TLPX, 1);
    dsi_setConfig(DSI_CONFIG_ID_BTA_TA_GO, 7);

    dsi_setConfig(DSI_CONFIG_ID_THS_PREPARE, 1);
    dsi_setConfig(DSI_CONFIG_ID_THS_ZERO, 4);
    dsi_setConfig(DSI_CONFIG_ID_THS_TRAIL, 3);
    dsi_setConfig(DSI_CONFIG_ID_THS_EXIT, 4);

    dsi_setConfig(DSI_CONFIG_ID_TCLK_PREPARE, 2);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_ZERO, 9);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_POST, 9);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_PRE, 1);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_TRAIL, 2);
#elif(DSI_TARGET_CLK == 120)
    dsi_setConfig(DSI_CONFIG_ID_TLPX, 0);
    dsi_setConfig(DSI_CONFIG_ID_BTA_TA_GO, 3);

    dsi_setConfig(DSI_CONFIG_ID_THS_PREPARE, 1);
    dsi_setConfig(DSI_CONFIG_ID_THS_ZERO, 2);
    dsi_setConfig(DSI_CONFIG_ID_THS_TRAIL, 1);
    dsi_setConfig(DSI_CONFIG_ID_THS_EXIT, 1);

    dsi_setConfig(DSI_CONFIG_ID_TCLK_PREPARE, 0);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_ZERO, 5);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_POST, 8);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_PRE, 1);
    dsi_setConfig(DSI_CONFIG_ID_TCLK_TRAIL, 1);
#endif
//#if (HX8379C_CFG_LANE == HX8379C_CFG_1_LANE)
//    dsi_setConfig(DSI_CONFIG_ID_DATALANE_NO, DSI_DATA_LANE_0);
//#else
    dsi_setConfig(DSI_CONFIG_ID_DATALANE_NO, DSI_DATA_LANE_1);
//#endif
    //dsi_setConfig(DSI_CONFIG_ID_TE_BTA_INTERVAL, 0x1F);
    //dsi_setConfig(DSI_CONFIG_ID_CLK_PHASE_OFS, 0x3);
    //dsi_setConfig(DSI_CONFIG_ID_PHASE_DELAY_ENABLE_OFS, 0x1);

    dsi_setConfig(DSI_CONFIG_ID_CLK_LP_CTRL, 0x1);
    //dsi_setConfig(DSI_CONFIG_ID_SYNC_DLY_CNT, 0xF);

    dsi_setConfig(DSI_CONFIG_ID_EOT_PKT_EN, 0);
}
