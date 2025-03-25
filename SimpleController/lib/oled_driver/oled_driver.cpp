#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "oled_driver.h"
#include "key_driver.h"
#include "mxy_menu.h"
#include "BT_Comm.h"
#include "mpu6050_driver.h"

// 定义电源电压引脚
#define VOLTAGE_PIN 15

//IIC实例
TwoWire OLED_IIC = TwoWire(0);
// 构造对象 连接到I2C(SDA、SCL引脚)的SSD1306声明
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &OLED_IIC, OLED_RESET);

void OLED_init(void)
{
    //IIC引脚配置
    OLED_IIC.begin(25,26,400000UL);

    // OLED初始化
    if (!OLED.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // 如果初始化失败，则无限循环在此处
    }
    Serial.println(F("SSD1306 allocation init"));
    // 清除显示缓冲区
    OLED.clearDisplay();
}

void OLEDprintf(int yPos, const char* format, ...)
{
  char buffer[32]; // 根据你的需要调整缓冲区大小
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  OLED.setCursor(0, yPos);
  OLED.print(buffer);
}


int BT_Flag = 0;

void OLED_display()
{
    // 清除显示缓冲区
  OLED.clearDisplay();
  // 设置文本大小和颜色
  OLED.setTextSize(1); // 设置文本大小
  OLED.setTextColor(SSD1306_WHITE); // 设置字体颜色为白色

// // 读取电源电压
  int sensorValue = analogRead(VOLTAGE_PIN);
  float voltage = sensorValue * (3.3 / 4095.0) * 2; // 假设最大模拟输入电压为3.3V
  voltage = voltage / 0.93; // 修正7%的误差
  if(voltage>=3.75)
    OLEDprintf(0,"                 %.2f",voltage);
  else
  {
    OLEDprintf(0,"              warning");
  }
  

  pMenu->display();   //菜单显示
  pMenu->action();    //菜单操作;

  //只在主页显示蓝牙连接状态
  if(strcmp(pMenu->title,"root") == 0)
  {
    if(BT_Flag) OLEDprintf(0,"         BT:Y");
    else OLEDprintf(0,"         BT:N");
  }
  // OLEDprintf(5*10,"KEY: %d" ,get_key_state(&keyVal));
    // 刷新屏幕
  OLED.display();
}

void MPU6050_InitShow(void)
{
      //显示状态信息
      OLED.clearDisplay();
      // 设置文本大小和颜色
      OLED.setTextSize(1); // 设置文本大小
      OLED.setTextColor(SSD1306_WHITE); // 设置字体颜色为白色
      OLEDprintf(1*10,"OLED OK!");
      OLEDprintf(3*10,"MPU6050 initial...");
      // 刷新屏幕
      OLED.display();
}
void BlueTooth_InitShow(void)
{
      //显示状态信息
      OLED.clearDisplay();
      // 设置文本大小和颜色
      OLED.setTextSize(1); // 设置文本大小
      OLED.setTextColor(SSD1306_WHITE); // 设置字体颜色为白色
      OLEDprintf(1*10,"MPU6050 OK!");
      OLEDprintf(3*10,"BT Connecting...");
      // 刷新屏幕
      OLED.display();
}
