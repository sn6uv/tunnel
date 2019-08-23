#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

// How many pixels per strip.
constexpr int kNumPixels = 90;

// How many strips are present
constexpr int kNumStrips = 5;

// First output pin. Strips should be at pins kStartPin to kStartPin+kNumStrips-1.
constexpr int kStartPin = 8;

// Which pin the button is connected to.
constexpr int kButtonPin = 2;

// Global multiplier for brightness. In range [0, 255].
constexpr uint8_t kBrightness = 255;

// Handles button state.
class Button {
 public:
  // Returns true if button was pressed after the last call.
  bool wasPressed() {
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

      auto& strip_obj = state.pixels[strip];

      int pos = random(kNumPixels);
      state.vals[strip][pos] = 255;
      state.hues[strip][pos] = state.hue + random(64);
      for (int led = 0; led < kNumPixels; ++led) {
        strip_obj.setPixelColor(led, strip_obj.gamma32(strip_obj.ColorHSV(static_cast<uint16_t>(state.hues[strip][led]) * 256, 150, state.vals[strip][led])));
      }
    }
  }
};

class Angus : public Effect {
 public:
  void step(State& state) override {
    static long int t = 0;
    t++;
    constexpr float kFreq = 0.5;
    constexpr float kDelta = 0.1;

    for (int strip = 0; strip < kNumStrips; ++strip) {
      auto& strip_obj = state.pixels[strip];
      for (int led = 0; led < kNumPixels; ++led) {
        float phase = kDelta * led;
        float val = (sin(kFreq * t - phase) + 1.0) / 2.0;
        strip_obj.setPixelColor(led, strip_obj.gamma32(strip_obj.ColorHSV(state.hue * 255, 255, val * 255)));
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
        auto& strip_obj = state.pixels[strip];
        state.vals[strip][pos] = 255;
        for (int led = 0; led < kNumPixels; ++led) {
          strip_obj.setPixelColor(led, strip_obj.gamma32(strip_obj.ColorHSV(65535, 0, state.vals[strip][led])));
        }
      }
    }
};

class MovingPulse : public Effect {
 public:
  void step(State& state) override {
    fadeToBlackBy(state.vals, 20);

    constexpr int kBpm = 40;
    constexpr long kTail = 30;
    const float progress = beat16(kBpm) / static_cast<float>(65536l);
    const long int pos = progress * (kNumPixels + kTail);
    
    for (int strip = 0; strip < kNumStrips; ++strip) {
      auto& strip_obj = state.pixels[strip];
      if (pos < kNumPixels) {
        state.vals[strip][pos] = 255;
      }
      for (int led = 0; led < kNumPixels; ++led) {
        strip_obj.setPixelColor(led, strip_obj.gamma32(strip_obj.ColorHSV(65536, 0, state.vals[strip][led])));
      }
    }
  }
};

class Rainbow : public Effect {
 public:
  void step(State& state) override {
    
    constexpr int kBpm = 40;
    uint16_t start_hue = beat16(kBpm);
    constexpr int kRainbowLengthPixels = 60;

    for (int strip = 0; strip < kNumStrips; ++strip) {
      auto& strip_obj = state.pixels[strip];
      for (int led = 0; led < kNumPixels; ++led) {        
        long int hue = (start_hue - 65535l * led  / kRainbowLengthPixels) % 65536l;
        strip_obj.setPixelColor(led, strip_obj.gamma32(strip_obj.ColorHSV(hue, 255, 255)));
      }
    }
  }
};

class RainbowPulses : public Effect {
 public:

  void step(State& state) override {
    constexpr int kBpm = 50;
    long start_hue = beat16(kBpm);
    constexpr int kRainbowLengthPixels = 30;

    constexpr long kTail = 10000;
    for (int strip = 0; strip < kNumStrips; ++strip) {
      for (int led = 0; led < kNumPixels; ++led) {        
        uint16_t hue = (start_hue - 65535l * static_cast<long>(led)  / kRainbowLengthPixels) % 65536l;
        uint8_t val = 255;
        if (hue > 65536l - kTail) {
          val = 0;
        }
        hue = (65536l * hue) / (65536l - kTail);
        state.pixels[strip].setPixelColor(led, state.pixels[strip].ColorHSV(hue, 255, val));
      }
    }
  }
};

State global_state;

Rainbow rainbow;
MovingPulse pulse;
Sinelon sinelon;
Confetti confetti;
RainbowPulses rainbow_pulse;
Angus angus;

Effect* effects[] = {&confetti, &rainbow, &angus, &pulse};

void setup() {
  
  for (int i = 0; i < kNumStrips; ++i) {
    global_state.pixels[i] = Adafruit_NeoPixel(kNumPixels, kStartPin + i, NEO_RGBW + NEO_KHZ800);
    global_state.pixels[i].begin();
    global_state.pixels[i].clear();
    global_state.pixels[i].setBrightness(kBrightness);

    for (int j = 0; j < kNumPixels; ++j) {
      global_state.hues[i][j] = 0;
      global_state.vals[i][j] = 0;
    }
  }
  pinMode(kButtonPin, INPUT_PULLUP);
}

void loop() {
  constexpr int kNumEffects = sizeof(effects) / sizeof(Effect*);
  if (global_state.button.wasPressed()) {
    global_state.current_effect = (global_state.current_effect + 1) % kNumEffects;
  }

  effects[global_state.current_effect]->step(global_state);
  for (int i = 0; i < kNumStrips; ++i) {
    global_state.pixels[i].show();
  }
  EVERY_N_MILLISECONDS( 40 ) { global_state.hue++; }

}
