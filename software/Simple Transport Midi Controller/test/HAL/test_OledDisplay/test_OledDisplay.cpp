/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

 #include <SPI.h>
 #include <Wire.h>
 #include <Adafruit_GFX.h>
 #include <Adafruit_SSD1306.h>
 #include <LEDRingSmall.h>
 
 #define SCREEN_WIDTH 128 // OLED display width, in pixels
 #define SCREEN_HEIGHT 64 // OLED display height, in pixels
 
 // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
 // The pins for I2C are defined by the Wire-library. 
 // On an arduino UNO:       A4(SDA), A5(SCL)
 // On an arduino MEGA 2560: 20(SDA), 21(SCL)
 // On an arduino LEONARDO:   2(SDA),  3(SCL), ...
 #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
 #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
 

 #define SCREEN2_WIDTH 128 // OLED display width, in pixels
#define SCREEN2_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
// Remember: pins 6-11 are ussed for flash spi
#define OLED2_DC    27
#define OLED2_CS    33
#define OLED2_RESET 15
Adafruit_SSD1306 displayLarge(SCREEN2_WIDTH, SCREEN2_HEIGHT,
  MOSI, SCK, OLED2_DC, OLED2_RESET, OLED2_CS);


 #define NUMFLAKES     10 // Number of snowflakes in the animation example
 
 #define LOGO_HEIGHT   16
 #define LOGO_WIDTH    16
 static const unsigned char PROGMEM logo_bmp[] =
 { 0b00000000, 0b11000000,
   0b00000001, 0b11000000,
   0b00000001, 0b11000000,
   0b00000011, 0b11100000,
   0b11110011, 0b11100000,
   0b11111110, 0b11111000,
   0b01111110, 0b11111111,
   0b00110011, 0b10011111,
   0b00011111, 0b11111100,
   0b00001101, 0b01110000,
   0b00011011, 0b10100000,
   0b00111111, 0b11100000,
   0b00111111, 0b11110000,
   0b01111100, 0b11110000,
   0b01110000, 0b01110000,
   0b00000000, 0b00110000 };
 
   void testdrawline() {
    int16_t i;
  
    display.clearDisplay(); // Clear display buffer
  
    for(i=0; i<display.width(); i+=4) {
      display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
      display.display(); // Update screen with each newly-drawn line
      delay(1);
    }
    for(i=0; i<display.height(); i+=4) {
      display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
      display.display();
      delay(1);
    }
    delay(250);
  
    display.clearDisplay();
  
    for(i=0; i<display.width(); i+=4) {
      display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
      display.display();
      delay(1);
    }
    for(i=display.height()-1; i>=0; i-=4) {
      display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
      display.display();
      delay(1);
    }
    delay(250);
  
    display.clearDisplay();
  
    for(i=display.width()-1; i>=0; i-=4) {
      display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
      display.display();
      delay(1);
    }
    for(i=display.height()-1; i>=0; i-=4) {
      display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
      display.display();
      delay(1);
    }
    delay(250);
  
    display.clearDisplay();
  
    for(i=0; i<display.height(); i+=4) {
      display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
      display.display();
      delay(1);
    }
    for(i=0; i<display.width(); i+=4) {
      display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
      display.display();
      delay(1);
    }
  
    delay(2000); // Pause for 2 seconds
  }
 
#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2
  void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
    int8_t f, icons[NUMFLAKES][3];
  
    // Initialize 'snowflake' positions
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][XPOS]   = random(1 - LOGO_WIDTH, displayLarge.width());
      icons[f][YPOS]   = -LOGO_HEIGHT;
      icons[f][DELTAY] = random(1, 6);
      Serial.print(F("x: "));
      Serial.print(icons[f][XPOS], DEC);
      Serial.print(F(" y: "));
      Serial.print(icons[f][YPOS], DEC);
      Serial.print(F(" dy: "));
      Serial.println(icons[f][DELTAY], DEC);
    }
  
    for(;;) { // Loop forever...
      displayLarge.clearDisplay(); // Clear the display buffer
  
      // Draw each snowflake:
      for(f=0; f< NUMFLAKES; f++) {
        displayLarge.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
      }
  
      displayLarge.display(); // Show the display buffer on the screen
      delay(200);        // Pause for 1/10 second
  
      // Then update coordinates of each flake...
      for(f=0; f< NUMFLAKES; f++) {
        icons[f][YPOS] += icons[f][DELTAY];
        // If snowflake is off the bottom of the screen...
        if (icons[f][YPOS] >= displayLarge.height()) {
          // Reinitialize to a random position, just off the top
          icons[f][XPOS]   = random(1 - LOGO_WIDTH, displayLarge.width());
          icons[f][YPOS]   = -LOGO_HEIGHT;
          icons[f][DELTAY] = random(1, 6);
        }
      }
    }}
  void setup() {
    bool I2C1_INIT = Wire1.setPins(SDA,SCL);
    Serial.begin(115200);

    pinMode(SDA, OUTPUT);
    Serial.println("Asserting I2C pins high \n");
    digitalWrite(SDA, HIGH);
    Wire1.begin();
    Serial.println("Communication with device succeeded");
 
   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
     Serial.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever
   }
   else {Serial.println(F("I2C ok"));}
   
   if(!displayLarge.begin(SSD1306_SWITCHCAPVCC,(uint8_t)0u, true)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  else {Serial.println(F("SPI ok"));}
 
   // Show initial display buffer contents on the screen --
   // the library initializes this with an Adafruit splash screen.
   display.display();
   displayLarge.display();
   delay(2000); // Pause for 2 seconds
 
   // Clear the buffer
   display.clearDisplay();
   displayLarge.clearDisplay();

   // Draw a single pixel in white
   display.drawPixel(10, 10, SSD1306_WHITE);
   displayLarge.drawPixel(10, 10, SSD1306_WHITE);
 
   // Show the display buffer on the screen. You MUST call display() after
   // drawing commands to make them visible on screen!
   display.display();
   displayLarge.display();

   delay(2000);
   // display.display() is NOT necessary after every single drawing command,
   // unless that's what you want...rather, you can batch up a bunch of
   // drawing operations and then update the screen all at once by calling
   // display.display(). These examples demonstrate both approaches...
 
   testdrawline();      // Draw many lines
 
   /* testdrawrect();      // Draw rectangles (outlines)
 
   testfillrect();      // Draw rectangles (filled)
 
   testdrawcircle();    // Draw circles (outlines)
 
   testfillcircle();    // Draw circles (filled)
 
   testdrawroundrect(); // Draw rounded rectangles (outlines)
 
   testfillroundrect(); // Draw rounded rectangles (filled)
 
   testdrawtriangle();  // Draw triangles (outlines)
 
   testfilltriangle();  // Draw triangles (filled)
 
   testdrawchar();      // Draw characters of the default font
 
   testdrawstyles();    // Draw 'stylized' characters
 
   testscrolltext();    // Draw scrolling text
 
   testdrawbitmap();    // Draw a small bitmap image
  */
   // Invert and restore display, pausing in-between
   display.invertDisplay(true);
   displayLarge.invertDisplay(true);
   delay(1000);
   display.invertDisplay(false);
   displayLarge.invertDisplay(false);
   delay(1000);

   testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
   /* testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps */
 }
 
 void loop() {
 }
 
