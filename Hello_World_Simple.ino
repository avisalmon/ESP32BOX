/*
 * ESP32-S3-Touch-LCD Hello World Test
 * 
 * SIMPLE VERSION - Uses existing DEMO drivers
 * 
 * 🎯 GOAL: Display Hello World using proven DEMO code
 * 
 * This file goes in: DEMO/Arduino/examples/LVGL_Arduino/
 * Run this instead of the complex examples
 */

#include "Display_ST77916.h"
#include "LVGL_Driver.h"
#include "TCA9554PWR.h"
#include "I2C_Driver.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("ESP32-S3 Hello World Test");
    Serial.println("Using DEMO drivers");
    Serial.println("========================");
    
    // Use exact same init sequence as working DEMO
    Serial.print("I2C... ");
    I2C_Init();
    Serial.println("OK");
    
    Serial.print("TCA9554PWR... ");
    TCA9554PWR_Init(0x00);
    Serial.println("OK");
    
    Serial.print("Backlight... ");
    Backlight_Init();
    Serial.println("OK");
    
    Serial.print("Display... ");
    LCD_Init();
    Serial.println("OK");
    
    Serial.print("LVGL... ");
    Lvgl_Init();
    Serial.println("OK");
    
    // The Lvgl_Init() already shows "Hello Arduino and LVGL!" 
    // Let's add our own message
    createSimpleHelloWorld();
    
    Serial.println();
    Serial.println("SUCCESS! Check your display!");
}

void loop() {
    // Same as working DEMO
    Lvgl_Loop();
    delay(5);
}

void createSimpleHelloWorld() {
    // Clear screen and create simple Hello World
    lv_obj_clean(lv_scr_act());
    
    // Create Hello World label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello World!");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    
    Serial.println("Hello World created on display!");
}