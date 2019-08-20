#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

// How many pixels per strip.
constexpr int kNumPixels = 90;

// How many strips are present
constexpr int kNumStrips = 5;

// First output pin. Strips should be at pins kStartPin to kStartPin+kNumStrips-1.
constexpr int kStartPin = 8;

constexpr int kButtonPin = 2;

// Global multiplier for brightness. In range [0, 255].
constexpr uint8_t kBrightness = 255;

class Button {
 public:
  bool didChange() {
    int reading = digitalRead(kButtonPin);
    bool did_change = false;
    if (reading != last_button_state_) {
      last_debounce_time_ = millis();
    }
    constexpr int kDebounceDelay = 50;
    if ((millis() - last_debounce_time_) > kDebounceDelay) {

      if (reading != button_state_) {
        button_state_ = reading;
        if (reading == HIGH) {
          did_change = true;
        }
      }
    }
    last_button_state_ = reading;
    return did_change;
  }
 private:
  int button_state_= HIGH;
  int last_button_state_ = HIGH;
  unsigned long last_debounce_time_ = 0;
};

struct State {
  Adafruit_NeoPixel pixels[kNumStrips];
  Button button;
  int current_effect = 0;
  uint8_t hue = 0;
  uint8_t hues[kNumStrips][kNumPixels];
  uint8_t vals[kNumStrips][kNumPixels];
};

void fadeToBlackBy(uint8_t vals[kNumStrips][kNumPixels], uint8_t val) {
  for (int strip = 0; strip < kNumStrips; ++strip) {
    for (int led = 0; led < kNumPixels; ++led) {

      int prev = vals[strip][led];
      vals[strip][led] = static_cast<uint8_t>(max(0, prev - val));
    }
  }  
}

class Effect {
 public:
  virtual void step(State& state);
};

class Confetti : public Effect {
 public:
  void step(State& state) override {

    fadeToBlackBy(state.vals, 7);

    for (int strip = 0; strip < kNumStrips; ++strip) {
      int pos = random16(kNumPixels);
      state.vals[strip][pos] = 255;
      state.hues[strip][pos] = state.hue + random8(64);
      for (int led = 0; led < kNumPixels; ++led) {
        state.pixels[strip].setPixelColor(led, state.pixels[strip].ColorHSV(static_cast<uint16_t>(state.hues[strip][led]) * 256, 200, state.vals[strip][led])); 
      }
    }
  }
};


class Sinelon : public Effect {
 public:
    void step(State& state) override {

      fadeToBlackBy(state.vals, 20);

      int pos = beatsin16(20, 0, kNumPixels - 1);
      
      for (int strip = 0; strip < kNumStrips; ++strip) {
        state.vals[strip][pos] = 255;
        for (int led = 0; led < kNumPixels; ++led) {
          state.pixels[strip].setPixelColor(led, state.pixels[strip].ColorHSV(state.hue * 256, 255, state.vals[strip][led]));
        }
      }
    }
};

class MovingPulse : public Effect {
 public:
  void step(State& state) override {
    fadeToBlackBy(state.vals, 40);

    constexpr int kBpm = 60;
    constexpr long kTail = 90;
    const float progress = beat8(kBpm) / 255.0f;
    const int pos = progress * (kNumPixels + kTail);
    
    for (int strip = 0; strip < kNumStrips; ++strip) {
      if (pos < kNumPixels) {
        state.vals[strip][pos] = 255;
      }
      for (int led = 0; led < kNumPixels; ++led) {
        state.pixels[strip].setPixelColor(led, state.pixels[strip].ColorHSV(state.hue * 256, 255, state.vals[strip][led]));
      }
    }
  }
};

class Rainbow : public Effect {
 public:
  Rainbow(const bool random_offsets = false) {
    for (int i = 0; i < kNumStrips; ++i) {
      long offset = 0;
      if (random_offsets) {
        offset = random(kColorWheelRange_);
      }
      steps_[i] = offset;
    }
  }
  void step(State& state) override {
    for (int strip = 0; strip < kNumStrips; ++strip) {
      steps_[strip] = steps_[strip] % kColorWheelRange_;
      for (int led = 0; led < kNumPixels; ++led) {
        state.hues[strip][led] = steps_[strip] + (led * 256 / kNumPixels);        
        state.pixels[strip].setPixelColor(led, state.pixels[strip].ColorHSV(state.hues[strip][led] * 256));
      }
      steps_[strip] += 7;
    }
  }
 private:
  static constexpr long kColorWheelRange_ = 256;
  static constexpr int kDelay_= 0;
  long steps_[kNumStrips];
};


State global_state;

Rainbow rainbow;
MovingPulse pulse;
Sinelon sinelon;
Confetti confetti;

Effect* effects[] = {&confetti, &sinelon, &rainbow, &pulse};

void setup() {
  
  for (int i = 0; i < kNumStrips; ++i) {
    global_state.pixels[i] = Adafruit_NeoPixel(kNumPixels, kStartPin+i, NEO_RGBW + NEO_KHZ800);
    global_state.pixels[i].begin();
    global_state.pixels[i].clear();
    global_state.pixels[i].setBrightness(kBrightness);
    global_state.pixels[i].show();

    for (int j = 0; j < kNumPixels; ++j) {
      global_state.hues[i][j] = 0;
      global_state.vals[i][j] = 0;
    }
  }
  pinMode(kButtonPin, INPUT_PULLUP);
}

void loop() {
  if (global_state.button.didChange()) {
    global_state.current_effect = (global_state.current_effect + 1) % (sizeof(effects) / sizeof(Effect*));
  }

  effects[global_state.current_effect]->step(global_state);
  for (int i = 0; i < kNumStrips; ++i) {
    global_state.pixels[i].show();
  }
  EVERY_N_MILLISECONDS( 20 ) { global_state.hue++; }

}
