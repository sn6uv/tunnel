#include "FastLED.h"
#define NUM_LEDS_PER_STRIP 120
#define NUM_STRIPS 5
#define START_PIN 6
#define DELAYVAL 0 // Time (in milliseconds) to pause between pixels

CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

enum class WaveType {
  kSin = 0,
};
  
void setup() {
  FastLED.addLeds<SK6812, START_PIN + 0, RGB>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<SK6812, START_PIN + 1, RGB>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<SK6812, START_PIN + 2, RGB>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<SK6812, START_PIN + 3, RGB>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<SK6812, START_PIN + 4, RGB>(leds[4], NUM_LEDS_PER_STRIP);
}



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

void loop() {
  FastLED.clear();
  const int kNumSteps = 100;
  uint8_t r, g, b;
  int step = 0;
  for (int strip = 0; strip < NUM_STRIPS; ++strip) {
    for (int led = 0; led < NUM_LEDS_PER_STRIP; ++led) {
      leds[strip][led].r = 50;
      leds[strip][led].g = 50;
      leds[strip][led].b = 50;
    }
  }
  FastLED.show();
//  while(true) {
//    step = step % kNumSteps;
//    for (int led = 0; led < NUM_LEDS_PER_STRIP; ++led) {
//      getRgb(WaveType::kSin, step, kNumSteps, led, NUM_LEDS_PER_STRIP, &r, &g, &b);
//      for (int strip = 0; strip < NUM_STRIPS; ++strip) {
//        leds[strip][led].r = r;
//        leds[strip][led].g = g;
//        leds[strip][led].b = b;
//      }
//    }
//    FastLED.show();
//    ++step;
//    delay(DELAYVAL); // Pause before next pass through loop 
//  }
}
