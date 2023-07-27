#include <Arduino.h>
#include <MyButton.h>
#include "MyLog.h"
#include "MyLed.h"

// 机器人按键（位于改造的玩具上）
OneButton ChatButton(GPIO_USER_BUTTON, true, true);

// BOOT按键（位于ESP32开发板上）
OneButton BootButton(GPIO_BOOT_BUTTON, true, true);

extern void play_mp3(String filename);

// 任务初始化（需要加入任务）
void TaskButtonTick(void *pvParameters)
{
    while (true)
    {
        ChatButton.tick();
        BootButton.tick();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void attachLongPress()
{
    Log.infoln("检测到长按按键!");
}

void attachShortPress()
{
    Log.infoln("检测到短按按键!");
}

void attachLongPressStarted()
{
    Log.infoln("检测到长按开始!");
    xEventGroupSetBits(xEventGroupLEDControl, ROBOT_LISTEN);
}

void attachClick()
{
    Log.infoln("检测到点击事件!");
}

void attachLongPressEnd()
{
    Log.infoln("检测到长按结束!，播放提示音");
    play_mp3("notification.mp3");
}

void button_setup()
{
    // 长按时间（ms）
    ChatButton.setPressTicks(500);
    // 短按时间（ms）
    ChatButton.setClickTicks(100);
    BootButton.setPressTicks(500);
    BootButton.setClickTicks(100);
    // 事件绑定
    // ChatButton.attachClick(attachClick);
    // ChatButton.attachLongPressStop(attachLongPress);
    // ChatButton.attachClick(attachShortPress);
    // ChatButton.attachLongPressStart(attachLongPressStarted);
    ChatButton.attachLongPressStop(attachLongPressEnd);
    BootButton.attachLongPressStop(attachLongPressEnd);

    // 按键任务初始化
    // 堆栈1024时，溢出***ERROR*** A stack overflow in task TaskButtonTick has been detected.
    xTaskCreatePinnedToCore(TaskButtonTick, "TaskButtonTick", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
}
