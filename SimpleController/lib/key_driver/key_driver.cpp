#include "key_driver.h"
#include <Arduino.h>

KeyType keyVal;

void key_init()
{
//按键引脚初始化为输入模式
  pinMode(KEY_MODE, INPUT_PULLUP); //模式按键
  pinMode(KEY_STICK, INPUT_PULLUP); //摇杆按键
  key_scan(&keyVal);
  keyVal.center_x = keyVal.stick_x;
  keyVal.center_y = keyVal.stick_y;
  Serial.println(F("key init finish"));
}


void key_scan(KeyType* keyVal)
{
    keyVal->key_mode = digitalRead(KEY_MODE); // 读取按键状态
    keyVal->key_stick = digitalRead(KEY_STICK); // 读取按键状态
    keyVal->stick_x = analogRead(STICK_X)/100;
    keyVal->stick_y = analogRead(STICK_Y)/100;
}

int get_key_state(KeyType* keyVal)
{
    KeyEnum state = space;
    int k_m = (keyVal->key_mode == HIGH)?1:0;
    int k_s = (keyVal->key_stick == HIGH)?1:0;
    int s_x = (keyVal->stick_x < keyVal->center_x - X_THRESHOLD)?1:
                (keyVal->stick_x > keyVal->center_x + X_THRESHOLD?2:0);    //0：居中   1：偏左     2：偏右
    int s_y = (keyVal->stick_y < keyVal->center_y - Y_THRESHOLD)?1:
                (keyVal->stick_y > keyVal->center_y + Y_THRESHOLD?2:0);    //0：居中   1：偏左     2：偏右
    //单操作检测
    if(k_m && !k_s && !s_x && !s_y)
        state = key_m;
    else if(!k_m && k_s && !s_x && !s_y)
        state = key_s;
    else if(!k_m && !k_s && s_x==1 && !s_y)
        state = s_x_r;
    else if(!k_m && !k_s && s_x==2 && !s_y)
        state = s_x_l;
    else if(!k_m && !k_s && !s_x && s_y==1)
        state = s_y_l;
    else if(!k_m && !k_s && !s_x && s_y==2)
        state = s_y_r;
    //模式按键组合
    else if(k_m && !k_s && s_x==1 && !s_y)
        state = m_x_l;
    else if(k_m && !k_s && s_x==2 && !s_y)
        state = m_x_r;
    else if(k_m && !k_s && !s_x && s_y==1)
        state = m_y_l;
    else if(k_m && !k_s && !s_x && s_y==2)
        state = m_y_r;
    //摇杆组合
    else if(!k_m && !k_s && s_x==1 && s_y==1)
        state = xl_yl;
    else if(!k_m && !k_s && s_x==1 && s_y==2)
        state = xl_yr;
    else if(!k_m && !k_s && s_x==2 && s_y==1)
        state = xr_yl;
    else if(!k_m && !k_s && s_x==2 && s_y==2)
        state = xr_yr;
    //双按键
    else if(k_s && k_m)
        state = m_s;
    return state;
}

int IsCheckBT(void)
{
    key_scan(&keyVal);
    if(key_m == get_key_state(&keyVal))
        return 1;
    return 0;
}
