#ifndef Display_h
#define Display_h

//for 0.96" OLED display ssd1306 clone from ebay
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2 
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
Adafruit_SSD1306 display(OLED_RESET);

void initDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C 
  display.display();
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, Nick. It's nice to see you.");
  display.display();  
  display.clearDisplay();
}

#endif
