/*
 * ESP32-S3-Touch-LCD Hello World Display Demo
 * 
 * VERSION: 3.0 - Simple Display Hello World
 * LAST UPDATED: October 3, 2025
 * 
 * 🎯 GOAL: Show "Hello World" on the 360x360 circular display
 * 
 * Uses drivers copied from DEMO/Arduino/examples/LVGL_Arduino/
 */

#include <Wire.h>
#include <lvgl.h>
#include "I2C_Driver.h"
#include "TCA9554PWR.h" 
#include "Backlight.h"
#include "Display_ST77916.h"
#include "LVGL_Driver.h"

void setup() {
    // Serial for debugging
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║     ESP32-S3 HELLO WORLD DISPLAY     ║");
    Serial.println("║         360x360 Circular LCD         ║");
    Serial.println("╚═══════════════════════════════════════╗");
    Serial.println();
    
    // Initialize hardware using official drivers
    Serial.println("🔧 Initializing Hardware...");
    
    // Step 1: I2C and power management
    Serial.println("   1. I2C and I/O Expander...");
    I2C_Init();                    // Initialize I2C bus
    TCA9554PWR_Init(0x00);         // Initialize I/O expander (all outputs low)
    
    // Step 2: Backlight
    Serial.println("   2. Display backlight...");
    Backlight_Init();              // Initialize PWM backlight
    Set_Backlight(80);             // Set to 80% brightness
    
    // Step 3: Display initialization
    Serial.println("   3. ST77916 display driver...");
    LCD_Init();                    // Initialize ST77916 QSPI display
    
    // Step 4: LVGL graphics library
    Serial.println("   4. LVGL graphics system...");
    Lvgl_Init();                   // Initialize LVGL with display driver
    
    // Step 5: Create Hello World
    Serial.println("   5. Creating Hello World label...");
    createSimpleHelloWorld();
    
    Serial.println();
    Serial.println("🎉 Success! Check the display screen!");
    Serial.println("📺 You should see 'Hello World' centered on screen");
}

void createSimpleHelloWorld() {
    // Create a simple label with Hello World text
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello World!\n\nESP32-S3\n360x360 Display\n\nBasic Demo v3.0");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    
    // Make text a bit larger if possible
    static lv_style_t style_large;
    lv_style_init(&style_large);
    lv_style_set_text_font(&style_large, &lv_font_montserrat_16);
    lv_obj_add_style(label, &style_large, 0);
    
    Serial.println("      ✅ Hello World label created and styled!");
}

void loop() {
    // LVGL task handler - must be called regularly
    lv_timer_handler();
    
    // Simple heartbeat every 5 seconds
    static unsigned long lastPrint = 0;
    static int counter = 0;
    
    if (millis() - lastPrint >= 5000) {
        counter++;
        Serial.printf("💓 Display Heartbeat #%d - Running %lu seconds\n", 
                      counter, millis() / 1000);
        lastPrint = millis();
    }
    
    delay(5);  // Small delay for LVGL timing
}