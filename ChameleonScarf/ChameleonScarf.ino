#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN  6
#define NUMBER_OF_LEDS  8
#define NUMBER_OF_SAMPLES 10

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */
   
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();


/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_LEDS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

byte gammatable[256];
float base = 65535;
float red, green, blue, clear;

void setup(void) {
  uint32_t startupColours[3];
  uint16_t redReading, greenReading, blueReading, clearReading;
  float r, g, b;

  Serial.begin(9600);
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
    tcs.setInterrupt(true);      // turn off LED
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
    //Serial.println(gammatable[i]);
  }
  
   startupColours[0] = strip.Color(188, 188, 188);  // DIM WHITE
   startupColours[1] = strip.Color(188, 0, 0);  // RED
  startupColours[2] = strip.Color(188, 188, 0);  // YELLOW
  startupColours[3] = strip.Color(0, 188, 0);  // GREEN

   for (int i=0; i<3; i++){ //this sequence flashes the first pixel three times as a countdown to the color reading. 
                           // now in a traffic light pattern
      
    strip.setPixelColor (0, startupColours[i]); 
    strip.show();
    tcs.setInterrupt(false);      // turn on LED
    delay(1000);
    strip.setPixelColor (0, strip.Color(0, 0, 0));
    strip.show();
    tcs.setInterrupt(true);      // turn off LED
    delay(500);
  }
    strip.setPixelColor (0, startupColours[3]);
 
  // Now we're ready to get readings!
  for ( float reading = 0 ; reading < NUMBER_OF_SAMPLES ; reading++ )
  {
    /* This would be better as an actual average
      , not this fake rolling average
      */
    tcs.setInterrupt(false);      // turn on LED
    if ( reading == 0 ) 
    {
      tcs.getRawData(&redReading, &greenReading, &blueReading, &clearReading);
      tcs.setInterrupt(true);      // turn off LED

      red = redReading ;
      green = greenReading ;
      blue = blueReading;
    }
    else
    {
      tcs.getRawData(&redReading, &greenReading, &blueReading, &clearReading);
      tcs.setInterrupt(true);      // turn off LED

      red = ( red + redReading ) / 2;
      green = ( green + greenReading ) / 2;
      blue = ( blue + blueReading ) / 2;
      clear = ( clear + clearReading ) / 2;
    }
    r = red / base;
    g = green / base;
    b = blue / base;
    r *= 255; g *= 255; b *= 255;

  strip.setPixelColor( NUMBER_OF_LEDS - ( NUMBER_OF_LEDS * ( reading / NUMBER_OF_SAMPLES ) )
          , strip.Color( (int)(gammatable[int(r)])
          , (int)(gammatable[int(g)])
          , (int)(gammatable[int(b)]) 
          )
          );
    strip.show();
    
    }

//  }


  Serial.print("R: "); 
  serialPrintNumber(red,5);
  Serial.print(" ");
  
  Serial.print("\tG: "); 
  serialPrintNumber(green,5);
  Serial.print(" ");
  
  Serial.print("\tB: "); 
  serialPrintNumber(blue,5);
  Serial.print(" ");

  Serial.print("\tC: "); 
  serialPrintNumber(clear,5);
  Serial.print(" ");

/*  Serial.print("\tColor Temp: "); 
  serialPrintNumber(colorTemp,5);
  Serial.print(" K - ");

  Serial.print("\tLux: "); 
  serialPrintNumber(lux,5);
  Serial.print(" - ");
*/
  Serial.println(" ");

  Serial.print("R: "); 
  serialPrintNumber(int(r),5);
  Serial.print(" ");

  Serial.print("\tG: "); 
  serialPrintNumber(int(g),5);
  Serial.print(" ");

  Serial.print("\tB: "); 
  serialPrintNumber(int(b),5);
  Serial.print(" ");

  Serial.println(" ");

  colorFill( (int)(gammatable[int(r)])
          , (int)(gammatable[int(g)])
          , (int)(gammatable[int(b)]) 
          );
}

void loop(void) {
    //loop is empty because it only takes the color reading once on power up! Turn the scarf off and on again to change the color.
}


// Fill the dots one after the other with a color
void colorFill( uint8_t r, uint8_t g, uint8_t b )  
{
  uint32_t c=strip.Color(r,g,b);
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

void serialPrintNumber(uint16_t number, int width)
{
  int i;
  for ( i = width ; i > log10(number) ; i-- )
    Serial.print(" ");
  Serial.print(number, DEC); 
}


