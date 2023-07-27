/*
 * @Author: cnbluesky cnbluesky@qq.com
 * @Date: 2023-04-27 23:14:28
 * @LastEditors: cnbluesky cnbluesky@qq.com
 * @LastEditTime: 2023-05-15 11:09:56
 * @FilePath: \230427-225643-esp32-s3-wroom-1-guoxinghua\lib\MyLed\MyLed.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MyLed_h
#define MyLed_h
#include "FastLED.h"

#define NUM_LEDS 6  // LED灯珠数量
#define DATA_PIN 18 // Arduino输出控制信号引脚

void led_init();
void led_setup();

void LED_WiFi_Connected();
void LED_MQTT_Connected();
void LED_MQTT_PubState();
void LED_Opening();
void LED_Waring();
void LED_Clear();
void led_flash(const struct CRGB &color, uint8_t times, uint16_t period_ms = 200);

extern CRGB leds[];

typedef enum ENUM_EMOTION
{
    LISIENNING = 0,
    SAY = 1,
    THINKING = 2,
    SLEEP = 3,
    WAKEUP = 4

} Emotion_t;

void LED_Emotion(Emotion_t emotion);
extern EventGroupHandle_t xEventGroupLEDControl;
extern Emotion_t EMOTION;

#define ROBOT_STARTUP BIT0
#define ROBOT_ACTIVE BIT1
#define ROBOT_LISTEN BIT2
#define ROBOT_SAY BIT3
#define ROBOT_IDLE BIT4
#define ROBOT_THINKING BIT5
#define ROBOT_ERROR BIT9

#endif