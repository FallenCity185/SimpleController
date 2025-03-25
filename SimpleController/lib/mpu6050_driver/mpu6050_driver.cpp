#include <Arduino.h>
#include <stdio.h>
#include <MPU6050_tockn.h>
#include "Wire.h"
#include "mpu6050_driver.h"

TwoWire I2Ctwo = TwoWire(1);
// MPU6050实例
MPU6050 mpu6050(I2Ctwo);

void MPU_Init(void)
{
    I2Ctwo.begin(IIC_SDA, IIC_SCL, 400000UL);
    // mpu6050设置
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);
}

// yaw轴角度累加函数
// YAW轴控制数据
float YAW_gyro = 0;
float YAW_angle = 0;
float YAW_angle_last = 0;
float YAW_angle_total = 0;
float YAW_angle_zero_point = -10;
// float YAW_output = 0;
void yaw_angle_addup()
{
  YAW_angle = (float)mpu6050.getAngleZ();
  YAW_gyro = (float)mpu6050.getGyroZ();

  if (YAW_angle_zero_point == (-10))
  {
    YAW_angle_zero_point = YAW_angle;
  }

  float yaw_angle_1, yaw_angle_2, yaw_addup_angle;
  if (YAW_angle > YAW_angle_last)
  {
    yaw_angle_1 = YAW_angle - YAW_angle_last;
    yaw_angle_2 = YAW_angle - YAW_angle_last - 2 * PI;
  }
  else
  {
    yaw_angle_1 = YAW_angle - YAW_angle_last;
    yaw_angle_2 = YAW_angle - YAW_angle_last + 2 * PI;
  }

  if (abs(yaw_angle_1) > abs(yaw_angle_2))
  {
    yaw_addup_angle = yaw_angle_2;
  }
  else
  {
    yaw_addup_angle = yaw_angle_1;
  }

  YAW_angle_total = YAW_angle_total + yaw_addup_angle;
  YAW_angle_last = YAW_angle;
}

