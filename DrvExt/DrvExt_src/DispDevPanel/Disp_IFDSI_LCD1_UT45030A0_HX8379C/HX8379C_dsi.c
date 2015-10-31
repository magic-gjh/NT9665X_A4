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

#define PANEL_TYPE_VERSION01  0X01
#define PANEL_TYPE_VERSION02  0X02//ben-HC-brightness H
#define PANEL_TYPE  PANEL_TYPE_VERSION01

#define PANEL_WIDTH     480

#if(PANEL_TYPE == PANEL_TYPE_VERSION01)
#define PANEL_HEIGHT    864//848
#else
#define PANEL_HEIGHT    864
#endif


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
#if(PANEL_TYPE == PANEL_TYPE_VERSION01)
#define VVALIDST    0x09//0x15  //uiVSyncBackPorchOdd/Even
#else
#define VVALIDST    0x0f//0x15  //uiVSyncBackPorchOdd/Even
#endif
#define HSYNCT      0x02
#define VSYNCT      0x02


/*
    Panel Parameters for TCON HX8379C
*/
//@{
/*Used in DSI*/
const tPANEL_CMD tCmdModeDSI[] =
{
#if 0
    {DSICMD_CMD,    0x01},     //Software reset
    {CMDDELAY_MS,   10},

    {DSICMD_CMD,    0xB9},     //Set EXTC
    {DSICMD_DATA,   0xFF},
    {DSICMD_DATA,   0x83},
    {DSICMD_DATA,   0x79},

    {DSICMD_CMD,    0x36},     //set Address mode
    {DSICMD_DATA,   0x02},

    {DSICMD_CMD,    0xB1},     // set Power
    {DSICMD_DATA,   0x64},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x31},
    {DSICMD_DATA,   0x91},
    {DSICMD_DATA,   0x90},
    {DSICMD_DATA,   0xF0},
    {DSICMD_DATA,   0xEC},
    {DSICMD_DATA,   0xD8},
    {DSICMD_DATA,   0x80},
    {DSICMD_DATA,   0x38},
    {DSICMD_DATA,   0x38},
    {DSICMD_DATA,   0xF8},
    {DSICMD_DATA,   0x44},
    {DSICMD_DATA,   0x44},
    {DSICMD_DATA,   0x44},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x80},
    {DSICMD_DATA,   0x30},
    {DSICMD_DATA,   0x00},

    {DSICMD_CMD,    0xB2},
    {DSICMD_DATA,   0x80},
    {DSICMD_DATA,   0xFE},
    {DSICMD_DATA,   0x0B},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x30},
    {DSICMD_DATA,   0x50},
    {DSICMD_DATA,   0x11},
    {DSICMD_DATA,   0x42},
    {DSICMD_DATA,   0x1D},

    {DSICMD_CMD,    0xB4},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x80},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x86},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x86},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x86},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x86},

    {DSICMD_CMD,    0xCC},
    {DSICMD_DATA,   0x02},

    {DSICMD_CMD,    0xD3},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00 },
    {DSICMD_DATA,   0x00 },
    {DSICMD_DATA,   0x00 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x32 },
    {DSICMD_DATA,   0x10 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x00 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x03 },
    {DSICMD_DATA,   0x70 },
    {DSICMD_DATA,   0x03 },
    {DSICMD_DATA,   0x70 },
    {DSICMD_DATA,   0x00 },
    {DSICMD_DATA,   0x08 },
    {DSICMD_DATA,   0x00 },
    {DSICMD_DATA,   0x08 },
    {DSICMD_DATA,   0x11 },
    {DSICMD_DATA,   0x11 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x13 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x06 },
    {DSICMD_DATA,   0x13 },
    {DSICMD_DATA,   0x09},

    {DSICMD_CMD,    0xD5},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18 },
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x20},
    {DSICMD_DATA,   0x21},
    {DSICMD_DATA,   0x19},
    {DSICMD_DATA,   0x19},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},

    {DSICMD_CMD,    0xD6},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x19},
    {DSICMD_DATA,   0x19},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x21},
    {DSICMD_DATA,   0x20},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x18},

    {DSICMD_CMD,    0xE0},    //  Set Gamma
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x1B},
    {DSICMD_DATA,   0x19},
    {DSICMD_DATA,   0x3F},
    {DSICMD_DATA,   0x13},
    {DSICMD_DATA,   0x31},
    {DSICMD_DATA,   0x0A},
    {DSICMD_DATA,   0x0C},
    {DSICMD_DATA,   0x0C},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x0D},
    {DSICMD_DATA,   0x10},
    {DSICMD_DATA,   0x17},
    {DSICMD_DATA,   0x12},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x13},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x1A},
    {DSICMD_DATA,   0x1C},
    {DSICMD_DATA,   0x3F},
    {DSICMD_DATA,   0x12},
    {DSICMD_DATA,   0x31},
    {DSICMD_DATA,   0x0A},
    {DSICMD_DATA,   0x0B},
    {DSICMD_DATA,   0x0C},
    {DSICMD_DATA,   0x18},
    {DSICMD_DATA,   0x10},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x13},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x15},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x13},
    {DSICMD_DATA,   0x15},
    {DSICMD_DATA,   0x1A},

    {DSICMD_CMD,    0xB6},
    {DSICMD_DATA,   0x47},
    {DSICMD_DATA,   0x47},

    {DSICMD_CMD,    0x11},     // sleep out
    {CMDDELAY_MS,   120},


    {DSICMD_CMD,    0x29},     // display on
    {CMDDELAY_MS,   10},
    {CMDDELAY_MS,   10},
    {CMDDELAY_MS,   10},
    {CMDDELAY_MS,   10},
#elif(PANEL_TYPE == PANEL_TYPE_VERSION01)  //version 1
                                                  
    {DSICMD_CMD,    0xB9}, //Set Password                                     
    {DSICMD_DATA,  0xFF}, //PA1                                              
    {DSICMD_DATA,  0x83}, //PA2                                              
    {DSICMD_DATA,  0x79}, //PA3              

    {DSICMD_CMD,    0x36},     //set Address mode , rotate 
    {DSICMD_DATA,   0x02},//    {DSICMD_DATA,   0x01},
                                                 
    {DSICMD_CMD,   0xB1}, 
    {DSICMD_DATA,  0x64}, 
    {DSICMD_DATA,  0x17}, 
    {DSICMD_DATA,  0x17}, 
    {DSICMD_DATA,  0x31}, 
    {DSICMD_DATA,  0x31}, 
    {DSICMD_DATA,  0x50}, 
    {DSICMD_DATA,  0xD0}, 
    {DSICMD_DATA,  0xEC}, 
    {DSICMD_DATA,  0x54}, 
    {DSICMD_DATA,  0x80}, 
    {DSICMD_DATA,  0x38}, 
    {DSICMD_DATA,  0x38}, 
    {DSICMD_DATA,  0xF8}, 
    {DSICMD_DATA,  0x22}, 
    {DSICMD_DATA,  0x22}, 
    {DSICMD_DATA,  0x22}, 
    {DSICMD_DATA,  0x00}, 
    {DSICMD_DATA,  0x80}, 
    {DSICMD_DATA,  0x30}, 
    {DSICMD_DATA,  0x00},                                            
                                                
    {DSICMD_CMD,   0xB2}, //                                               
    {DSICMD_DATA,  0x80}, //PA1    column                                   
    {DSICMD_DATA,  0x44}, //PA2    480*864//480*854                                
    {DSICMD_DATA,  0x0B}, //PA3    BP=VSA+VBP                              
    {DSICMD_DATA,  0x04}, //PA4    FP=VFP-2                                
    {DSICMD_DATA,  0x00}, //PA5    SAP=0                                       
    {DSICMD_DATA,  0x50}, //PA6                                            
    {DSICMD_DATA,  0x11}, //PA7                                            
    {DSICMD_DATA,  0x42}, //PA8                                            
    {DSICMD_DATA,  0x1D}, //PA9   Video_GEN_EN=1                                         
                                                 
    {DSICMD_CMD,   0xB4}, //                                                 
    {DSICMD_DATA,  0x6E}, //PA1    SPON                                      
    {DSICMD_DATA,  0x6E}, //PA2    SPOFF                                     
    {DSICMD_DATA,  0x6E}, //PA3    CON                                       
    {DSICMD_DATA,  0x6E}, //PA4    COFF                                      
    {DSICMD_DATA,  0x6E}, //PA5    CON1                                      
    {DSICMD_DATA,  0x6E}, //PA6    COFF1                                     
    {DSICMD_DATA,  0x22}, //PA7    EQON1                                     
    {DSICMD_DATA,  0x86}, //PA8    EQON2                                     
    {DSICMD_DATA,  0x23}, //PA9    SON                                       
    {DSICMD_DATA,  0x86}, //PA10   SOFF                                      
                                                 
    {DSICMD_CMD,   0xCC}, //                                                 
    {DSICMD_DATA,  0x0A}, //PA1                                             
                                                  
    {DSICMD_CMD,   0xD2}, //                                                 
    {DSICMD_DATA,  0x22}, //PA1                                              
                                                  
    {DSICMD_CMD,   0xD3},  //                                                
    {DSICMD_DATA,  0x00},  //PA1   Default                                   
    {DSICMD_DATA,  0x07},  //PA2   EQ_DELAY[7:0]=7                                   
    {DSICMD_DATA,  0x00},  //PA3   Default                                   
    {DSICMD_DATA,  0x00},  //PA4   GTO                                       
    {DSICMD_DATA,  0x00},  //PA5   GNO                                       
    {DSICMD_DATA,  0x06},  //PA6   USER_GIP_GATE                             
    {DSICMD_DATA,  0x06},  //PA7   USER_GIP_GATE1                            
    {DSICMD_DATA,  0x32},  //PA8   SHR0_3,SHR0_2                             
    {DSICMD_DATA,  0x10},  //PA9   SHR0_1,SHR0[11:8]                         
    {DSICMD_DATA,  0x04},  //PA10  SHR0[7:0]                                 
    {DSICMD_DATA,  0x00},  //PA11         SHR0_GS[11:8]                      
    {DSICMD_DATA,  0x04},  //PA12  SHR0_GS[7:0]                              
    {DSICMD_DATA,  0x03},  //PA13         SHR1[11:8]                         
    {DSICMD_DATA,  0x6E},  //PA14  SHR1[7:0]                                 
    {DSICMD_DATA,  0x03},  //PA15         SHR1_GS[11:8]                      
    {DSICMD_DATA,  0x6E},  //PA16  SHR1_GS[7:0]                              
    {DSICMD_DATA,  0x00},  //PA17         SHR2[11:8]                         
    {DSICMD_DATA,  0x06},  //PA18  SHR2[7:0]                                 
    {DSICMD_DATA,  0x00},  //PA19         SHR2_GS[11:8]                      
    {DSICMD_DATA,  0x06},  //PA20  SHR2_GS[7:0]                              
    {DSICMD_DATA,  0x21},  //PA21  SHP0,SCP                                  
    {DSICMD_DATA,  0x22},  //PA22  SHP2,SHP1                                 
    {DSICMD_DATA,  0x05},  //PA23  CHR0                                      
    {DSICMD_DATA,  0x05},  //PA24  CHR0_GS                                   
    {DSICMD_DATA,  0x23},  //PA25  CHP0,CCP0                                 
    {DSICMD_DATA,  0x05},  //PA26  CHR1                                      
    {DSICMD_DATA,  0x05},  //PA27  CHR1_GS                                   
    {DSICMD_DATA,  0x23},  //PA28  CHP1,CCP1                                 
    {DSICMD_DATA,  0x09},  //PA29  vbp_setting=VSA+VBP-2                     
                                                   
    {DSICMD_CMD,   0xD5},  //Forward Scan                                    
    {DSICMD_DATA,  0x18},  //PA1   CGOUT0_L                                  
    {DSICMD_DATA,  0x18},  //PA2   CGOUT0_R                                  
    {DSICMD_DATA,  0x19},  //PA3   CGOUT1_L                                  
    {DSICMD_DATA,  0x19},  //PA4   CGOUT1_R                                  
    {DSICMD_DATA,  0x01},  //PA5   CGOUT2_L                                  
    {DSICMD_DATA,  0x00},  //PA6   CGOUT2_R                                  
    {DSICMD_DATA,  0x03},  //PA7   CGOUT3_L                                  
    {DSICMD_DATA,  0x02},  //PA8   CGOUT3_R                                  
    {DSICMD_DATA,  0x21},  //PA9   CGOUT4_L                                  
    {DSICMD_DATA,  0x20},  //PA10  CGOUT4_R                                  
    {DSICMD_DATA,  0x18},  //PA11  CGOUT5_L                                  
    {DSICMD_DATA,  0x18},  //PA12  CGOUT5_R                                  
    {DSICMD_DATA,  0x18},  //PA13  CGOUT6_L                                  
    {DSICMD_DATA,  0x18},  //PA14  CGOUT6_R                                  
    {DSICMD_DATA,  0x18},  //PA15  CGOUT7_L                                  
    {DSICMD_DATA,  0x18},  //PA16  CGOUT7_R                                  
    {DSICMD_DATA,  0x18},  //PA17  CGOUT8_L                                  
    {DSICMD_DATA,  0x18},  //PA18  CGOUT8_R                                  
    {DSICMD_DATA,  0x18},  //PA19  CGOUT9_L                                  
    {DSICMD_DATA,  0x18},  //PA20  CGOUT9_R                                  
    {DSICMD_DATA,  0x18},  //PA21  CGOUT10_L                                 
    {DSICMD_DATA,  0x18},  //PA22  CGOUT10_R                                 
    {DSICMD_DATA,  0x18},  //PA23  CGOUT11_L                                 
    {DSICMD_DATA,  0x18},  //PA24  CGOUT11_R                                 
    {DSICMD_DATA,  0x18},  //PA25  CGOUT12_L                                 
    {DSICMD_DATA,  0x18},  //PA26  CGOUT12_R                                 
    {DSICMD_DATA,  0x18},  //PA27  CGOUT13_L                                 
    {DSICMD_DATA,  0x18},  //PA28  CGOUT13_R                                 
    {DSICMD_DATA,  0x18},  //PA29  CGOUT14_L                                 
    {DSICMD_DATA,  0x18},  //PA30  CGOUT14_R                                 
    {DSICMD_DATA,  0x18},  //PA31  CGOUT15_L                                 
    {DSICMD_DATA,  0x18},  //PA32  CGOUT15_R                                 
                                                  
    {DSICMD_CMD,   0xD6},  //Backward Scan                                   
    {DSICMD_DATA,  0x18},  //PA1   CGOUT0_L                                  
    {DSICMD_DATA,  0x18},  //PA2   CGOUT0_R                                  
    {DSICMD_DATA,  0x18},  //PA3   CGOUT1_L                                  
    {DSICMD_DATA,  0x18},  //PA4   CGOUT1_R                                  
    {DSICMD_DATA,  0x00},  //PA5   CGOUT2_L                                  
    {DSICMD_DATA,  0x01},  //PA6   CGOUT2_R                                  
    {DSICMD_DATA,  0x02},  //PA7   CGOUT3_L                                  
    {DSICMD_DATA,  0x03},  //PA8   CGOUT3_R                                  
    {DSICMD_DATA,  0x20},  //PA9   CGOUT4_L                                  
    {DSICMD_DATA,  0x21},  //PA10  CGOUT4_R                                  
    {DSICMD_DATA,  0x19},  //PA11  CGOUT5_L                                  
    {DSICMD_DATA,  0x19},  //PA12  CGOUT5_R                                  
    {DSICMD_DATA,  0x18},  //PA13  CGOUT6_L                                  
    {DSICMD_DATA,  0x18},  //PA14  CGOUT6_R                                  
    {DSICMD_DATA,  0x18},  //PA15  CGOUT7_L                                  
    {DSICMD_DATA,  0x18},  //PA16  CGOUT7_R                                  
    {DSICMD_DATA,  0x18},  //PA17  CGOUT8_L                                  
    {DSICMD_DATA,  0x18},  //PA18  CGOUT8_R                                  
    {DSICMD_DATA,  0x18},  //PA19  CGOUT9_L                                  
    {DSICMD_DATA,  0x18},  //PA20  CGOUT9_R                                  
    {DSICMD_DATA,  0x18},  //PA21  CGOUT10_L                                 
    {DSICMD_DATA,  0x18},  //PA22  CGOUT10_R                                 
    {DSICMD_DATA,  0x18},  //PA23  CGOUT11_L                                 
    {DSICMD_DATA,  0x18},  //PA24  CGOUT11_R                                 
    {DSICMD_DATA,  0x18},  //PA25  CGOUT12_L                                 
    {DSICMD_DATA,  0x18},  //PA26  CGOUT12_R                                 
    {DSICMD_DATA,  0x18},  //PA27  CGOUT13_L                                 
    {DSICMD_DATA,  0x18},  //PA28  CGOUT13_R                                 
    {DSICMD_DATA,  0x18},  //PA29  CGOUT14_L                                 
    {DSICMD_DATA,  0x18},  //PA30  CGOUT14_R                                 
    {DSICMD_DATA,  0x18},  //PA31  CGOUT15_L                                 
    {DSICMD_DATA,  0x18},  //PA32  CGOUT15_R                                 
                                                  
    {DSICMD_CMD,   0xE0}, 
    {DSICMD_DATA,  0x00}, 
    {DSICMD_DATA,  0x04}, 
    {DSICMD_DATA,  0x08}, 
    {DSICMD_DATA,  0x28}, 
    {DSICMD_DATA,  0x2F}, 
    {DSICMD_DATA,  0x3F}, 
    {DSICMD_DATA,  0x16}, 
    {DSICMD_DATA,  0x36}, 
    {DSICMD_DATA,  0x06}, 
    {DSICMD_DATA,  0x0A}, 
    {DSICMD_DATA,  0x0B}, 
    {DSICMD_DATA,  0x16}, 
    {DSICMD_DATA,  0x0E}, 
    {DSICMD_DATA,  0x12}, 
    {DSICMD_DATA,  0x15}, 
    {DSICMD_DATA,  0x13}, 
    {DSICMD_DATA,  0x15}, 
    {DSICMD_DATA,  0x06}, 
    {DSICMD_DATA,  0x10}, 
    {DSICMD_DATA,  0x13}, 
    {DSICMD_DATA,  0x18}, 
    {DSICMD_DATA,  0x00}, 
    {DSICMD_DATA,  0x04}, 
    {DSICMD_DATA,  0x09}, 
    {DSICMD_DATA,  0x29}, 
    {DSICMD_DATA,  0x30}, 
    {DSICMD_DATA,  0x3F}, 
    {DSICMD_DATA,  0x15}, 
    {DSICMD_DATA,  0x36}, 
    {DSICMD_DATA,  0x06}, 
    {DSICMD_DATA,  0x09},
    {DSICMD_DATA,  0x0C}, 
    {DSICMD_DATA,  0x17}, 
    {DSICMD_DATA,  0x0F}, 
    {DSICMD_DATA,  0x13}, 
    {DSICMD_DATA,  0x16}, 
    {DSICMD_DATA,  0x13}, 
    {DSICMD_DATA,  0x14}, 
    {DSICMD_DATA,  0x07}, 
    {DSICMD_DATA,  0x11}, 
    {DSICMD_DATA,  0x12}, 
    {DSICMD_DATA,  0x17},                                          
	                                                 
    {DSICMD_CMD,   0xB6}, //                                                 
    {DSICMD_DATA,  0x4E}, // PA1 0x43                                           
    {DSICMD_DATA,  0x4E}, // PA2 0x43                                          
                                                   
    {DSICMD_CMD,   0x11}, //Sleep Out  
    {CMDDELAY_MS,   130},
                                                   
    {DSICMD_CMD,   0x29}, //Display On                                       
#elif(PANEL_TYPE == PANEL_TYPE_VERSION02)  //version 1
        //AP
        //B9h//		
        {DSICMD_CMD,  0xB9},  // SET password
        {DSICMD_DATA,  0xFF},  
        {DSICMD_DATA,  0x83},  
        {DSICMD_DATA,  0x79},  

        {DSICMD_CMD,    0x36},     //set Address mode
        {DSICMD_DATA,   0x01},//    {DSICMD_DATA,   0x01},

        //B1h//

        {DSICMD_CMD,  0xB1}, 
        {DSICMD_DATA,  0x44},  //00},  //00},    
        {DSICMD_DATA,  0x18},  //50},  //50},    
        {DSICMD_DATA,  0x18},  //44},  //34}, //44               
        {DSICMD_DATA,  0x31},  //EE},  //E8},    
        {DSICMD_DATA,  0x51},  //90},  //4F},    
        {DSICMD_DATA,  0x50},  //08},  //08},    
        {DSICMD_DATA,  0xD0},  //11},  //11},    
        {DSICMD_DATA,  0xF6},  //12},  //11},    

        {DSICMD_DATA,  0x94},  //74},  //11},                   
        {DSICMD_DATA,  0x80},  //24},  //28}, //2F  //27   
        {DSICMD_DATA,  0x38},  //2C},  //30}, //37  //27 
        {DSICMD_DATA,  0x38},  //AA},  //9A},         //aa
        {DSICMD_DATA,  0xF8},  //2A},  //1A},        //2a
        {DSICMD_DATA,  0x33},  //42},  //42},  //42  

        {DSICMD_DATA,  0x32},  //0B},  //05},    
        {DSICMD_DATA,  0x22},  //5E
        {DSICMD_DATA,  0x00},  //F1},  //F1},    
        {DSICMD_DATA,  0x80},  //00},  //00},    
        {DSICMD_DATA,  0x30},  //E6},  //E6},    
        {DSICMD_DATA,  0x00},                                                                                 
                                                                                                           

        //B2h//
        {DSICMD_CMD,  0xB2}, 
        {DSICMD_DATA,  0x82}, //00},
        {DSICMD_DATA,  0xFE}, //00},  FE  
        {DSICMD_DATA,  0x0F}, //3C},    
        {DSICMD_DATA,  0x0D}, //08},  
        {DSICMD_DATA,  0x00}, //00},
        {DSICMD_DATA,  0x50}, //00},    
        {DSICMD_DATA,  0x11}, //3C},    
        {DSICMD_DATA,  0x42}, //08}, 
        {DSICMD_DATA,  0x1D}, //08},                                                          
       

        //B4h//
        {DSICMD_CMD,  0xB4},  
        {DSICMD_DATA,  0x08},  // => PA1 80},  //00},  //80    
        {DSICMD_DATA,  0x50},  // => PA2 08},  //08},    
        {DSICMD_DATA,  0x18},  //18// => PA3 00},  //00},    
        {DSICMD_DATA,  0x6D},  //78// => PA4 32},  //32},    
        {DSICMD_DATA,  0x15},  // => PA5 10},  //10},    
        {DSICMD_DATA,  0x56},  // => PA6 08},  //01}, //00   
        {DSICMD_DATA,  0x15},  // => PA7 32},  //00},    
        {DSICMD_DATA,  0x70},  // => PA8 13},  //00},    
        {DSICMD_DATA,  0x18},  // => PA9 30},  //00},    
        {DSICMD_DATA,  0x70},  // => PA1032},  //00},    
                  

        //D2h//
        {DSICMD_CMD,  0xD2},  
        {DSICMD_DATA,  0x33}, 	


        //D3h//
        {DSICMD_CMD,  0xD3},  
        {DSICMD_DATA,  0x00}, 
        {DSICMD_DATA,  0x07}, 	 
        {DSICMD_DATA,  0x00}, 	 
        {DSICMD_DATA,  0x00}, 	
        {DSICMD_DATA,  0x00}, 	
        {DSICMD_DATA,  0x08},
        {DSICMD_DATA,  0x08},
        {DSICMD_DATA,  0x32},

        {DSICMD_DATA,  0x10},
        {DSICMD_DATA,  0x08},
        {DSICMD_DATA,  0x00},
        {DSICMD_DATA,  0x08},
        {DSICMD_DATA,  0x03},
        {DSICMD_DATA,  0x62},//65

        {DSICMD_DATA,  0x03},
        {DSICMD_DATA,  0x65},
        {DSICMD_DATA,  0x00},
        {DSICMD_DATA,  0x08},
        {DSICMD_DATA,  0x00},
        {DSICMD_DATA,  0x08},
        {DSICMD_DATA,  0x37},
        {DSICMD_DATA,  0x33},
        {DSICMD_DATA,  0x0B},
        {DSICMD_DATA,  0x0B},

        {DSICMD_DATA,  0x37},//27
        {DSICMD_DATA,  0x0B},
        {DSICMD_DATA,  0x0B},
        {DSICMD_DATA,  0x37},
        {DSICMD_DATA,  0x0D},


        //D5h//
        {DSICMD_CMD,  0xD5},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x23},
        {DSICMD_DATA,  0x22},
        {DSICMD_DATA,  0x21},
        {DSICMD_DATA,  0x20},
        {DSICMD_DATA,  0x01},
        {DSICMD_DATA,  0x00},
        {DSICMD_DATA,  0x03},
        {DSICMD_DATA,  0x02},
        {DSICMD_DATA,  0x05},
        {DSICMD_DATA,  0x04},
        {DSICMD_DATA,  0x07},
        {DSICMD_DATA,  0x06},
        {DSICMD_DATA,  0x25},
        {DSICMD_DATA,  0x24},
        {DSICMD_DATA,  0x27},
        {DSICMD_DATA,  0x26},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},


        //D6h//
        {DSICMD_CMD,  0xD6},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x26},
        {DSICMD_DATA,  0x27},
        {DSICMD_DATA,  0x24},
        {DSICMD_DATA,  0x25},
        {DSICMD_DATA,  0x00},
        {DSICMD_DATA,  0x01},
        {DSICMD_DATA,  0x06},
        {DSICMD_DATA,  0x07},
        {DSICMD_DATA,  0x04},
        {DSICMD_DATA,  0x05},
        {DSICMD_DATA,  0x02},
        {DSICMD_DATA,  0x03},
        {DSICMD_DATA,  0x22},
        {DSICMD_DATA,  0x23},
        {DSICMD_DATA,  0x20},
        {DSICMD_DATA,  0x21},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},
        {DSICMD_DATA,  0x18},


        //E0h//
        {DSICMD_CMD,  0xE0},
        {DSICMD_DATA,  0x00},// => PA1 
        {DSICMD_DATA,  0x00},// => PA2 
        {DSICMD_DATA,  0x11},// => PA3 
        {DSICMD_DATA,  0x19},// => PA4 
        {DSICMD_DATA,  0x1C},// => PA5 
        {DSICMD_DATA,  0x3F},// => PA6 
        {DSICMD_DATA,  0x32},// => PA7 
        {DSICMD_DATA,  0x3D},// => PA8 
        {DSICMD_DATA,  0x0C},// => PA9 
        {DSICMD_DATA,  0x11},// => PA10
        {DSICMD_DATA,  0x12},// => PA11
        {DSICMD_DATA,  0x1A},// => PA12
        {DSICMD_DATA,  0x0F},// => PA13
        {DSICMD_DATA,  0x14},// => PA14
        {DSICMD_DATA,  0x16},// => PA15
        {DSICMD_DATA,  0x14},// => PA16
        {DSICMD_DATA,  0x15},// => PA17
        {DSICMD_DATA,  0x09},// => PA18
        {DSICMD_DATA,  0x13},// => PA19
        {DSICMD_DATA,  0x13},// => PA20
        {DSICMD_DATA,  0x17},// => PA21

        {DSICMD_DATA,  0x00},// => PA1 
        {DSICMD_DATA,  0x01},// => PA2 
        {DSICMD_DATA,  0x10},// => PA3 
        {DSICMD_DATA,  0x19},// => PA4 
        {DSICMD_DATA,  0x1B},// => PA5 
        {DSICMD_DATA,  0x3F},// => PA6 
        {DSICMD_DATA,  0x32},// => PA7 
        {DSICMD_DATA,  0x3D},// => PA8 
        {DSICMD_DATA,  0x0C},// => PA9 
        {DSICMD_DATA,  0x11},// => PA10
        {DSICMD_DATA,  0x13},// => PA11
        {DSICMD_DATA,  0x1A},// => PA12
        {DSICMD_DATA,  0x10},// => PA13
        {DSICMD_DATA,  0x14},// => PA14
        {DSICMD_DATA,  0x17},// => PA15
        {DSICMD_DATA,  0x15},// => PA16
        {DSICMD_DATA,  0x16},// => PA17
        {DSICMD_DATA,  0x08},// => PA18
        {DSICMD_DATA,  0x13},// => PA19
        {DSICMD_DATA,  0x12},// => PA20
        {DSICMD_DATA,  0x18},// => PA21
  
		
        //B6h//
        							
        {DSICMD_DATA,  0xB6},  											
        {DSICMD_DATA,  0x76},
        {DSICMD_DATA,  0x76},  
        //    {DSICMD_DATA,  0x00}, 

        //CCh//
        {DSICMD_CMD,  0xCC},
        {DSICMD_DATA,  0x0A},//0a


        //11h//

        {DSICMD_CMD,  0x11}, //Sleep Out
        {CMDDELAY_MS,   100},

        //29h//

        {DSICMD_CMD,  0x29}, //Display On
        {CMDDELAY_MS,   100},
#else //version 3
    {DSICMD_CMD,    0xFF},     //Software reset
    
    {DSICMD_DATA,    0xCC},     //Set EXTC
    {DSICMD_DATA,   0x33},
    {DSICMD_DATA,   0x12},
    {DSICMD_DATA,   0x82},


    {DSICMD_CMD,    0xF7},     // set Power
    {DSICMD_DATA,   0x65},
    {DSICMD_DATA,   0xC0},
    {DSICMD_DATA,   0x40},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xB0},
    {DSICMD_DATA,   0x98},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x40},
    {DSICMD_DATA,   0x54},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0xC0},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x42},
    {DSICMD_DATA,   0x28},

    {DSICMD_CMD,    0xF0},
    {DSICMD_DATA,   0x55},
    {DSICMD_DATA,   0xAA},
    {DSICMD_DATA,   0x52},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x01},

    {DSICMD_CMD,    0xB0},
    {DSICMD_DATA,   0x0a},
    {DSICMD_DATA,   0x0a},
    {DSICMD_DATA,   0x0a},

    {DSICMD_CMD,    0xB1},
    {DSICMD_DATA,   0x0a},
    {DSICMD_DATA,   0x0a},
    {DSICMD_DATA,   0x0a},

    {DSICMD_CMD,    0xB2},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},

    {DSICMD_CMD,    0xB3},
    {DSICMD_DATA,   0x06},
    {DSICMD_DATA,   0x06},
    {DSICMD_DATA,   0x06},

  
    {DSICMD_CMD,    0xB5},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x08},

    {DSICMD_CMD,    0xB6},
    {DSICMD_DATA,   0x54},
    {DSICMD_DATA,   0x54},
    {DSICMD_DATA,   0x54},

    {DSICMD_CMD,    0xB7},
    {DSICMD_DATA,   0x34},
    {DSICMD_DATA,   0x34},
    {DSICMD_DATA,   0x34},

    {DSICMD_CMD,    0xB8},
    {DSICMD_DATA,   0x34},
    {DSICMD_DATA,   0x34},
    {DSICMD_DATA,   0x34},

    {DSICMD_CMD,    0xB9},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x14},

    {DSICMD_CMD,    0xBA},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x14},
    {DSICMD_DATA,   0x14},

    {DSICMD_CMD,    0xBC},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x78},
    {DSICMD_DATA,   0x00},

    {DSICMD_CMD,    0xBD},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x78},
    {DSICMD_DATA,   0x00},


    {DSICMD_CMD,    0xBE},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x44},

    {DSICMD_CMD,    0xBF},
    {DSICMD_DATA,   0x01},

    {DSICMD_CMD,    0xD0},
    {DSICMD_DATA,   0x0B},
    {DSICMD_DATA,   0x0F},
    {DSICMD_DATA,   0x10},
    {DSICMD_DATA,   0x10},

    {DSICMD_CMD,    0xD1},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x87},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0xCF},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x1C},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x31},
    
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x4F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x68},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x8F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xAD},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xDD},
    
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x05},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x43},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x77},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x79},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xAA},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xE2},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x3A},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x5C},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x90},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xAE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xD4},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xF1},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFC},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFF},

    {DSICMD_CMD,    0xD2},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x87},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0xCF},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x1C},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x31},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x4F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x68},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x8F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xAD},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xDD},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x05},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x43},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x77},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x79},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xAA},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xE2},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x3A},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x5C},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x90},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xAE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xD4},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xF1},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFC},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFF},

    {DSICMD_CMD,    0xD3},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x87},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0xCF},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x1C},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x31},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x4F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x68},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x8F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xAD},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xDD},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x05},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x43},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x77},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x79},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xAA},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xE2},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x3A},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x5C},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x90},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xAE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xD4},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xF1},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFC},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFF},

    {DSICMD_CMD,    0xD4},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x87},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0xCF},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x1C},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x31},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x4F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x68},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x8F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xAD},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xDD},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x05},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x43},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x77},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x79},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xAA},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xE2},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x3A},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x5C},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x90},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xAE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xD4},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xF1},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFC},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFF},


    {DSICMD_CMD,    0xD5},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x87},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0xCF},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x1C},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x31},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x4F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x68},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x8F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xAD},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xDD},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x05},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x43},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x77},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x79},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xAA},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xE2},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x3A},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x5C},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x90},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xAE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xD4},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xF1},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFC},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFF},


    {DSICMD_CMD,    0xD6},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x87},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0xCF},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x1C},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x31},

    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x4F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x68},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x8F},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xAD},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0xDD},

    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x05},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x43},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x77},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x79},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xAA},

    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0xE2},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x04},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x3A},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x5C},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0x90},

    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xAE},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xD4},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xF1},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFC},
    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFE},

    {DSICMD_DATA,   0x03},
    {DSICMD_DATA,   0xFF},

    {DSICMD_CMD,    0xF0},
    {DSICMD_DATA,   0x55},
    {DSICMD_DATA,   0xAA},
    {DSICMD_DATA,   0x52},
    {DSICMD_DATA,   0x08},
    {DSICMD_DATA,   0x00},
    	
    {DSICMD_CMD,    0xB0},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x1E},
    {DSICMD_DATA,   0x0A},
    {DSICMD_DATA,   0x50},
    {DSICMD_DATA,   0x0A},   //VBP=30,VFP=10,HBP=80,HFP=10 

    {DSICMD_CMD,    0xB1},
    {DSICMD_DATA,   0xFC},
    {DSICMD_DATA,   0x00}, 

    {DSICMD_CMD,    0xB4},
    {DSICMD_DATA,   0x00},
    	
    {DSICMD_CMD,    0xB5},
    {DSICMD_DATA,   0x70},   ///6B //480*854

    {DSICMD_CMD,    0xB6},
    {DSICMD_DATA,   0x04},	


    {DSICMD_CMD,    0xB7},
    {DSICMD_DATA,   0x70},
    {DSICMD_DATA,   0x70},

    {DSICMD_CMD,    0xB8},
    {DSICMD_DATA,   0x01},
    {DSICMD_DATA,   0x07},
    {DSICMD_DATA,   0x07},
    {DSICMD_DATA,   0x07},

    {DSICMD_CMD,    0xBC},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},
    {DSICMD_DATA,   0x00},

    {DSICMD_CMD,    0xC9},
    {DSICMD_DATA,   0x40},
    {DSICMD_DATA,   0x02},
    {DSICMD_DATA,   0x50},
    {DSICMD_DATA,   0x50},
    {DSICMD_DATA,   0x50},

    {DSICMD_CMD,    0x3A},
    {DSICMD_DATA,   0x77},

    {DSICMD_CMD,    0x11},
    {DSICMD_DATA,   0x00},
    {CMDDELAY_MS,   120},

    {DSICMD_CMD,    0x29},
    {DSICMD_DATA,   0x00},
    {CMDDELAY_MS,   150},

#endif
};


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
