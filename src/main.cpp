#include "Arduino.h"
#include "AudioTools.h"
#include "AudioLibs/Communication.h"
#include "AudioLibs/AudioSourceSPIFFS.h"
#include "AudioCodecs/CodecMP3Helix.h"
#include "MyButton.h"
#include "MyLog.h"
#include "MyLed.h"
#include "config.h"

#define HUOHUOTU_USE_SPEAKER
#define HUOHUOTU_USE_MIC

#define UDP_SERVER_PORT 8085
#define UDP_ESP32_PORT 8085
// ！需要修改为ESP32 IP地址
// IPAddress udpServerAddress(192, 168, 1, 4);

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

#define VOLUME_SAMPLE_PIN 8

// 根据实际连接的引脚进行配置
#define I2S_SPEAKER_BCLK 5 // 时钟信号
#define I2S_SPEAKER_DOUT 6 // 数据输出
#define I2S_SPEAKER_LRC 4  // 声道选择信号

#define I2S_MIC_PORT I2S_NUM_0
#define I2S_SPEAKER_PORT I2S_NUM_1

// 根据实际连接的引脚进行配置
#define I2S_MIC_BCLK 15 // 时钟信号
#define I2S_MIC_DIN 16  // 数据输入
#define I2S_MIC_LRC 7   // 声道选择信号

// 扬声器
AudioInfo speakerInfo(16000, 1, 16);
MP3DecoderHelix decoder;
I2SStream speakerStream;
VolumeStream volumeSpeakerStream(speakerStream);

// UDP 音频流
UDPStream udpSource(ssid, password);
StreamCopy udp2I2sCopier(volumeSpeakerStream, udpSource);
const int udpLocalPort = UDP_ESP32_PORT;

// MP3 音频流
const char *startFilePath = "/";
const char *ext = "mp3";
AudioSourceSPIFFS spiffsSource(startFilePath, ext);
AudioPlayer mp3Player(spiffsSource, volumeSpeakerStream, decoder);

// 麦克风
UDPStream udpMic(ssid, password);
AudioInfo micInfo(16000, 1, 16);
I2SStream micStream;
StreamCopy i2s2UdpCopier(udpMic, micStream);

const int udpServerPort = UDP_SERVER_PORT;

// 音量全（0~1 浮点数）
float volume = 0;

// 播放模式（0：UDP音频流 1:MP3音频流）
int audioPlayMode = 0;

void volume_setup()
{
    pinMode(VOLUME_SAMPLE_PIN, INPUT); // 将引脚设置为输入模式
    analogReadResolution(12);          // 设置ADC分辨率为12位
}

void callbackPrintMetaData(MetaDataType type, const char *str, int len)
{
    Serial.print("==> ");
    Serial.print(toStr(type));
    Serial.print(": ");
    Serial.println(str);
}

void task_mp3_player_loop(void *pvParameters)
{
    while (true)
    {
        // Log.infoln("palyer:%T", player.isActive());
        mp3Player.copy();
    }
    vTaskDelete(NULL);
}

void MySpeaker_setup()
{
    AudioLogger::instance().begin(Serial, AudioLogger::Info);
    Serial.println("UDP server started on port " + String(udpLocalPort));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    udpSource.begin(udpLocalPort, udpServerAddress, udpServerPort);
    // start I2S
    Serial.println("MySpeaker starting I2S...");
    auto config = speakerStream.defaultConfig(TX_MODE);
    config.copyFrom(speakerInfo);
    config.port_no = I2S_SPEAKER_PORT;
    config.pin_bck = I2S_SPEAKER_BCLK;
    config.pin_data = I2S_SPEAKER_DOUT;
    config.pin_ws = I2S_SPEAKER_LRC;
    config.buffer_size = 1024;
    config.buffer_count = 20;
    speakerStream.begin(config);
    volumeSpeakerStream.begin(config);
    volumeSpeakerStream.setVolume(1);
    // mp3Player.setMetadataCallback(callbackPrintMetaData);
    Serial.println("MySpeaker started...");
}

void MyMic_setup()
{
    udpMic.begin(udpLocalPort, udpServerAddress, udpServerPort);
    Serial.println("MyMic starting I2S...");
    auto config_in = micStream.defaultConfig(RX_MODE);
    config_in.copyFrom(micInfo);
    config_in.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    config_in.is_master = true;
    config_in.port_no = I2S_MIC_PORT;
    config_in.pin_bck = I2S_MIC_BCLK;
    config_in.pin_ws = I2S_MIC_LRC;
    config_in.pin_data = I2S_MIC_DIN;
    // 是否设定use_apll均可正常工作
    config_in.use_apll = false;
    micStream.begin(config_in);
    // err = i2s_set_clk(I2S_MIC_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO); // Set the clock and bitwidth used by I2S Rx and Tx.// 设置I2S RX、Tx使用的时钟和位宽
    Serial.println("MyMic I2S started...");
}

void play_mp3(String filename)
{
    // if (mp3Player.isActive())
    // {
    //     Log.infoln("Mp3 is active,stoping...");
    //     mp3Player.end();
    // }
    spiffsSource.setFileFilter(filename.c_str());
    mp3Player.begin();
    // 设置为MP3播放模式
    audioPlayMode = 1;
}

void task_mic_loop(void *pvParameters)
{
    bool nowState = HIGH;
    while (true)
    {
        nowState = digitalRead(GPIO_USER_BUTTON);
        // 录音模式
        if (nowState == LOW)
        {
            xEventGroupSetBits(xEventGroupLEDControl, ROBOT_LISTEN);
            i2s2UdpCopier.copy();
        }
        else
        {
            vTaskDelay(300 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

void task_speaker_loop(void *pvParameters)
{
    // Spkeaker 空闲开始时间
    uint32_t speakerIdleStartTime;
    // Spkeaker 空闲累计时间
    uint32_t speakerIdleElapsedTime;
    while (true)
    {
        // 这里若没有加延时或者打印串口，1s大概可以循环2万次
        // UDP 音频播放模式
        if (audioPlayMode == 0)
        {
            // 缓存数据检查
            if (udp2I2sCopier.available() > 0)
            {
                // 检测到数据时，改变状态
                xEventGroupSetBits(xEventGroupLEDControl, ROBOT_SAY);
                // 忙状态，空闲开始时间归零
                speakerIdleStartTime == 0;
                udp2I2sCopier.copy();
            }
            else
            {
                // 第一次检测到空闲，记录空闲时间
                if (speakerIdleStartTime == 0)
                {
                    speakerIdleStartTime = millis();
                }
                // 再次检测到空闲，记录空闲持续时间
                else
                {
                    speakerIdleElapsedTime = millis() - speakerIdleStartTime;
                    // 连续检测到空闲超过5s时，机器人处于空闲状态
                    if (speakerIdleElapsedTime > 5000)
                    {
                        Log.infoln("More than 5s not received audio frame, enter sleep mode");
                        xEventGroupSetBits(xEventGroupLEDControl, ROBOT_IDLE);
                        speakerIdleStartTime = 0;
                    }
                }
            }
        }
        // MP3音频播放模式
        else
        {
            mp3Player.copy();

            // 若MP3播放完成，则mp3Player.isActive()返回false,，此时需终止MP3音频播放模式
            if (!mp3Player.isActive())
            {
                // 切换至UDP播放模式
                audioPlayMode = 0;
                // 需要重新初始化播放器，否则播放断续（可能是Stram配置问题）
                MySpeaker_setup();
                // 机器人处于空闲状态
                xEventGroupSetBits(xEventGroupLEDControl, ROBOT_IDLE);
            }
        }
    }
    vTaskDelete(NULL);
}

void volume_ctrl_task(void *pvParameters)
{
    int rawValue = 0;
    int lastrawValue = 0;
    while (true)
    {
        // 通过AD读取尾部滑动变阻器阻值
        rawValue = analogRead(VOLUME_SAMPLE_PIN);
        // 转换为音量
        volume = rawValue / 4095.00;
        // 当音量变化超过60时，才设置音量
        if (abs(rawValue - lastrawValue) > 60 || rawValue == 0)
        {
            volumeSpeakerStream.setVolume(volume);
        }
        lastrawValue = rawValue;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void setup()
{
    // 日志初始化
    Serial.begin(115200);

    log_setup();

    // Chat按键初始化
    button_setup();

    // 头部耳灯初始化
    led_setup();

#ifdef HUOHUOTU_USE_SPEAKER
    // 音量调节初始化
    volume_setup();
    // 扬声器初始化
    MySpeaker_setup();
    // 扬声器播放任务
    xTaskCreatePinnedToCore(task_speaker_loop, "task_speaker_loop", 1024 * 8, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    // 扬声器音量控制任务
    xTaskCreatePinnedToCore(volume_ctrl_task, "volume_ctrl_task", 1024 * 4, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
#endif

#ifdef HUOHUOTU_USE_MIC
    // MIC初始化
    MyMic_setup();
    // MIC声音采集任务
    xTaskCreatePinnedToCore(task_mic_loop, "task_mic_loop", 1024 * 4, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
#endif

    // 初始化机器人状态
    xEventGroupSetBits(xEventGroupLEDControl, ROBOT_STARTUP);
}

void loop()
{
    vTaskDelay(10000);
}