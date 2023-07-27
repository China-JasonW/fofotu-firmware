#ifndef MyButton_h
#define MyButton_h
#include <OneButton.h>

// 根据实际连接情况修改
#define GPIO_USER_BUTTON 17

#define GPIO_BOOT_BUTTON 0

extern OneButton ChatButton;

// 按键初始化
void button_setup();

#endif