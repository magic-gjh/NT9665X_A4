
//#include "UIFlow.h"
#include "../../D800x480/UIFlow.h"

#if(_MODEL_DSC_ == _MODEL_DUAL_G2800_)
#include "UICfgDefault_G2800.h"
#elif(_MODEL_DSC_ == _MODEL_DUAL_89M3_)
#include "UICfgDefault_89M3.h"
#elif(_MODEL_DSC_ == _MODEL_DUAL_SAIBOSHI_)
#include "UICfgDefault_SBS.h"
#elif(_MODEL_DSC_ == _MODEL_DUAL_AONI328_)
#include "UICfgDefault_328.h"
#else
#include "UICfgDefault_Demo.h"
#endif

