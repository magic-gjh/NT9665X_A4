#include "BG_Images.h"
#if (_MODEL_DSC_ == _MODEL_CARDV_B50_)
//#include "BG_renyixing_on.c"    //��E ��
//#include "BG_renyixing_off.c"

//#include "BG_ruidian_on.c"//�������ڲ�����
//#include "BG_ruidian_off.c"

//#include "BG_renyixing_eng_on.c" //������Ӣ��LOGO
//#include "BG_renyixing_eng_off.c"

//#include "BG_provision_on.c" //������Pro vison 
//#include "BG_provision_off.c"

#include "BG_blueskysea_on.c" //blue sky sea 
#include "BG_blueskysea_off.c" 

#elif(_MODEL_DSC_ == _MODEL_CARDV_CHINA_ELECTRONICS_)
#include "welcome.c"
#include "Goodbye.c"
#elif(_MODEL_DSC_ == _MODEL_DUAL_G2800_)
#include "BG_zhengfuzhe_on.c" //������
#include "BG_zhengfuzhe_off.c"
#elif(_MODEL_DSC_ ==_MODEL_CARDV_S650_)
#include "BG_xianke_on.c" 
#include "BG_xianke_off.c"
#elif(_MODEL_DSC_ ==_MODEL_CARDV_GT200_)
#include "welcome.c"
#include "Goodbye.c"
#else
#include "welcome.c"
#include "Goodbye.c"

//#include "xianzhi_open.c"
//#include "xianzhi_close.c"

//#include "e_car_open.c" // E ��E��
//#include "e_car_close.c"

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
