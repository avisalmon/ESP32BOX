/*
 * ESP32-S3-Touch-LCD Hello World Test
 * 
 * VERSION: 1.0 - Based on working DEMO code
 * LAST UPDATED: October 2, 2025
 * 
 * 🎯 GOAL: Display "Hello World" on screen using proven DEMO approach
 * 
 * Based on: DEMO/Arduino/examples/LVGL_Arduino/LVGL_Arduino.ino
 * Simplified to show only "Hello World" text on display
 */

// Include the working drivers from DEMO
#include "Display_ST77916.h"
#include "LVGL_Driver.h"
#include "TCA9554PWR.h"
#include "I2C_Driver.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║       ESP32-S3 HELLO WORLD TEST      ║");
    Serial.println("║      Using Working DEMO Drivers      ║");
    Serial.println("╚═══════════════════════════════════════╝");
    Serial.println();
    
    // Initialize hardware (same sequence as working DEMO)
    Serial.println("🚀 Initializing hardware...");
    
    Serial.print("📡 I2C... ");
    I2C_Init();
    Serial.println("✅");
    
    Serial.print("🔌 TCA9554PWR... ");
    TCA9554PWR_Init(0x00);
    Serial.println("✅");
    
    Serial.print("💡 Backlight... ");
    Backlight_Init();
    Serial.println("✅");
    
    Serial.print("🖥️  Display... ");
    LCD_Init();
    Serial.println("✅");
    
    Serial.print("🎨 LVGL... ");
    Lvgl_Init();
    Serial.println("✅");
    
    // The LVGL_Init() function already creates a "Hello Arduino and LVGL!" label
    // Let's add our own "Hello World" message
    createHelloWorldDisplay();
    
    Serial.println();
    Serial.println("🎉 SUCCESS! Check your display screen!");
    Serial.println("👀 You should see 'Hello World' text!");
    Serial.println();
}

void loop() {
    // Keep LVGL running (same as working DEMO)
    Lvgl_Loop();
    delay(5);
}

void createHelloWorldDisplay() {
    Serial.println("✨ Creating Hello World display...");
    
    // Clear screen and create new content
    lv_obj_clean(lv_scr_act());
    
    // Create main "Hello World" label
    lv_obj_t *hello_label = lv_label_create(lv_scr_act());
    lv_label_set_text(hello_label, "Hello World!");
    lv_obj_set_style_text_font(hello_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(hello_label, lv_color_white(), 0);
    lv_obj_align(hello_label, LV_ALIGN_CENTER, 0, -30);
    
    // Create subtitle
    lv_obj_t *subtitle = lv_label_create(lv_scr_act());
    lv_label_set_text(subtitle, "ESP32-S3 Display Working!");
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0x00FF00), 0);
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 20);
    
    // Add decorative circle
    lv_obj_t *circle = lv_obj_create(lv_scr_act());
    lv_obj_set_size(circle, 200, 200);
    lv_obj_set_style_radius(circle, 100, 0);
    lv_obj_set_style_bg_opa(circle, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(circle, lv_color_hex(0x0080FF), 0);
    lv_obj_set_style_border_width(circle, 3, 0);
    lv_obj_align(circle, LV_ALIGN_CENTER, 0, 0);
    
    Serial.println("   🎨 Hello World display created!");
}