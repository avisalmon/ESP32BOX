/*
 * ESP32-S3-Touch-LCD Minimal Display Demo
 * 
 * Based on DEMO/Arduino/examples/LVGL_Arduino/ but without audio components
 * This should compile and show "Hello Arduino and LVGL!" on the display
 */

#include "Display_ST77916.h"
#include "LVGL_Driver.h"
#include "I2C_Driver.h"
#include "TCA9554PWR.h"

void Driver_Init()
{
    I2C_Init();
    TCA9554PWR_Init(0x00);   
    Backlight_Init();
}

void setup()
{
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("ESP32-S3 Minimal Display Demo");
    Serial.println("==============================");
    
    // Initialize hardware (without audio, SD, mic, etc.)
    Driver_Init();
    LCD_Init();
    Lvgl_Init();  // This creates the "Hello Arduino and LVGL!" label automatically
    
    Serial.println("Display initialized!");
    Serial.println("Check screen for 'Hello Arduino and LVGL!' message");
}

void loop() {
    Lvgl_Loop();
    delay(5);
}