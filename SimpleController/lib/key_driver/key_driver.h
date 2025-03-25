#ifndef KEY_DRIVER_H
#define KEY_DRIVER_H

/*----操作定义----*/
//前--y +
//左--x +

//按键定义
#define KEY_MODE  27  //模式按键引脚
#define KEY_STICK 35  //摇杆按键引脚

//摇杆定义
#define STICK_X 32      //X轴
#define STICK_Y 33      //Y轴

#define X_THRESHOLD 10     //X轴触发阈值
#define Y_THRESHOLD 10     //Y轴触发阈值

typedef struct KeyStruct{
        int key_mode;   //模式按键状态
        int key_stick;  //摇杆按键状态
        int stick_x;    //摇杆X轴状态
        int stick_y;    //摇杆Y轴状态
        int center_x;   //x轴中值   18-20
        int center_y;   //y轴中值   18-20
} KeyType;

enum KeyEnum    //4种按键状态存在的排列组合
{
    //基本状态
    space,  //空状态
    key_m,  //仅模式按键
    key_s,  //仅摇杆按键
    s_x_l,  //仅摇杆x轴左触发
    s_x_r,  //仅摇杆x轴右触发
    s_y_l,  //仅摇杆y轴左触发
    s_y_r,  //仅摇杆y轴右触发

    //模式按键组合
    m_x_l,  //模式按键+摇杆X左触发
    m_x_r,  //模式按键+摇杆X右触发
    m_y_l,  //模式按键+摇杆y左触发
    m_y_r,  //模式按键+摇杆y右触发
    m_s,    //模式按键+摇杆按键

    //摇杆组合  l为近0端    r为近4095端
    xl_yl,  //摇杆左上
    xl_yr,  //摇杆右上
    xr_yl,  //摇杆左下
    xr_yr,  //摇杆右下
};


void key_init(void);
void key_scan(KeyType* keyVal);
int get_key_state(KeyType* keyVal);

int IsCheckBT(void);

extern KeyType keyVal;

#endif
