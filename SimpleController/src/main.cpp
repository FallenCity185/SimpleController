#include <Arduino.h>
#include <string.h>

#include "oled_driver.h"
#include "key_driver.h"
#include "mxy_menu.h"
#include "BT_Comm.h"
#include "mpu6050_driver.h"

void setup() {
  Serial.begin(115200);
  menuInit();
  key_init();
  OLED_init();
  MPU6050_InitShow();
  MPU_Init();
  BlueTooth_InitShow();
  if(IsCheckBT()) //可退出蓝牙连接，调试用
    BT_init();
}

void loop() {
  
  // 处理来自轮足的数据
  handleIncomingData();
  // 定期发送状态请求
  if(strcmp(pMenu->title,"paramSet") == 0) {
    sendCMD = CMD_PARAM;
    timeCicle = 200; // 每200ms发送一次命令
  }
  else if(strcmp(pMenu->title,"control") == 0){
    sendCMD = CMD_CONTROL;
    timeCicle = 200;
  }
  else if(strcmp(pMenu->title,"posCtl") == 0) {
    sendCMD = CMD_STATUS;
    timeCicle = 50;
  }
  else if(strcmp(pMenu->title,"car_state") == 0) {
    sendCMD = CMD_CAR_STATE;
    timeCicle = 50;
  }
  else sendCMD = 0;
  
  static unsigned long lastTime = 0;
  if (millis() - lastTime > timeCicle && sendCMD != 0) {
    switch (sendCMD)
    {
      case CMD_PARAM:
        if(paramCMD_T.changeFlag == 1)
        {
          paramCMD_Set(&paramCMD_T,&my_param);
          sendPacket(sendCMD, &paramCMD_T, sizeof(paramCMD_T));
          paramCMD_T.changeFlag = 0;
        }
        break;
      case CMD_CONTROL:
        sendPacket(sendCMD, &controlCMD_T, sizeof(controlCMD_T));
        break;
      case CMD_STATUS:
        sendPacket(sendCMD, &statureCMD_T, sizeof(statureCMD_T));
        break;
      case CMD_CAR_STATE:
        sendPacket(sendCMD, 0, 0);  //发送请求指令
      break;
      default:
        break;
    }
    lastTime = millis();
  }
  
  mpu6050.update();
  OLED_display();

}
