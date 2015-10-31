/**
    @file       GPSParser.h
    @ingroup

    @brief      Sample module brief
                Sample module description

    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.
*/

#ifndef _NMEAParser_H
#define _NMEAParser_H

#include "Type.h"
#include "UART.h"

#define _GPS_CheckSum_Enable      0   //Enable to open check sum mechanism
#define RECEIVE_FROM_UART2        1

#define NMEA_BUFFER_OFFSET       (35*1024)   //35KB
#define NMEA_BUFFER_RESERVED     128
#define NMEA_SENTENCE_SIZE       180
#define GSA_SENTENCE             0x41534750 //'A''S''G''P'
#define RMC_SENTENCE             0x434D5250 //'C''M''R''P'
#define GGA_SENTENCE             0x41474750 //'A''G''G''P'
#define GSV_SENTENCE             0x56534750 //'V''S''G''P'
#define GLL_SENTENCE             0x4C4C4750 //'L''L''G''P'
#define VTG_SENTENCE             0x47545650 //'G''T''V''P'
#define SNR_SENTENCE             0x4F534E45 //'O''S''N''E'
#define ROS_SENTENCE             0x534F5259 //'S''O''R''Y'

#define GPS_FLAG_DATAVALID      0x00000001
#define GPS_FLAG_LOG            0x00000002

//#NT#2011/1/14#Philex Lin-begin
typedef struct {
    CHAR    *pGPSDataStartAddr;
    UINT32  GPSBufUsedPos;
    UINT32  GPSBufSize;
} GPSBuf,*pGPSBuf;
//#NT#2011/1/14#Philex Lin-end
typedef struct{

    CHAR    *GGA;
    CHAR    *RMC;
    CHAR    *GSA;
    CHAR    *GSV;
    CHAR    *GLL;
    CHAR    *VTG;
    GPSBuf  GPSDataBuf;

}NMEASENTENCE;
extern NMEASENTENCE NMEASentence;

typedef enum{
    No_Fix = 1,
    TWO_D_Fix,
    THREE_D_Fix
}FIXMODE;


//GSA, Satellite status
typedef struct{

    CHAR    SelMode;          //'A' auto selection of 2D or 3D fix
                              //'M' manual
    UINT32  FixMode;          //'1' no fix
                              //'2' 2D fix
                              //'3' 3D fix
    UINT32  SatUsed01;        //PRNs of satellites used for fix (space for 12)
    UINT32  SatUsed02;
    UINT32  SatUsed03;
    UINT32  SatUsed04;
    UINT32  SatUsed05;
    UINT32  SatUsed06;
    UINT32  SatUsed07;
    UINT32  SatUsed08;
    UINT32  SatUsed09;
    UINT32  SatUsed10;
    UINT32  SatUsed11;
    UINT32  SatUsed12;
    double  PDOP;             //Position dilution of precision
    double  HDOP;             //Horizontal dilution of precision
    double  VDOP;             //Vertical dilution of precision

}GSAINFO;
extern GSAINFO GSAInfo;

//RMC, Recommended Minimum sentence C
typedef struct{

    UINT32    Hour;
    UINT32    Minute;
    UINT32    Second;
    UINT32    Year;
    UINT32    Month;
    UINT32    Day;
    CHAR      Status;              //Status A=active or V=Void
    CHAR      NSInd;
    CHAR      EWInd;
    CHAR      reservd;
    float     Latitude;
    float     Longitude;
    float     Speed;               //Speed over the ground in knots
    float     Angle;               //Track angle in degrees True

}RMCINFO;
extern RMCINFO RMCInfo;

//GGA, Global Positioning System Fix Data
typedef struct{

    UINT32    Hour;
    UINT32    Minute;
    UINT32    Second;
    double    Latitude;
    CHAR      NSInd;
    double    Longitude;
    CHAR      EWInd;
    UINT32    PosFixInd;         //Fix quality: 0 = invalid
                                 //             1 = GPS fix
    UINT32    SatNumber;         //Number of satellites being tracked
    double    HDOP;              //Horizontal dilution of position
    double    Altitude;          //Altitude, Meters, above mean sea level
    double    MSL;               //Height of geoid (mean sea level) above WGS84

}GGAINFO;
extern GGAINFO GGAInfo;

typedef struct{

    UINT32    SatNum;            //Satellite PRN number
    UINT32    Elev;              //Elevation, degrees
    UINT32    Azi;               //Azimuth, degrees
    UINT32    SNR;               //SNR - higher is better

}SATDATA;

//GSV, Satellites in view
typedef struct{

    UINT32    NumOfSen;          //Number of sentences for full data
    UINT32    SenNum;            //Sentence number
    UINT32    SatInView;         //Number of satellites in view
    SATDATA   SAT01;
    SATDATA   SAT02;
    SATDATA   SAT03;
    SATDATA   SAT04;
    SATDATA   SAT05;
    SATDATA   SAT06;
    SATDATA   SAT07;
    SATDATA   SAT08;
    SATDATA   SAT09;
    SATDATA   SAT10;
    SATDATA   SAT11;
    SATDATA   SAT12;
    SATDATA   SAT13;
    SATDATA   SAT14;
    SATDATA   SAT15;
    SATDATA   SAT16;

}GSVINFO;
extern GSVINFO GSVInfo;

typedef enum{
    GSA,
    RMC,
    GGA,
    GSV,
    GLL,
    VTG
}NMEATYPE;

//----for edog ---------------------------------------------------

typedef struct{
    UINT8    SatStatus;         //Status of satellites being tracked;      A or V
    UINT8    WarningType;              
    UINT8    SpeedLimit;              
    UINT8    Distance1;               
    UINT8    Distance2;	
    UINT8    Speed;	
    UINT8    Direction;
    UINT8    Latitude1;
    UINT8    Latitude2;
    UINT8    Latitude3;	
    UINT8    Longitude1;
    UINT8    Longitude2;
    UINT8    Longitude3;	    
}EGOD_COMMAND_G;

typedef struct{
    UINT8    RadarBand;        
    UINT8    RadarFrequency1;              
    UINT8    RadarFrequency2;              	    
}EGOD_COMMAND_R;


typedef struct{
    UINT8    Year;        
    UINT8    Month;              
    UINT8    Day;              
    UINT8    Hour;               
    UINT8    Minute;	
    UINT8    Second;	
    UINT8    Week;	    
}EGOD_COMMAND_T;


typedef struct{
    UINT8    DataVersion1;        
    UINT8    DataVersion2;              
    UINT8    FWVersion;              
    UINT8    Volume;               
    UINT8    RadarMuteSetting;	
    UINT8    RadarMode;	
    UINT8    RadarSensitivity;	    
    UINT8    OverSpeedSetting;	    
    UINT8    SpeedOffset;	    
    UINT8    VoiceSetting;	    	
}EGOD_COMMAND_P;

typedef struct{
    UINT8    GPRSStatus;           
}EGOD_COMMAND_C;

typedef enum{
	CHECK_SUM_ING,
	ERROR_1,
	ERROR_2,
	ERROR_3,
	ERROR_4,
	ERROR_RETAIN1,	
	ERROR_RETAIN2,		
	READ_KEYMESSAGE_FAILE,
	CHECKSUM_KEYMESSAGE_FAILE
}ErrorType;

typedef enum{
	CONNECT_OK=0x01,
	UPGRATING,
	UPGRATING_DONE,
	INTERRUPTION_LASTIME,
	PLEASE_CONNECT_DEVICE
}UpdateType;

typedef enum{
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,	
	FRIDAY,		
	SATURDAY
}Week;

typedef struct{
EGOD_COMMAND_G    CommandG;
EGOD_COMMAND_R    CommandR;       
EGOD_COMMAND_T    CommandT;  
EGOD_COMMAND_P    CommandP;               
EGOD_COMMAND_C    CommandC;               
UINT32 EdogErrorType;
UINT32 EdogUdateType;
}EDOG_INFO;

extern void GPS_InstallID(void);
extern ER GPSRec_Open(UART_BAUDRATE BaudRate);
extern ER GPSRec_Close(void);
extern ER GPSRec_Reset(void);
extern BOOL GPSRec_CheckData(void);

extern BOOL GPSRec_GetRMCDate(RMCINFO *pRMCInfo);
extern UINT32 GPSRec_GetDataCount();

extern BOOL EdogRec_CheckSum(CHAR *NMEA_str, UINT32 Len);

extern UINT32 _SECTION(".kercfg_data") GPSRECEIVE_ID; // GPS Task
extern UINT32 _SECTION(".kercfg_data") FLG_ID_GPS; // GPS Flag


#endif




