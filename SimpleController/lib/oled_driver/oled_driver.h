#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H



#define SCREEN_WIDTH      128   // OLED display width, in pixels
#define SCREEN_HEIGHT     64    // OLED display height, in pixels
#define OLED_RESET        -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS    0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

void OLED_init(void);
void OLEDprintf(int yPos, const char* format, ...);
void OLED_display();

void MPU6050_InitShow(void);
void BlueTooth_InitShow(void);

extern int BT_Flag;

#endif
