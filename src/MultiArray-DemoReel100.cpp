#include <Arduino.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN_1 3
#define DATA_PIN_2 5
#define DATA_PIN_3 6
//#define CLK_PIN   4
#define LED_TYPE NEOPIXEL
//#define COLOR_ORDER GRB
#define NUM_STRIPS 3
#define NUM_LEDS 68
CRGB leds[NUM_LEDS];
//CRGB leds_2[NUM_LEDS];
//CRGB leds_3[NUM_LEDS];

#define BRIGHTNESS 100
#define FRAMES_PER_SECOND 120

// List of patterns to cycle through.  Each is defined as a separate function below.
void rainbow();
void rainbowWithGlitter();
void confetti();
void sinelon();
void juggle();
void bpm();
void nextPattern();
void addGlitter(fract8 chanceOfGlitter);

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

void setup()
{
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  //  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // mirror LED setup
  FastLED.addLeds<LED_TYPE, 3>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 5>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 6>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
}

void loop()
{
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();

  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; };   // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(10) { nextPattern(); }; // change patterns periodically
  Serial.begin(9600);
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
  //  fill_rainbow(leds_1, NUM_LEDS, gHue, 7);
  //  fill_rainbow(leds_2, NUM_LEDS, gHue, 7);
  //  fill_rainbow(leds_3, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::White;

    //    leds_1[random16(NUM_LEDS)] += CRGB::White;
    //    leds_2[random16(NUM_LEDS)] += CRGB::White;
    //    leds_3[random16(NUM_LEDS)] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);

  //  fadeToBlackBy(leds_1, NUM_LEDS, 10);
  //  fadeToBlackBy(leds_2, NUM_LEDS, 10);
  //  fadeToBlackBy(leds_3, NUM_LEDS, 10);
  //  int pos_1 = random16(NUM_LEDS);
  //  int pos_2 = random16(NUM_LEDS);
  //  int pos_3 = random16(NUM_LEDS);
  //  leds_1[pos_1] += CHSV(gHue + random8(64), 200, 255);
  //  leds_2[pos_2] += CHSV(gHue + random8(64), 200, 255);
  //  leds_3[pos_3] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);

  //  fadeToBlackBy(leds_1, NUM_LEDS, 20);
  //  fadeToBlackBy(leds_2, NUM_LEDS, 20);
  //  fadeToBlackBy(leds_3, NUM_LEDS, 20);
  //  int pos_1 = beatsin16(13, 0, NUM_LEDS - 1);
  //  int pos_2 = beatsin16(13, 0, NUM_LEDS - 1);
  //  int pos_3 = beatsin16(13, 0, NUM_LEDS - 1);
  //  leds_1[pos_1] += CHSV(gHue, 255, 192);
  //  leds_2[pos_2] += CHSV(gHue, 255, 192);
  //  leds_3[pos_3] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++)
  { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    //    leds_1[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    //    leds_2[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    //    leds_3[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  //  fadeToBlackBy(leds_1, NUM_LEDS, 20);
  //  fadeToBlackBy(leds_2, NUM_LEDS, 20);
  //  fadeToBlackBy(leds_3, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    //    leds_1[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    //    leds_2[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    //    leds_3[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
