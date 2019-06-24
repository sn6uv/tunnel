#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 90

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels0(NUMPIXELS, 7, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel pixels1(NUMPIXELS, 8, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS, 9, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel pixels3(NUMPIXELS, 10, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel pixels4(NUMPIXELS, 6, NEO_RGBW + NEO_KHZ800);

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


#define DELAYVAL 0 // Time (in milliseconds) to pause between pixels

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  //Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pixels0.begin();
  pixels1.begin();
  pixels2.begin();
  pixels3.begin();
  pixels4.begin();
  
}

void loop() {
  // Set all pixel colors to 'off'
  pixels0.clear();
  pixels1.clear();
  pixels2.clear();
  pixels3.clear();
  pixels4.clear();
  
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  const int kNumSteps = 100;
  uint8_t r, g, b;
  int step = 0;
  
  while(true) {
    step = step % kNumSteps;
    for (int led = 0; led < NUMPIXELS; ++led) {
      getRgb(WaveType::kSin, step, kNumSteps, led, NUMPIXELS, &r, &g, &b);
      pixels0.setPixelColor(led, pixels0.Color(g, r, b));
      pixels1.setPixelColor(led, pixels1.Color(g, r, b));
      pixels2.setPixelColor(led, pixels2.Color(g, r, b));
      pixels3.setPixelColor(led, pixels3.Color(g, r, b));
      pixels4.setPixelColor(led, pixels4.Color(g, r, b));
    }
    pixels0.show();
    pixels1.show();
    pixels2.show();
    pixels3.show();
    pixels4.show();
    ++step;
    delay(DELAYVAL); // Pause before next pass through loop 
  }
}
