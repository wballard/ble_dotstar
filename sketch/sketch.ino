
#include <avr/pgmspace.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <RFduinoBLE.h>

#define NUMPIXELS 144

#define DATAPIN    5 //the green wire
#define CLOCKPIN   6 //the yellow wire
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN);
#define FLASH_PAGE  249
uint32_t *color = ADDRESS_OF_PAGE(FLASH_PAGE);
uint32_t current_color = *color;

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS);

void setup() {
  Serial.begin(9600);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  // start the BLE stack
  RFduinoBLE.deviceName = "Kitchen Light";
  RFduinoBLE.advertisementData = "rgb";
  RFduinoBLE.begin();

  if (current_color == 0) {
    current_color = 0xFFFFFF;
    save(current_color);
  }
  RFduinoBLE_onReceive("",0);
}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.


void loop() {
  // switch to lower power mode
  RFduino_ULPDelay(INFINITE);
}

void save(uint32_t new_color) {
  int rc = flashPageErase(FLASH_PAGE);
  if (rc == 0)
    Serial.println("Success");
  else if (rc == 1)
    Serial.println("Error - the flash page is reserved");
  else if (rc == 2)
    Serial.println("Error - the flash page is used by the sketch");
  rc = flashWrite(color, new_color);
  if (rc == 0)
    Serial.println("Success");
  else if (rc == 1)
    Serial.println("Error - the flash page is reserved");
  else if (rc == 2)
    Serial.println("Error - the flash page is used by the sketch");
}


void RFduinoBLE_onReceive(char *data, int len) {
  // each transmission should contain an RGB triple

  if (len >= 3)
  {
    // get the RGB values
    uint32_t r = data[0];
    uint32_t g = data[1];
    uint32_t b = data[2];
    Serial.println(r, HEX);
    Serial.println(g, HEX);
    Serial.println(b, HEX);
    current_color = (r << 16) + (g << 8) + b;
    Serial.println("COLOR CHANGE");
    Serial.println(current_color, HEX);
    save(current_color);
  }
  Serial.println("COLOR DISPLAY");
  Serial.println(current_color, HEX);
  for (int i=0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, current_color);
    //semi-animation -- show the strip every five lights
    if (i%10 == 0) {
      strip.show();
    }
  }
  strip.show();

}
