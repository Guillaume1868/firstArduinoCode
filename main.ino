#include <Adafruit_NeoPixel.h>

/**
 * NEO Pixel Turn signals.  
 * Author: Timothy Lorens <tlorens@cyberdyne.org>
 * Edit by : Guillaume Aubert
 * Date: 07/05/2018
 * Edit Date: 18/09/2018
 * Simple sequential (sweeping) Turn signal logic with dim solid red run state lights
 * along with white reverse light. AnalogRead() is used to determine if there's voltage
 * on one of the analog pins. Since the blinker relay isn't consistent our loop should be
 * relatively fast to ensure we catch the voltage reading.  Once we get one, fire the animation
 * for which direction we're turning or set the reverse light on.  
 *
 * See Also: https://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
 * Video: https://www.youtube.com/watch?v=-3qWpKn6ggE
 */
 
#define LeftSignal A5     // Analog Pin 5 used as trigger for left turn-signal
#define RightSignal A4    // Analog Pin 4 used as trigger for right turn-signal
#define ReverseSignal A3  // Analog Pin 3 used as trigger for reverse light
#define OFFSignal A2
#define PIN 10             // NeoPixel data pin  
#define BRIGHTNESS 255    // Full brightness
#define NUM_PIXELS 10      // Total number of NeoPixels
#define WIPE_SPEED 50     // Sweep animation delay.
#define AnimDel 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int LeftBlinkerState  = 0;
int RightBlinkerState = 0;
int ReverseLightState = 0;
int OFFState = 0;
int runLightState     = 0;
int reverseState      = 0;

uint32_t AMBER = strip.Color(255, 130, 0);
uint32_t RED   = strip.Color(255, 0, 0);
uint32_t WHITE = strip.Color(255, 255, 255);
uint32_t BLUE  = strip.Color(0, 0, 255);
uint32_t OFF   = strip.Color(0, 0, 0);

/**
 * Setup pins and do fancy start-up sequence. 
 * Red/White/Blue Larson scanner. 
 */
void setup() {
  pinMode(LeftSignal, INPUT); 
  pinMode(RightSignal, INPUT);   
  pinMode(ReverseSignal, INPUT);   
  pinMode(OFF, INPUT);   
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();   
  knightRider(1, WIPE_SPEED, 2, RED);  
  knightRider(1, WIPE_SPEED, 2, WHITE);
  knightRider(1, WIPE_SPEED, 2, BLUE);    
}

/**
 * Main loop
 */
void loop() {
  // Read the state of the left turn-signal voltage
  LeftBlinkerState = analogRead(LeftSignal);
  float LeftVoltage = LeftBlinkerState * (5.0 / 1023.0);  

OFFState = analogRead(OFFSignal);
  float OFFVoltage = OFFState * (5.0 / 1023.0);  

  // Read the state of the right turn-signal voltage
  RightBlinkerState = analogRead(RightSignal);
  float RightVoltage = RightBlinkerState * (5.0 / 1023.0);  

  // Read the state of the reverse lights voltage
  ReverseLightState = analogRead(ReverseSignal);
  float ReverseVoltage = ReverseLightState * (5.0 / 1023.0);  

  // If we have voltage clear the strip from 'run light state' 
  // and start Larson scan to the left. 

    
  if (LeftVoltage > 4.99) {
    runLightState = 0;
    reverseState = 0;
    clearStrip();
    leftTurn();
    delay(210);
  } else if (RightVoltage > 4.99 ) {
    // If we have voltage clear the strip from 'run light state' start Larson scan to the right. 
    runLightState = 0;
    reverseState = 0;
    clearStrip();
    rightTurn();
    delay(210);
  } 
  else if (ReverseVoltage > 4.99) {
     runLightState = 0;
     reverseState = 0;
     //clearStrip();
     reverseLight();
     delay(0);
  }else if (OFFVoltage > 4.99) {
    runLightState = 0;
    reverseState = 0;
    clearStrip();
    delay(220);
  } else if (RightVoltage < 3.0 && LeftVoltage < 3.0 && ReverseVoltage < 3.0 && OFFVoltage < 3.0) { 
    // If there's < 3v (capasitor drain) revert to run light state. 
    runLight();
    delay(200);
  }
  
  
}

/**
 * Larson scan for right turn.
 */
void rightTurn() {
  strip.setBrightness(BRIGHTNESS);
  
  // Turn strip on. 
  for (uint16_t i = NUM_PIXELS / 2; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, AMBER);
    strip.show();
    delay(AnimDel);
  }
  
  delay(300);
  
  // Turn strip off.
  for (uint16_t i = NUM_PIXELS / 2; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, OFF);
    strip.show();
    delay(AnimDel);
  }
  
  delay(150);
}

/**
 * Larson scan for Left turn.
 */
void leftTurn() {
  strip.setBrightness(BRIGHTNESS);
  
  for (int16_t i = (NUM_PIXELS-1) /2 ; i > -1; i--) {
    strip.setPixelColor(i, AMBER);
    strip.show();
    delay(AnimDel);
  }

  delay(300);

  for (int16_t i = NUM_PIXELS / 2; i > -1; i--) {
    strip.setPixelColor(i, OFF);
    strip.show();
    delay(AnimDel);
  }  
  
  delay(150);
}

/**
 * Helper function to make all LEDs color (c)
 */
void all(uint32_t c) {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);  
  }
  strip.show();
}

/**
 * Nothing is goign on, red 'run' state.
 */
void runLight()
{
  if (runLightState == 0) {
    strip.setBrightness(BRIGHTNESS / 4);
    spread2(WIPE_SPEED, RED);
    runLightState = 1;
    all(RED);
  }
  else {
    strip.setBrightness(BRIGHTNESS / 8);
    spread2(WIPE_SPEED, RED);
    runLightState = 0;
    all(RED);
  }
}

/**
 * We're in reverse turn white. 
 */
void reverseLight()
{
  /*if (reverseState == 0) {
    strip.setBrightness(BRIGHTNESS);
    spread(WIPE_SPEED, WHITE);
    reverseState = 1;  
    all(WHITE);
    strip.show();
  }*/
  
  knightRider(1, WIPE_SPEED*2.2, 2, RED);  
  //clearStrip();
  delay(WIPE_SPEED*2);
}

// Function for larson scan. 
void knightRider(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color) {
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!

  for(int i = 0; i < cycles; i++){
    /*ReverseLightState = analogRead(ReverseSignal);
    float ReverseVoltage = ReverseLightState * (5.0 / 1023.0);  

if (ReverseVoltage < 3.0){
  i = 100
}*/
    
    for (int count = 1; count < NUM_PIXELS; count++) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x>0; x--) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x-1, old_val[x-1]); 
      }
      strip.show();
      delay(speed);
    }
    
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x+1, old_val[x+1]);
      }
      strip.show();
      delay(speed);
    }

  }
}

// Set all LEDs to off/black. 
void clearStrip() {
  for (int i = 0; i<NUM_PIXELS; i++) {
    strip.setPixelColor(i, 0x000000); 
    strip.show();
  }
}

// Dim a color across a width of leds. 
uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
}

// Function to light LEDS from the center one at a time (spreading). 
void spread(uint16_t speed, uint32_t color) {
  clearStrip();
  
  delay(300);
  
  int center = NUM_PIXELS / 2;
  
  for (int x = 0; x < center; x++) {
    strip.setPixelColor(center + x, color); 
    strip.setPixelColor(center + (x*-1), color); 
    strip.show();
    delay(speed);
  }    
}

void spread2(uint16_t speed, uint32_t color) {
  
  delay(300);
  
  int center = NUM_PIXELS / 2;
  
  for (int x = 0; x < center; x++) {
    strip.setPixelColor(center + x, color); 
    strip.setPixelColor(center + (x*-1), color); 
    strip.show();
    delay(speed);
  }    
}
