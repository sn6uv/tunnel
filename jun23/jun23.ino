#include <Adafruit_NeoPixel.h>

// How many pixels per strip.
#define NUMPIXELS 90

// How many strips are present
#define NUMSTRIPS 1

// First output pin. Strips should be at pins STARTPIN to STARTPIN+NUMSTRIPS-1.
#define STARTPIN 6

// Time (in milliseconds) to pause between steps
#define DELAYVAL 0

// How long is the signal in pixels. Pixels in front of the signal and behind
// will be set to zero.
#define SIGNAL_LENGTH 30
// How many steps (in pixels) to take.
#define NUM_STEPS 300
// Global multiplier for brightness. In range [0, 1.0].
#define BRIGHTNESS 1.0

enum class SignalType {
  kMinusExp = 0,
  kSin = 1
};

void getSignal(const SignalType type, const int t, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* w) {
  const float v = static_cast<float>(t) / SIGNAL_LENGTH;
  if (type == SignalType::kMinusExp) {
    const float kFadeMultiplier = 7.0;
    *r = static_cast<uint8_t>(exp(-v * kFadeMultiplier) * 255.0 * BRIGHTNESS);
    *g = *r;
    *b = *r;
    *w = *r;
  } else if (type == SignalType::kSin) {
    *r = static_cast<uint8_t>(sin(v * PI) * 255.0 * BRIGHTNESS);
    *g = *r;
    *b = *r;
    *w = *r;
  } else {
    *r = 0;
    *g = 0;
    *b = 0;
    *w = 0;
  }
}

Adafruit_NeoPixel pixels[NUMSTRIPS];

// Precomputed arrays.
uint8_t r_arr[SIGNAL_LENGTH];
uint8_t g_arr[SIGNAL_LENGTH];
uint8_t b_arr[SIGNAL_LENGTH];
uint8_t w_arr[SIGNAL_LENGTH];

const SignalType type = SignalType::kMinusExp;
void setup() {
  for (int i = 0; i < NUMSTRIPS; ++i) {
    pixels[i] = Adafruit_NeoPixel(NUMPIXELS, STARTPIN+i, NEO_RGBW + NEO_KHZ800);
    pixels[i].begin();
  }
  // Set all pixel colors to 'off'
  for (int i = 0; i < NUMSTRIPS; ++i) {
    pixels[i].clear();
  }
  // Precompute arrays.
  uint8_t r, g, b, w;
  for (int i = 0; i < SIGNAL_LENGTH; ++i) {
    getSignal(type, i, &r, &g, &b, &w);
    r_arr[i] = r;
    g_arr[i] = g;
    b_arr[i] = b;
    w_arr[i] = w;
  }
}

int step = 0;
void loop() {
  step = step % NUM_STEPS;
  for (int strip = 0; strip < NUMSTRIPS; ++strip) {
    if (step == 0) {
      // Turn off all pixels. This is either the first cycle or the signal is
      // wrapping around for another iteration.
      for (int led = 0; led < NUMPIXELS; ++led) {
        pixels[strip].setPixelColor(led, 0, 0, 0, 0);
      }
      continue;
    }

    // Turn off last pixel from previous step.
    pixels[strip].setPixelColor(step-1, 0, 0, 0, 0);

    // Set lit pixels.
    for (int idx = 0; idx < SIGNAL_LENGTH; ++idx) {
      const int led = step + idx;
      pixels[strip].setPixelColor(led, r_arr[idx], g_arr[idx], b_arr[idx], w_arr[idx]);
    }
  }
  for (int i = 0; i < NUMSTRIPS; ++i) {
    pixels[i].show();
  }
  ++step;
  delay(DELAYVAL);
}
