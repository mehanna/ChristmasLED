#include <FastLED.h>

/*
  =======================
  FastLED Christmas LED Modular Demo
  =======================
  This code controls two LED strips (or segments) with different color orders using the FastLED library.
  It features a modular pattern system, palette cycling, and a variety of colorful effects for Christmas tree lights.

  --- HARDWARE SETUP ---
  - DATA_PIN: The pin used to control both LED strips.
  - LED_TYPE: The type of LED strip (e.g., WS2811).
  - COLOR_ORDER1: Color order for the first strip (GRB).
  - COLOR_ORDER2: Color order for the second strip (RGB).
  - NUM_LEDS1: Number of LEDs in the first strip (main tree).
  - NUM_LEDS2: Number of LEDs in the second strip (e.g., star).

  --- SOFTWARE FEATURES ---
  - Patterns are modular: you can add/remove patterns easily.
  - Palette system: color palettes can be changed periodically.
  - Each pattern runs for 'updatTime' seconds before switching.
  - Both strips are updated in every pattern for synchronized effects.
*/

#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER1 GRB
#define COLOR_ORDER2 RGB
#define NUM_LEDS1    350
#define NUM_LEDS2    50 // this is for a second strip (christmas star)
#define BRIGHTNESS  255
#define FRAMES_PER_SECOND  120
#define updatTime 30 // Duration (seconds) each pattern runs

// LED arrays for each strip
CRGB leds1[NUM_LEDS1];
CRGB leds2[NUM_LEDS2];

// Palette and blending mode for color effects
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

// Global hue for color cycling
uint8_t gHue = 0;

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
// Example palette for red, white, and blue effects
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Green, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Green, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Green, CRGB::Green, CRGB::Gray, CRGB::Gray,
  CRGB::Blue, CRGB::Blue, CRGB::Black, CRGB::Black
};

// =======================
// Palette Change Functions
// =======================
// Functions to set different palettes and blending modes
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

// Array of palette change functions for cycling
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
  // Add both LED strips with their respective color orders
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER1>(leds1, NUM_LEDS1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER2>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  currentPalette = RainbowStripeColors_p;
  currentBlending = NOBLEND;
  Serial.begin(9600);

  // Register your patterns here (order matters for cycling)
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
}

// =======================
// Main Loop
// =======================

void loop() {
  if (numActivePatterns == 0) return; // No patterns to show

  // Run the current pattern
  activePatterns[currentPatternIndex]();
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // Switch to the next pattern every 'updatTime' seconds
  EVERY_N_SECONDS(updatTime) {
    currentPatternIndex = (currentPatternIndex + 1) % numActivePatterns;
  }
  // Slowly cycle the base hue for color effects
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

// =======================
// Pattern Implementations (all use leds1 and leds2)
// =======================

// Classic rainbow across both strips
void rainbow() {
  fill_rainbow(leds1, NUM_LEDS1, gHue, 7);
  fill_rainbow(leds2, NUM_LEDS2, gHue, 7);
}

// Rainbow with random white glitter
void rainbowWithGlitter() {
  rainbow();
  addGlitter(80);
}

// Adds white glitter randomly
void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds1[random16(NUM_LEDS1)] += CRGB::White;
    leds2[random16(NUM_LEDS2)] += CRGB::White;
  }
}

// Confetti: random colored speckles that blink in and fade smoothly
void confetti() {
  fadeToBlackBy(leds1, NUM_LEDS1, 10);
  fadeToBlackBy(leds2, NUM_LEDS2, 10);
  int pos1 = random16(NUM_LEDS1);
  int pos2 = random16(NUM_LEDS2);
  leds1[pos1] += CHSV(gHue + random8(64), 200, 255);
  leds2[pos2] += CHSV(gHue + random8(64), 200, 255);
}

// Sinelon: a colored dot sweeping back and forth, with fading trails
void sinelon() {
  fadeToBlackBy(leds1, NUM_LEDS1, 20);
  fadeToBlackBy(leds2, NUM_LEDS2, 20);
  int pos1 = beatsin16(13, 0, NUM_LEDS1-1);
  int pos2 = beatsin16(13, 0, NUM_LEDS2-1);
  leds1[pos1] += CHSV(gHue, 255, 192);
  leds2[pos2] += CHSV(gHue, 255, 192);
}

// BPM: colored stripes pulsing at a defined Beats-Per-Minute
void bpm() {
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = ColorFromPalette(palette, gHue + (i*2), beat - gHue + (i*10));
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = ColorFromPalette(palette, gHue + (i*2), beat - gHue + (i*10));
  }
}

// Juggle: 8 colored dots, weaving in and out of sync with each other
void juggle() {
  fadeToBlackBy(leds1, NUM_LEDS1, 20);
  fadeToBlackBy(leds2, NUM_LEDS2, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds1[beatsin16(i+7, 0, NUM_LEDS1-1)] |= CHSV(dothue, 200, 255);
    leds2[beatsin16(i+7, 0, NUM_LEDS2-1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// Rainbow chase: rainbow colors chase each other along the strip
void rainbowChase() {
  static uint8_t startIndex = 0;
  startIndex++;
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = CHSV((startIndex + i * 8) % 255, 255, 255);
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = CHSV((startIndex + i * 8) % 255, 255, 255);
  }
}

// Color waves: smooth, flowing waves of color
void colorWaves() {
  uint8_t wave1 = beatsin8(9, 0, 255);
  uint8_t wave2 = beatsin8(7, 0, 255);
  uint8_t wave3 = beatsin8(5, 0, 255);
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = CHSV((wave1 + wave2 + wave3 + i * 8) % 255, 255, 255);
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = CHSV((wave1 + wave2 + wave3 + i * 8) % 255, 255, 255);
  }
}

// Moving rainbow bands: bands of rainbow colors move along the strip
void movingRainbowBands() {
  uint8_t speed = 4;
  uint8_t scale = 30;
  for (int i = 0; i < NUM_LEDS1; i++) {
    uint8_t color = sin8(i * scale + millis() / speed);
    leds1[i] = CHSV(color, 255, 255);
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    uint8_t color = sin8(i * scale + millis() / speed);
    leds2[i] = CHSV(color, 255, 255);
  }
}

// Palette blend waves: smoothly blends colors from the current palette in waves
void paletteBlendWaves() {
  uint8_t wave = beatsin8(7, 0, 255);
  for (int i = 0; i < NUM_LEDS1; i++) {
    uint8_t index = wave + i * 8;
    leds1[i] = ColorFromPalette(currentPalette, index, 255, currentBlending);
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    uint8_t index = wave + i * 8;
    leds2[i] = ColorFromPalette(currentPalette, index, 255, currentBlending);
  }
}

// Sinelon rainbow: multiple rainbow dots sweep back and forth
void sinelonRainbow() {
  fadeToBlackBy(leds1, NUM_LEDS1, 20);
  fadeToBlackBy(leds2, NUM_LEDS2, 20);
  int pos1 = beatsin16(13, 0, NUM_LEDS1 - 1);
  int pos2 = beatsin16(13, 0, NUM_LEDS2 - 1);
  leds1[pos1] = CHSV(gHue, 255, 255);
  leds2[pos2] = CHSV(gHue, 255, 255);
  for (int i = 0; i < 3; i++) {
    int offset1 = beatsin16(7 + i * 2, 0, NUM_LEDS1 - 1);
    int offset2 = beatsin16(7 + i * 2, 0, NUM_LEDS2 - 1);
    leds1[offset1] = CHSV(gHue + i * 85, 255, 255);
    leds2[offset2] = CHSV(gHue + i * 85, 255, 255);
  }
}

// =======================
// Palette Loop Pattern
// =======================

// Loops through palettes for a dynamic effect
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

// Fills both strips from the current palette
void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

// =======================
// Consistent Palette Cycling
// =======================

// Changes palette every updatTime seconds
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

// Creates a totally random palette
void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

// Black and white striped palette
void SetupBlackAndWhiteStripedPalette() {
  fill_solid(currentPalette, 16, CRGB::Black);
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// Purple and green palette for Halloween or fun effects
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