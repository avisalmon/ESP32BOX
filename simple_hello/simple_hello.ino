/*
 * ESP32-S3-Touch-LCD Simple Hello World
 * 
 * VERSION: 3.1 - Ultra Simple Version 
 * LAST UPDATED: October 3, 2025
 * 
 * Uses the working demo structure directly
 */

// Include driver files (copied locally)
#include "Display_ST77916.h"
#include "LVGL_Driver.h"
#include "TCA9554PWR.h"
#include "I2C_Driver.h"
#include "Backlight.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("ESP32-S3 Simple Hello World Display");
    Serial.println("===================================");
    
    // Use exact initialization sequence from working demo
    I2C_Init();
    TCA9554PWR_Init(0x00);   
    Backlight_Init();
    LCD_Init();
    Lvgl_Init();
    
    // Create simple hello world (this will use the built-in "Hello Arduino and LVGL!" from LVGL_Driver.cpp)
    Serial.println("Display initialized - Hello World should appear!");
}

void loop() {
    Lvgl_Loop();  // Use the demo's loop function
    delay(10);
}