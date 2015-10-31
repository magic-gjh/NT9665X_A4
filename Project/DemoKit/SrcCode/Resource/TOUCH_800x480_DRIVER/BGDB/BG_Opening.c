#include "BG_Images.h"
#if (_MODEL_DSC_ == _MODEL_CARDV_B50_)
//#include "BG_renyixing_on.c"    //任E 行
//#include "BG_renyixing_off.c"

//#include "BG_ruidian_on.c"//任意行内部中性
//#include "BG_ruidian_off.c"

#include "BG_renyixing_eng_on.c" //任意行英文LOGO
#include "BG_renyixing_eng_off.c"
#elif(_MODEL_DSC_ == _MODEL_DUAL_G2800_)
#include "BG_zhengfuzhe_on.c" //征服者
#include "BG_zhengfuzhe_off.c"
#elif (_MODEL_DSC_ == _MODEL_DUAL_F5_)	
#include "e_car_open.c" // E 车E拍
#include "e_car_close.c"
#elif(_MODEL_DSC_ ==_MODEL_DUAL_790S_)
#include "BG_xianke_on.c" 
#include "BG_xianke_off.c"
#else
#include "welcome.c" 
#include "Goodbye.c"

//#include "xianzhi_open.c"
//#include "xianzhi_close.c"



//#include "F5_open.c" // yellow car "welcome"
//#include "F5_close.c"
#endif
int Logo_getBGOpening_size(void)
{
    return sizeof(g_ucBGOpening);
}

int Logo_getBGGoodbye_size(void)
{
    return sizeof(g_ucBGGoodbye);
}
