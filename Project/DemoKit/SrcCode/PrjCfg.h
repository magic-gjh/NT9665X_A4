/**
    Copyright   Novatek Microelectronics Corp. 2009.  All rights reserved.

    @file       PrjCfg.h
    @ingroup

    @brief

    @note       Nothing.
*/

#ifndef _PRJCFG_H_
#define _PRJCFG_H_

#include "Type.h"
#include "debug.h"

#include "PrjCfg_Comm.h"
#if (_MODEL_DSC_ == _MODEL_DEMO1_)
#include "PrjCfg_Demo.h"
#elif (_MODEL_DSC_ == _MODEL_EVB_)
#include "PrjCfg_EVB.h"
#elif (_MODEL_DSC_ == _MODEL_CARDV_)
#include "PrjCfg_CARDV.h"
#elif (_MODEL_DSC_ == _MODEL_PW8402_)
#include "PrjCfg_PW8402.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_F6PLUS_)
#include "PrjCfg_F6PLUS.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_JIACHANGLING_)
#include "PrjCfg_JiaChangLing.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_GEMEIDI_)
#include "PrjCfg_Gemeidi.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_BMWCABLE_)
#include "PrjCfg_BMWCable.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_V18_DT_)
#include "PrjCfg_V18DT.h"
#elif (_MODEL_DSC_ == _MODEL_CARDV_B50_)
#include "PrjCfg_B50.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_HLYD_)
#include "PrjCfg_HLYD.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_HLYD_MUBEN_)
#include "PrjCfg_HLYD_Muben.h"
#elif (_MODEL_DSC_ == _MODEL_CARDV_CHINA_ELECTRONICS_)
#include "PrjCfg_China_Electronics.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_G2800_)
#include "PrjCfg_G2800.h"
#elif (_MODEL_DSC_ == _MODEL_DUAL_F5_)
#include "PrjCfg_F5.h"
#elif (_MODEL_DSC_ == _MODEL_CARDV_S650_)
#include "PrjCfg_S650.h"
#elif(_MODEL_DSC_ ==_MODEL_DUAL_YITU_)
#include "PrjCfg_Yitu.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_NAZHIDA_)
#include "PrjCfg_Nazhida.h"
#elif(_MODEL_DSC_==_MODEL_CARDV_GT200_)
#include "PrjCfg_GT200.h"
#elif(_MODEL_DSC_==_MODEL_KERNAL_BOARD_880_)
#include "PrjCfg_KernalBoard_880.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_QIZHENG_M1_)
#include "PrjCfg_QizhengM1.h"
#elif(_MODEL_DSC_==_MODEL_K3_)
#include "PrjCfg_K3.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_XWH_)
#include "PrjCfg_XWH.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_ZHUOHONGCHUANG_)
#include "PrjCfg_ZhuoHongChuang.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_790S_)
#include "PrjCfg_790S.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_V18_LYSZ_)
#include "PrjCfg_V18_LYSZ.h"
#elif(_MODEL_DSC_==_MODEL_CARDV_A8_)
#include "PrjCfg_A8.h"
#elif(_MODEL_DSC_==_MODEL_CARDV_A10_)
#include "PrjCfg_A10.h"
//#elif(_MODEL_DSC_==_MODEL_DUAL_V18_GMD_)
//#include "PrjCfg_V18_GMD.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_89M3_)
#include "PrjCfg_89M3.h"
#elif(_MODEL_DSC_==_MODEL_DUAL_D333_)
#include "PrjCfg_D333.h"
#elif(_MODEL_DSC_ == _MODEL_CARDV_K12_)
#include "PrjCfg_K12.h"
#elif(_MODEL_DSC_ == _MODEL_DUAL_SAIBOSHI_)
#include "PrjCfg_SBS.h"
#elif(_MODEL_DSC_ == _MODEL_DUAL_AONI328_)
#include "PrjCfg_328.h"
#elif(_MODEL_DSC_ == _MODEL_CARDV_A4_)
#include "PrjCfg_A4.h"
#else //Unknown MODEL
#warning Unknown MODEL?
#warning Please assign your "MODEL" in include "ModelConfig.txt"
#error (see above)
#endif

#endif //_PRJCFG_H_
