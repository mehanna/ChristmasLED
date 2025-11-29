#include <FastLED.h>

// =======================
// FastLED Christmas LED Modular Demo
// =======================
// Modular pattern and palette system
// =======================

#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    350
#define BRIGHTNESS  255
#define FRAMES_PER_SECOND  120
#define updatTime 30

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
  CRGB::Green, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Green, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Green, CRGB::Green, CRGB::Gray, CRGB::Gray,
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


  /*pattern for christmas tree*/
  addPattern(rainbow);
  addPattern(rainbowWithGlitter);
  addPattern(confetti);
  addPattern(juggle);
  addPattern(bpm);
  addPattern(ColorPaletteloop);
  addPattern(rainbowChase);
  addPattern(colorWaves);
  addPattern(movingRainbowBands);
  addPattern(paletteBlendWaves);
  addPattern(sinelonRainbow);


  /*pattern for outdoor lights*/
  /*
  addPattern(rainbow);
  addPattern(rainbowWithGlitter);
  addPattern(confetti);
  addPattern(sinelon);
  addPattern(juggle);
  addPattern(bpm);
  addPattern(ColorPaletteloop);
  addPattern(rainbowChase);
  addPattern(colorWipe);
  addPattern(colorWaves);
  ddPattern(meteorRain);
  addPattern(paletteFireworks);
  addPattern(movingRainbowBands);
  addPattern(paletteBlendWaves);
  addPattern(sinelonRainbow);
  */
  
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


// Candy cane stripes pattern
void rainbowChase() {
  static uint8_t startIndex = 0;
  startIndex++;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((startIndex + i * 8) % 255, 255, 255);
  }
}


void colorWipe() {
  static uint8_t colorIndex = 0;
  static bool forward = true;
  static int pos = 0;
  CRGB colors[] = {CRGB::Green, CRGB::White, CRGB::Red};
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[pos] = colors[colorIndex];
  FastLED.show();
  delay(10);
  if (forward) {
    pos++;
    if (pos >= NUM_LEDS) {
      pos = NUM_LEDS - 1;
      forward = false;
      colorIndex = (colorIndex + 1) % 3;
    }
  } else {
    pos--;
    if (pos < 0) {
      pos = 0;
      forward = true;
      colorIndex = (colorIndex + 1) % 3;
    }
  }
}


void colorWaves() {
  uint8_t wave1 = beatsin8(9, 0, 255);
  uint8_t wave2 = beatsin8(7, 0, 255);
  uint8_t wave3 = beatsin8(5, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((wave1 + wave2 + wave3 + i * 8) % 255, 255, 255);
  }
}

void meteorRain() {
  static uint8_t meteorPos = 0;
  static uint8_t hue = 0;
  fadeToBlackBy(leds, NUM_LEDS, 64);
  for (int i = 0; i < 10; i++) {
    int pos = (meteorPos + i) % NUM_LEDS;
    leds[pos] = CHSV(hue + i * 16, 255, 255);
  }
  meteorPos = (meteorPos + 1) % NUM_LEDS;
  hue += 2;
}

void paletteFireworks() {
  fadeToBlackBy(leds, NUM_LEDS, 18);
  if (random8() < 40) {
    int pos = random16(NUM_LEDS);
    leds[pos] = ColorFromPalette(currentPalette, random8(), 255, currentBlending);
    if (pos > 0) leds[pos - 1] += leds[pos];
    if (pos < NUM_LEDS - 1) leds[pos + 1] += leds[pos];
  }
}

void movingRainbowBands() {
  uint8_t speed = 4;
  uint8_t scale = 30;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t color = sin8(i * scale + millis() / speed);
    leds[i] = CHSV(color, 255, 255);
  }
}

void paletteBlendWaves() {
  uint8_t wave = beatsin8(7, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = wave + i * 8;
    leds[i] = ColorFromPalette(currentPalette, index, 255, currentBlending);
  }
}

void sinelonRainbow() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] = CHSV(gHue, 255, 255);
  // Add extra rainbow dots for more color
  for (int i = 0; i < 3; i++) {
    int offset = beatsin16(7 + i * 2, 0, NUM_LEDS - 1);
    leds[offset] = CHSV(gHue + i * 85, 255, 255);
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