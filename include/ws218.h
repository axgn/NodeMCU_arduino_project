#include <FastLED.h>

#define LED_PIN D1
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define NUM_LEDS 120

#define COOLING 55
#define SPARKING 120

#define BRIGHTNESS 128
#define FRAMES_PER_SECOND 60

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

void fastledbegin()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void Fire2012()
{
  // Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++)
  {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--)
  {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING)
  {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < NUM_LEDS; j++)
  {
    CRGB color = HeatColor(heat[j]);
    int pixelnumber;
    if (gReverseDirection)
    {
      pixelnumber = (NUM_LEDS - 1) - j;
    }
    else
    {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

void fastledrun()
{
  Fire2012();     // run simulation frame
  FastLED.show(); // display this frame
  // FastLED.delay(1000 / FRAMES_PER_SECOND);
}

uint8_t pos = 0;
bool toggle = false;

void blurledbegin()
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void blurledrun()
{
  // Add a bright pixel that moves
  leds[pos] = CHSV(pos * 5, 255, 255);
  // Blur the entire strip
  blur1d(leds, NUM_LEDS, 255);
  fadeToBlackBy(leds, NUM_LEDS, 16);
  FastLED.show();
  // Move the position of the dot
  if (toggle)
  {

    pos = (pos + 1) % NUM_LEDS;
  }
  toggle = !toggle;
  delay(5);
}

// LED 配置
#define BRIGHTNESS 200
#define SPEED 20

uint8_t hue = 0;                // 色相值（0-255）
uint8_t pulseValue = 0;         // 呼吸亮度值
uint8_t pulseValue = 0;         // 当前亮度值
CRGB targetColor = CRGB::White; // 默认颜色
bool newColorReceived = false;  // 新颜色标志

// 呼吸灯函数
void breathingledbegin()
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void breathingledloop()
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  // 非阻塞定时器（每30ms更新一次）
  if (currentMillis - previousMillis >= 30)
  {
    previousMillis = currentMillis;

    // 彩虹色相渐变（每次+1）
    hue += 1;

    // 呼吸亮度计算（正弦波）
    pulseValue = beatsin8(SPEED, 50, 255); // 最小值50避免全黑

    // 为所有LED设置颜色
    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, pulseValue));

    FastLED.show();
  }

  // 此处可添加其他任务（WiFi/MQTT等）
  // yield(); // 如果启用WiFi需保持yield调用
}
