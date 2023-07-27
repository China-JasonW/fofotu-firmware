#include <MyLed.h>
#include <MyLog.h>

#define LED_TYPE WS2812B // LED灯带型号

#define COLOR_ORDER GRB // RGB灯珠中红色、绿色、蓝色LED的排列顺序

uint8_t max_bright = 255; // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高

CRGB leds[NUM_LEDS]; // 建立灯带leds

uint16_t brightness[NUM_LEDS];

EventGroupHandle_t xEventGroupLEDControl = xEventGroupCreate();

Emotion_t EMOTION = Emotion_t::SLEEP;

void setEmotion(Emotion_t e_now)
{
    EMOTION = e_now;
}

extern void play_mp3(String filename);

void taskLedEventControl(void *pvParameters)
{
    const EventBits_t xBitsToWaitFor = ROBOT_STARTUP | ROBOT_ACTIVE | ROBOT_LISTEN | ROBOT_THINKING | ROBOT_SAY | ROBOT_IDLE | ROBOT_ERROR;
    EventBits_t uxBits;
    while (true)
    {
        uxBits = xEventGroupWaitBits(xEventGroupLEDControl,
                                     xBitsToWaitFor,
                                     pdTRUE,
                                     pdFALSE,
                                     portMAX_DELAY);
        if ((uxBits & (ROBOT_STARTUP)) != 0)
        {
            Log.infoln("[EVENT] ROBOT_STARTUP");
            setEmotion(Emotion_t::WAKEUP);
        }
        if ((uxBits & (ROBOT_ACTIVE)) != 0)
        {
            Log.infoln("[EVENT] ROBOT_ACTIVE");
            setEmotion(Emotion_t::WAKEUP);
        }
        if ((uxBits & (ROBOT_LISTEN)) != 0)
        {
            Log.infoln("[EVENT] ROBOT_LISTEN");
            setEmotion(Emotion_t::LISIENNING);
        }
        if ((uxBits & (ROBOT_THINKING)) != 0)
        {
            Log.infoln("[EVENT] ROBOT_THINKING");
            setEmotion(Emotion_t::THINKING);
        }
        if ((uxBits & (ROBOT_SAY)) != 0)
        {
            Log.infoln("[EVENT] ROBOT_SAY");
            setEmotion(Emotion_t::SAY);
        }
        if ((uxBits & (ROBOT_IDLE)) != 0)
        {
            Log.infoln("[EVENT] ROBOT_IDLE");
            setEmotion(Emotion_t::SLEEP);
        }
        if ((uxBits & (ROBOT_ERROR)) != 0)
        {
            Log.infoln("[EVENT] ROBOT_ERROR");
        }
    }
    vTaskDelete(NULL);
}

void LED_Emotion(Emotion_t emotion)
{
    switch (emotion)
    {
    case Emotion_t::LISIENNING:
    {

        fill_solid(leds, NUM_LEDS, CRGB::Green);
        FastLED.show();
        break;
    }
    case Emotion_t::SLEEP:
    {
        LED_Clear();
        break;
    }
    default:
        break;
    }
}

void LED_Clear()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void taskLedControl(void *pvPara)
{
    uint8_t tempIndex = 0;
    uint8_t hue = 0;
    while (true)
    {
        switch (EMOTION)
        {
        case Emotion_t::WAKEUP:
        {
            fill_solid(leds, NUM_LEDS, CRGB::Orange);
            FastLED.show();
            break;
        }
        case Emotion_t::LISIENNING:
        {
            fill_solid(leds, NUM_LEDS, CRGB::Red);
            FastLED.show();
            break;
        }
        case Emotion_t::SAY:
        {
            // 彩虹变化（可以用在火火兔休息的情况下）
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CHSV(hue + (i * 10), 255, 255);
            }
            EVERY_N_MILLISECONDS(15)
            {
                hue++;
            }
            FastLED.show();
            break;
        }
        case Emotion_t::THINKING:
        {
            fill_solid(leds, NUM_LEDS, CRGB::Blue);
            FastLED.show();
            break;
        }

        case Emotion_t::SLEEP:
        {
            LED_Clear();
            break;
        }
        default:
            break;
        }

        // fill_solid(leds, NUM_LEDS, CRGB::White);

        // EVERY_N_SECONDS(1)
        // {
        //     tempIndex++;
        //     if (tempIndex > 8)
        //         tempIndex = 0;
        // }

        // switch (tempIndex)
        // {
        // case 0:
        //     FastLED.setTemperature(Candle);
        //     break;
        // case 1:
        //     FastLED.setTemperature(Tungsten40W);
        //     break;
        // case 2:
        //     FastLED.setTemperature(Tungsten100W);
        //     break;
        // case 3:
        //     FastLED.setTemperature(Halogen);
        //     break;
        // case 4:
        //     FastLED.setTemperature(CarbonArc);
        //     break;
        // case 5:
        //     FastLED.setTemperature(HighNoonSun);
        //     break;
        // case 6:
        //     FastLED.setTemperature(DirectSunlight);
        //     break;
        // case 7:
        //     FastLED.setTemperature(OvercastSky);
        //     break;
        // case 8:
        //     FastLED.setTemperature(ClearBlueSky);
        //     break;
        // }

        // FastLED.show();

        // vTaskDelay(500 / portTICK_PERIOD_MS);

        // 彩虹变化（可以用在火火兔休息的情况下）
        // for (int i = 0; i < NUM_LEDS; i++)
        // {
        //     // leds[i] = CHSV(hue, 255, 255);
        //     leds[i] = CHSV(hue + (i * 10), 255, 255);
        // }

        // EVERY_N_MILLISECONDS(15)
        // {
        //     hue++;
        // }

        // FastLED.show();

        // EVERY_N_MILLISECONDS(50)
        // {

        //     // Create a new HSV color for led[0]
        //     leds[0] = CHSV(160, random8(), random8(100, 255));

        //     // Copy each pixel to the next one, starting at the far end
        //     // thereby 'moving' the pattern along the strip
        //     for (int i = NUM_LEDS - 1; i > 0; i--)
        //     {
        //         leds[i] = leds[i - 1];
        //     }
        // }

        // FastLED.show();

        // Read the potentiometer and set brightness
        // uint16_t potRead = analogRead(8);
        // uint8_t brightness = map(potRead, 0, 1023, 0, 200);
        // FastLED.setBrightness(brightness);

        // // Create a new HSV color for led[0]
        // leds[0] = CHSV(160, random8(), random8(100, 255));

        // EVERY_N_MILLISECONDS(30)
        // {
        //     // Copy each pixel to the next one, starting at the far end
        //     // thereby 'moving' the pattern along the strip
        //     for (int i = NUM_LEDS - 1; i > 0; i--)
        //     {
        //         leds[i] = leds[i - 1];
        //     }
        // }

        // FastLED.show();

        // // Waves for LED position
        // uint8_t posBeat = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);
        // uint8_t posBeat2 = beatsin8(60, 0, NUM_LEDS - 1, 0, 0);
        // uint8_t posBeat3 = beatsin16(30, 0, NUM_LEDS - 1, 0, 127);
        // uint8_t posBeat4 = beatsin16(60, 0, NUM_LEDS - 1, 0, 127);

        // // In the video I use beatsin8 for the positions. For longer strips,
        // // the resolution isn't high enough for position and can lead to some
        // // LEDs not lighting. If this is the case, use the 16 bit versions below
        // // uint16_t posBeat  = beatsin16(30, 0, NUM_LEDS - 1, 0, 0);
        // // uint16_t posBeat2 = beatsin16(60, 0, NUM_LEDS - 1, 0, 0);
        // // uint16_t posBeat3 = beatsin16(30, 0, NUM_LEDS - 1, 0, 32767);
        // // uint16_t posBeat4 = beatsin16(60, 0, NUM_LEDS - 1, 0, 32767);

        // // Wave for LED color
        // uint8_t colBeat = beatsin8(45, 0, 255, 0, 0);

        // leds[(posBeat + posBeat2) / 2] = CHSV(colBeat, 255, 255);
        // leds[(posBeat3 + posBeat4) / 2] = CHSV(colBeat, 255, 255);

        // fadeToBlackBy(leds, NUM_LEDS, 10);
        // FastLED.show();
    }
    vTaskDelete(NULL);
}

void led_setup()
{
    // 初始化灯带
    LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    // 设置最大亮度
    FastLED.setBrightness(max_bright);

    // LED初始化，1024堆栈溢出
    xTaskCreatePinnedToCore(taskLedControl, "taskLedControl", 1024 * 4, NULL, 3, NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(taskLedEventControl, "taskLedEventControl", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
}
