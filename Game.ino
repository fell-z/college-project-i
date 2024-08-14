/// @author fell-z  matheusfelipeagapito@hotmail.com
// # FastLED related
#include <FastLED.h>

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    14

CRGB leds[NUM_LEDS];

#define BRIGHTNESS        20
#define FRAMES_PER_SECOND 120

// # Game logic related
uint8_t last_target_pos = -1;

#define NUM_TARGETS 9
#define TARGET_LED(index) index + 5 // index for CRGB array
const uint8_t targets[NUM_TARGETS] = { 4, 5, 6, 7, 8, 9, 15, 14, 16 };

void setup()
{
  delay(2000);

  Serial.begin(9600);

  for (int i = 0; i < NUM_TARGETS; i++)
    pinMode(targets[i], INPUT_PULLUP);

  randomSeed(analogRead(0));

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  random_target();
}

const uint8_t led_bar_nLeds = 5;
uint8_t g_hue = 0;
uint8_t g_delta_hue = 7;

bool switch_target = false;
int8_t matched_target_index;

void loop()
{
  fill_rainbow(leds, led_bar_nLeds, g_hue, g_delta_hue);
  
  game();

  /* for debugging/testing purposes

  Serial.print("Last target pos: ");
  Serial.println(last_target_pos);
  Serial.print("Target pin: ");
  Serial.println(targets[last_target_pos]);

  */

  FastLED.delay(1000/FRAMES_PER_SECOND);
  FastLED.show();

  EVERY_N_MILLISECONDS(20) { g_hue++; }
}

/// game loop
void game()
{
  matched_target_index = check_targets();

  if (matched_target_index == last_target_pos)
    switch_target = true;

  if (switch_target)
  {
    leds[TARGET_LED(last_target_pos)] = CRGB::Black;

    random_target();
    switch_target = false;
  }

  rainbow_by_index(leds, TARGET_LED(last_target_pos), (g_hue * 2.78));
}

/// Fill an unique led, selected by index, with rainbow colors
void rainbow_by_index(struct CRGB *targetArray, uint8_t index, uint8_t initialHue)
{
  CHSV hsv = { .hue = initialHue, .sat = 240, .val = 255 };
  leds[index] = hsv;
}

/// Check every target for a LOW input.
/// @return an index between 0 and 8 for the matched target
int8_t check_targets()
{
  int8_t index;
  uint8_t value;

  for (index = 0; index < NUM_TARGETS; index++)
  {
    value = digitalRead(targets[index]);
    if (value == LOW)
      break;
  }

  if (index > 8)
    return -1;

  return index;
}

/// Overwrites the last_target_pos variable with
/// an random index between 0 and 8
void random_target()
{
  uint8_t tmp = random(0, 9);
  while (tmp == last_target_pos)
  {
    tmp = random(0, 9);
  }

  last_target_pos = tmp;
}
