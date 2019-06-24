#include <Adafruit_NeoPixel.h>

// How many pixels per strip.
#define NUMPIXELS 90

// How many strips are present
#define NUMSTRIPS 5

// First output pin. Strips should be at pins STARTPIN to STARTPIN+NUMSTRIPS-1.
#define STARTPIN 6

// Time (in milliseconds) to pause between steps
#define DELAYVAL 0

Adafruit_NeoPixel pixels[NUMSTRIPS];

enum class WaveType {
  kSin = 0,
};

void getRgb(const WaveType wave_type, const int step, const int num_steps,
            const int led_idx, const int num_leds, uint8_t* r, uint8_t* g,
            uint8_t* b) {
  const float idx_normalized = static_cast<float>(led_idx) / num_leds;
  const float step_normalized = static_cast<float>(step) / num_steps;
  float val = 0.0;
  const int diff = step - led_idx;
  if (diff > 0 && diff < 20) {
    val = exp(-static_cast<float>((abs(diff))) / 2);
  }
  //const float val = sin((idx_normalized + step_normalized) * PI);
  float normalized_val = max(0.0, min(1.0, val));
  *r = static_cast<uint8_t>(normalized_val * 255);
  *g = static_cast<uint8_t>(normalized_val * 255);
  *b = static_cast<uint8_t>(normalized_val * 255);
}

void setup() {
  for (int i = 0; i < NUMSTRIPS; ++i) {
    pixels[i] = Adafruit_NeoPixel(NUMPIXELS, STARTPIN+i, NEO_RGBW + NEO_KHZ800);
    pixels[i].begin();
  }
}

void loop() {
  // Set all pixel colors to 'off'
  for (int i = 0; i < NUMSTRIPS; ++i) {
    pixels[i].clear();
  }

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  const int kNumSteps = 100;
  uint8_t r, g, b;
  int step = 0;

  while (true) {
    step = step % kNumSteps;
    for (int led = 0; led < NUMPIXELS; ++led) {
      getRgb(WaveType::kSin, step, kNumSteps, led, NUMPIXELS, &r, &g, &b);
      for (int i = 0; i < NUMSTRIPS; ++i) {
        pixels[i].setPixelColor(led, pixels[i].Color(g, r, b));
      }
    }
    for (int i = 0; i < NUMSTRIPS; ++i) {
      pixels[i].show();
    }
    ++step;
    delay(DELAYVAL);
  }
}
