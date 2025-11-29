#include <FastLED.h>

// =======================
// FastLED Christmas LED Modular Demo
// =======================
// Modular pattern and palette system
// =======================

#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    50
#define BRIGHTNESS  255
#define FRAMES_PER_SECOND  120
#define updatTime 20

CRGB leds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// =======================
// Modular Pattern System
// =======================

#define MAX_PATTERNS 10
typedef void (*PatternFn)();
PatternFn activePatterns[MAX_PATTERNS];
uint8_t numActivePatterns = 0;
uint8_t currentPatternIndex = 0;

// Add a pattern to the active list
void addPattern(PatternFn pattern) {
  if (numActivePatterns < MAX_PATTERNS) {
    activePatterns[numActivePatterns++] = pattern;
  }
}

// Remove a pattern by index
void removePattern(uint8_t index) {
  if (index < numActivePatterns) {
    for (uint8_t i = index; i < numActivePatterns - 1; i++) {
      activePatterns[i] = activePatterns[i + 1];
    }
    numActivePatterns--;
  }
}

// =======================
// Static Palette in PROGMEM
// =======================

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
  CRGB::Blue, CRGB::Blue, CRGB::Black, CRGB::Black
};

// =======================
// Palette Change Functions
// =======================

void setRainbowStripeNoBlend() {
  currentPalette = RainbowStripeColors_p;
  currentBlending = NOBLEND;
}

void setRainbowStripeBlend() {
  currentPalette = RainbowStripeColors_p;
  currentBlending = LINEARBLEND;
}

void setPurpleGreenBlend() {
  SetupPurpleAndGreenPalette();
  currentBlending = LINEARBLEND;
}

void setBWStripeNoBlend() {
  SetupBlackAndWhiteStripedPalette();
  currentBlending = NOBLEND;
}

void setBWStripeBlend() {
  SetupBlackAndWhiteStripedPalette();
  currentBlending = LINEARBLEND;
}

void setCloudBlend() {
  currentPalette = CloudColors_p;
  currentBlending = LINEARBLEND;
}

void setPartyBlend() {
  currentPalette = PartyColors_p;
  currentBlending = LINEARBLEND;
}

void setRWBNoBlend() {
  currentPalette = myRedWhiteBluePalette_p;
  currentBlending = NOBLEND;
}

void setRWBBlend() {
  currentPalette = myRedWhiteBluePalette_p;
  currentBlending = LINEARBLEND;
}

// Array of palette change functions
typedef void (*PaletteFn)();
PaletteFn paletteChanges[] = {
  setRainbowStripeNoBlend,
  setRainbowStripeBlend,
  setPurpleGreenBlend,
  setBWStripeNoBlend,
  setBWStripeBlend,
  setCloudBlend,
  setPartyBlend,
  setRWBNoBlend,
  setRWBBlend
};
const uint8_t numPaletteChanges = sizeof(paletteChanges) / sizeof(PaletteFn);

// =======================
// Setup & Main Loop
// =======================

void setup() {
  delay(3000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  currentPalette = RainbowStripeColors_p;
  currentBlending = NOBLEND;
  Serial.begin(9600);

  // Add only the patterns you want to run:
  addPattern(rainbow);
  addPattern(rainbowWithGlitter);
  addPattern(confetti);
  addPattern(sinelon);
  addPattern(juggle);
  addPattern(bpm);
  addPattern(ColorPaletteloop);
  // You can comment out or add more patterns here
}

void loop() {
  if (numActivePatterns == 0) return; // No patterns to show

  activePatterns[currentPatternIndex]();
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_SECONDS(updatTime) {
    currentPatternIndex = (currentPatternIndex + 1) % numActivePatterns;
  }
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

// =======================
// Pattern Implementations
// =======================

// FastLED's built-in rainbow generator
void rainbow() {
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

// Built-in FastLED rainbow, plus some random sparkly glitter
void rainbowWithGlitter() {
  rainbow();
  addGlitter(80);
}

// Add random white glitter
void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

// Random colored speckles that blink in and fade smoothly
void confetti() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

// A colored dot sweeping back and forth, with fading trails
void sinelon() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS-1);
  leds[pos] += CHSV(gHue, 255, 192);
}

// Colored stripes pulsing at a defined Beats-Per-Minute (BPM)
void bpm() {
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(palette, gHue + (i*2), beat - gHue + (i*10));
  }
}

// Eight colored dots, weaving in and out of sync with each other
void juggle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i+7, 0, NUM_LEDS-1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// =======================
// Palette Loop Pattern
// =======================

uint8_t exitloop;

void ColorPaletteloop() {
  exitloop = 1;
  while (exitloop) {
    ChangePalettePeriodically();
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    FillLEDsFromPaletteColors(startIndex);
    FastLED.show();
    FastLED.delay(1000 / 100);
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

// =======================
// Consistent Palette Cycling
// =======================

void ChangePalettePeriodically() {
  uint8_t paletteIndex = (millis() / 1000) / updatTime;
  static uint8_t lastIndex = 99;
  if (lastIndex != paletteIndex && paletteIndex < numPaletteChanges) {
    lastIndex = paletteIndex;
    paletteChanges[paletteIndex]();
  }
  if (paletteIndex >= numPaletteChanges)
    exitloop = 0;
}

// =======================
// Palette Setup Functions
// =======================

void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

void SetupBlackAndWhiteStripedPalette() {
  fill_solid(currentPalette, 16, CRGB::Black);
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green  = CHSV(HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  currentPalette = CRGBPalette16(
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black
  );
}