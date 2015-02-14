// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <RFduinoBLE.h>

#define NUMPIXELS 144 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    5
#define CLOCKPIN   6
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN);
uint32_t color = 0xFF0000;      // 'On' color (starts red)

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS);

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  // start the BLE stack
  RFduinoBLE.advertisementData = "rgb";
  RFduinoBLE.begin();
  RFduinoBLE_onReceive("", 0);
}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.


void loop() {
  // switch to lower power mode
  RFduino_ULPDelay(INFINITE);

}


void RFduinoBLE_onReceive(char *data, int len) {
  // each transmission should contain an RGB triple
  if (len >= 3)
  {
    // get the RGB values
    uint32_t r = data[0];
    uint32_t g = data[1];
    uint32_t b = data[2];
    color = (r << 16) + (g << 8) + b;
  }
  for (int i=0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();

}
