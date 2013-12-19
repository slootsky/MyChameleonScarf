#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_NeoPixel.h>
/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */
   
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, 6, NEO_GRB + NEO_KHZ800);

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

byte gammatable[256];
// float intensity = 1;
float base = 65535;

void setup(void) {
  Serial.begin(9600);
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
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
  
  
  // Now we're ready to get readings!
}

void loop(void) {
  uint16_t red, green, blue, clear, colorTemp, lux;
  
  tcs.getRawData(&red, &green, &blue, &clear);
  colorTemp = tcs.calculateColorTemperature(red, green, blue);
  lux = tcs.calculateLux(red, green, blue);
  
  float r, g, b;

/* works pretty well   
  r = red / (float)clear;
  g = green / (float)clear;
  b = blue / (float)clear;
  */

  r = red / base;
  g = green / base;
  b = blue / base;
  /*
  r = lux / (float)red;
  g = lux / (float)green;
  b = lux / (float)blue;
  */
  r *= 255; g *= 255; b *= 255;
  
  Serial.print("R: "); 
  serialPrintNumber(red,5);
  
  Serial.print("\tG: "); 
  serialPrintNumber(green,5);
  Serial.print(" ");
  
  Serial.print("\tB: "); 
  serialPrintNumber(blue,5);
  Serial.print(" ");
  Serial.print("\tC: "); 
  serialPrintNumber(clear,5);
  Serial.print(" ");
  Serial.print("\tColor Temp: "); 
  serialPrintNumber(colorTemp,5);
  Serial.print(" K - ");
  Serial.print("\tLux: "); 
  serialPrintNumber(lux,5);
  Serial.print(" - ");
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

  
/*  r = red / (float)clear;
  g = green / (float)clear;
  b = blue / (float)clear;
  */
/*  float max=red;
  if ( blue > max )
    max = blue;
  if ( green > max )
    max = green;
      
  r = (red/max)*255;
  g = (green/max)*255;
  b = (blue/max)*255;
  Serial.print("R: "); Serial.print(int(r), DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(int(g), DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(int(b), DEC); Serial.print(" ");
//  Serial.print("Ratio: "); Serial.print(ratio, DEC); Serial.print(" ");
  Serial.println(" ");
  */
//  colorWipe(strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]), 0);
  colorFill( (int)(gammatable[int(r)]*intensity)
          , (int)(gammatable[int(g)]*intensity)
          , (int)(gammatable[int(b)]*intensity) 
          );

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

// Fill the dots one after the other with a color
void colorFade(uint8_t r, uint8_t g,uint8_t b, uint8_t wait) 
{
  uint32_t c;
  
  for(uint16_t i=1; i<(strip.numPixels()+1); i++) {
    c=strip.Color
      ( (int)( (r*i)/( strip.numPixels() ) )
      , (int)( (g*i)/( strip.numPixels() ) )
      , (int)( (b*i)/( strip.numPixels() ) )
      );
      strip.setPixelColor(i, c);
      if ( wait > 0 ) {
        strip.show();
        delay(wait);
      }
  }
  if ( wait == 0 ) {
    strip.show();
  }
}

void serialPrintNumber(uint16_t number, int width)
{
  int i;
  for ( i = width ; i > log10(number) ; i-- )
    Serial.print(" ");
  Serial.print(number, DEC); 
}


