#include "BT_Comm.h"
#include <BluetoothSerial.h>

#include "oled_driver.h"
#include "mxy_paramSys.h"

//蓝牙配置
BluetoothSerial SerialBT;
const char *pin = "1234"; // 蓝牙匹配代码，根据自己需要更改
String slaveName = "JDY-31-SPP"; // 蓝牙从机名称
String myName = "ESP32-BT-Master"; // 蓝牙主机名称


// 用于存储最新发送的信息
ControlStruct controlCMD_T;        
StatureStruct statureCMD_T;
ParamStruct paramCMD_T;
//存储接收的信息   
CarStateStruct carStateCMD_R;

//解析接收数据函数
void executeCarStateCommand(CarStateStruct *carState);

void BT_init(void)
{
    //蓝牙服务器
    bool connected; // 布尔型变量，只有ture和flase两个值
    SerialBT.begin(myName, true); // 启动蓝牙串口
    Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", myName.c_str());
    SerialBT.setPin(pin); // 设置配对代码
    Serial.println("Using PIN");
    /*
    connect(address) 很快（最多 10 秒）， 
    connect(slaveName) 很慢（最多 30 秒），
    根据需要。首先将 SlaveName 解析为地址，但它允许连接到具有相同名称的不同设备 .
    设置CoreDebugLevel为Info可查看设备蓝牙地址和设备名称
    */
    connected = SerialBT.connect(slaveName);
    Serial.printf("Connecting to slave BT device named \"%s\"\n", slaveName.c_str());
    if(connected) {
        Serial.println("Connected Successfully!");
        BT_Flag = 1;
    } else {
        BT_Flag = 0;
    }

    //设置参数变更标志
    paramCMD_T.changeFlag = 0;
}
/*----------------------------------通信协议-----------------------------------------*/
/**
 * 起始帧
 * 命令字   8位
 * 数据长度 8位 （字节个数  只包含数据部分）
 * 数据 （多字节拆解为单字节发送）
 * ...
 * 数据
 * 校验码   8位 （包含命令字和数据长度）
 * 结束帧   8位 （发送结束帧后从机才开始解析）
 */
uint8_t sendCMD = 0;    //发送指令
uint8_t timeCicle = 100;//发送周期 ms

#define START_BYTE 0xA5
#define END_BYTE 0x5A
#define MAX_FRAME_LENGTH 64
uint8_t rxBuffer[MAX_FRAME_LENGTH]; // 接收缓冲区
uint8_t rxIndex = 0;                // 当前接收索引
bool frameStart = false;            // 帧开始标志
uint8_t frameLength = 0;            // 帧长度
uint8_t checksum = 0;               // 校验和

//参数赋值
void paramCMD_Set(ParamStruct *paramCMD_T, mxy_paramType *my_param)
{
  paramCMD_T->pos_p = my_param->p_pos;
  paramCMD_T->pos_d = my_param->d_pos;
  paramCMD_T->vel_p = my_param->p_vel;
  paramCMD_T->vel_i = my_param->i_vel;
}
//用于将本地参数赋值给传输参数


void sendPacket(uint8_t commandType, const void* payload, uint8_t data_length) {
  uint8_t checksum = 0;
  
  // 帧头
  SerialBT.write(START_BYTE);
  
  // 命令字
  SerialBT.write(commandType);
  checksum += commandType;

  // 数据长度（仅DATA部分）
  SerialBT.write(data_length); 
  checksum += data_length;

  // 数据部分
  for (uint8_t i = 0; i < data_length; ++i) {
    uint8_t byteData = ((uint8_t*)payload)[i];
    SerialBT.write(byteData);
    checksum += byteData;
  }

  // 校验和 & 帧尾
  SerialBT.write(checksum & 0xFF);
  SerialBT.write(END_BYTE);
}

void handleIncomingData(void) {
  static bool frameStart = false;
  static uint8_t rxIndex = 0;
  static uint8_t commandType = 0;
  static uint8_t dataLength = 0;
  static uint8_t calculatedChecksum = 0;
  // 定期检查蓝牙连接状态
  // static bool lastConnectionState = false;
  // bool isConnected = SerialBT.connected();
  // if (isConnected != lastConnectionState) { // 如果连接状态发生变化
  //   if (isConnected) {
  //     // Serial.println("蓝牙已连接");
  //     BT_Flag = 1;
  //   } else {
  //     // Serial.println("蓝牙已断开");
  //     BT_Flag = 0;
  //   }
  //   lastConnectionState = isConnected; // 更新上一次的连接状态
  // }
  //蓝牙数据帧读取
  while (SerialBT.available()) {
    uint8_t receivedByte = SerialBT.read();

    if (!frameStart) {
      if (receivedByte == START_BYTE) {
        frameStart = true;
        rxIndex = 0;
        calculatedChecksum = 0;
      }
      continue;
    }

    // START已接收，开始处理数据
    switch (rxIndex) {
      case 0:  // 命令字
        commandType = receivedByte;
        calculatedChecksum += receivedByte;
        rxIndex++;
        break;

      case 1:  // 数据长度
        dataLength = receivedByte;
        calculatedChecksum += receivedByte;
        rxIndex++;
        break;

      default:
        if (rxIndex < dataLength + 2) {  // +2补偿CMD和LEN位置
          // 数据存储（注意缓冲区溢出保护）
          if ((rxIndex - 2) < MAX_FRAME_LENGTH) {
            rxBuffer[rxIndex - 2] = receivedByte;
            calculatedChecksum += receivedByte;
          }
          rxIndex++;
        } else if (rxIndex == dataLength + 2) {
          // 校验和验证
          if ((calculatedChecksum & 0xFF) != receivedByte) {
            Serial.println("Checksum error");
            frameStart = false;
            return;
          }
          rxIndex++;
        } else if (receivedByte == END_BYTE) {
          // 完整帧接收成功
          processFrame(commandType, rxBuffer, dataLength);
          frameStart = false;
        } else {
          // 非法结束符
          Serial.println("END_BYTE error");
          frameStart = false;
        }
        break;
    }
  }
}

// 解析接收到的数据帧
void processFrame(uint8_t commandType, uint8_t *frame, uint8_t length) { 
  switch (commandType) {
    case CMD_PARAM:      // 参数设置
        //此为发送命令 无需接收
        break;

    case CMD_STATUS:     // 状态回传
        //此为发送命令 无需接收
        break;

    case CMD_CONTROL:    // 控制模式
        //此为发送命令 无需接收
        break;
    case CMD_CAR_STATE:    //小车状态反馈
        // 注意：frame此时指向数据区起始位置
        if (length != sizeof(carStateCMD_R)) {
          Serial.println("CMD_PARAM length error");
          return;
        }
        memcpy(&carStateCMD_R, frame, sizeof(carStateCMD_R));
        executeCarStateCommand(&carStateCMD_R);
        break;

    break;
    default:
        Serial.print("Unknown command: 0x");
        Serial.println(commandType, HEX);
        break;
  }
}

void executeCarStateCommand(CarStateStruct *carState){
  my_param.car_pit = carState->pitch;
  my_param.car_row = carState->row;
  my_param.car_yaw = carState->yaw;
  my_param.car_speed = carState->speed;
}