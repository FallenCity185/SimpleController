#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H

#include <MPU6050_tockn.h>

#define IIC_SDA 21
#define IIC_SCL 22

void MPU_Init(void);


extern MPU6050 mpu6050;

#endif
