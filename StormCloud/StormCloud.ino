// THIS VIDEO OUTLINES THE HARDWARE AND GIVES MORE INSIGHT INTO THE CODE 

// you will have to add the neopixel library if you haven't already
// https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-installation

#include <Adafruit_NeoPixel.h> 

// SETTINGS
#define RGB_LED_COUNT             8     // the total number of neopixels on the strip you've attached
#define SPEED_MULTIPLIER          0.003 // the lower this number, the slower all the animations will move
#define RGB_LED_BRIGHTNESS        125   // 0-255, the higher the number the brighter the lights will be ( and more vibrant the colors will be )
#define MOVEMENT_MODIFIER         650   // the different between the lights color, used for making movement across the strip more / less pronounced
#define LIGHTNING_MAX_BRIGHTNESS  60    // 0 - 127, the maximum brightness of the lightning strikes
#define LIGHTNING_CHANCE          1500  // the lower the number is, the more often lightning will strike
#define LIGHTNING_MIN_TIME        100.0 // the minimum amount of milliseconds lightning will strike for
#define LIGHTNING_MAX_TIME        500.0 // the maximum amoung of milliseconds lightning will strike for
#define LIGHTNING_WAIT_TIME       3000.0// the amount of milliseconds that must elapse before another lightning strike can happen
#define LIGHTNING_MIN_MODIFIER    20    // the minimum speed of lightning strike oscilations
#define LIGHTNING_MAX_MODIFIER    35    // the maximum speed of lightning striek oscilations

// PINS
#define LED_PIN                   5

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(RGB_LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long elapsed;
byte r;
byte g;
byte b;
byte ledIndex = 0;
unsigned long lightningEnd = 0.0;
unsigned long lightningWait = 0.0;
unsigned long lightningModifier = 0.0;

void setup() {

  // serial communications commented out, as they're not required for regular operations
  //Serial.begin(115200);

  // ensure the 'random' numbers that we get every run do not match
  randomSeed(analogRead(0));

  // start the neopixel strip and set it's initial brightness
  strip.begin();
  strip.setBrightness(RGB_LED_BRIGHTNESS);
}

void loop() {

  // first we get some kind of constantly increasing number to use in our calculations
  //
  // millis() returns the amount of milliseconds that have elapsed since the program has started.
  // NOTE: this will reset after about 50 days, although the impact of this on our use should be 
  // nothing.
  // 
  // ledIndex is a number that goes from 0 to the number of neopixels on your strip
  // this number increases every loop until it hits the max number and resets back to zero
  // we use this number multiplied by the setting MOVEMENT_MODIFIER to increase the values between
  // neopixel colors
  elapsed = millis() + (float(ledIndex) * MOVEMENT_MODIFIER);

  // the blue formula is the same regardless of whether lightning is striking or not, so it's outside
  // of the switch below. more details on why this formula looks the way it does can be found in the
  // companion video linked at the top of the file
  b = byte(((pow(sin((elapsed * SPEED_MULTIPLIER) / 4.0), 3.0) + 1.0) * 80.0) + 90.0);
  
  // if there is no lightningEnd time set, then we are not running lightning
  // so just show the idle animation instead
  if (lightningEnd == 0) {

    // more details on why these formulas look the way they do can be found in the
    // companion video linked at the top of the file
    r = byte((sin((elapsed * SPEED_MULTIPLIER) / 5.0) + 1.0) * 40.0);
    g = byte(pow(sin(((elapsed * SPEED_MULTIPLIER) / 8.0) + PI / 2.0), 4.0) * 50.0);
    
    // first check if we've past the wait period before another strike can even happen
    // then if the random chance of lightning == 1, we'll start a lightning strike
    if (millis() >= lightningWait && random(LIGHTNING_CHANCE) == 1) {

      // figure out how long this strike would run for, and add it to the current time. 
      // that's how we know when to stop doing it
      lightningEnd = millis() + random(LIGHTNING_MIN_TIME, LIGHTNING_MAX_TIME);

      // figure out how fast the lightning should oscillate
      lightningModifier = random(LIGHTNING_MIN_MODIFIER, LIGHTNING_MAX_MODIFIER);
    }
    
  } else {

    // we want to create something closer to white, so we make r and g the same values
    // more details on why these formulas look the way they do can be found in the
    // companion video linked at the top of the file
    r = g = byte((sin(elapsed / lightningModifier) + 1.0) * LIGHTNING_MAX_BRIGHTNESS);

    // check if the current time has passed the end of the lightning strike
    if (millis() >= lightningEnd) {

      // to stop the lightning strike, we reset the end time back to 0
      lightningEnd = 0;

      // we'll also mark the amount of time we want to disable lightning, that way it doesn't happen too often
      lightningWait = millis() + LIGHTNING_WAIT_TIME;
    }
  }

  // serial communications commented out, as they're not required for regular operations
  /*
  if (ledIndex == 0) {
    
    // this serious output is meant to be viewed on the Serial Plotter
    // to make the plotter display multiple graphs at the same time you have to seperate your
    // numeric values with commas and end the set with a line feed
    // 
    // you'll notice the order of the colors seems kind of weird. it's b, r and g becuase that
    // matches the coloring of the first, second and third graphs on the plotter
    Serial.print(b);
    Serial.print(",");
    Serial.print(r);
    Serial.print(",");
    Serial.println(g);
  }
  */
 
  // push the calculated colors to the current neopixel
  strip.setPixelColor(ledIndex, r, g, b);

  // write any changes to the strip
  strip.show();

  // increase the ledIndex until it hits the maximum value and reset it back to 0
  ledIndex ++;
  if (ledIndex >= RGB_LED_COUNT)
    ledIndex = 0;
}
