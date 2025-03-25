#ifndef BT_COMM_H
#define BT_COMM_H

#include <stdio.h>
#include "mxy_paramSys.h"
//命令字
//发送
#define CMD_PARAM   0x10        //参数设置    无需回复（状态中回复）
#define CMD_STATUS  0x11        //姿态陀螺仪控制
#define CMD_CONTROL 0x12        //控制模式
//接收
#define CMD_CAR_STATE 0x20        //小车状态

// 控制指令结构体定义
//发送
typedef struct {
  uint8_t key;  //键值
} ControlStruct;

typedef struct {
  float row;
  float pitch;
  float yaw;
} StatureStruct;

typedef struct {
  uint8_t changeFlag;
  float pos_p;
  float pos_d;
  float vel_p;
  float vel_i;
} ParamStruct;

//接收
typedef struct {
  float row;
  float pitch;
  float yaw;
  float speed;
} CarStateStruct;

void BT_init(void);
void sendPacket(uint8_t commandType, const void* payload, uint8_t length);
void handleIncomingData(void);
void processFrame(uint8_t commandType, uint8_t *frame, uint8_t length);

extern uint8_t sendCMD;    //发送指令
extern uint8_t timeCicle;//发送周期 ms

extern ControlStruct controlCMD_T;
extern StatureStruct statureCMD_T;
extern ParamStruct paramCMD_T;

void paramCMD_Set(ParamStruct *paramCMD_T, mxy_paramType *my_param);
#endif
