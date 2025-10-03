/*
 * ESP32-S3-Touch-LCD Hello World Test
 * 
 * VERSION: 4.0 - Simple Hello World with basic drivers
 * LAST UPDATED: October 2, 2025
 * 
 * 🎯 GOAL: Display "Hello World" using TFT_eSPI
 * 
 * Includes minimal drivers from DEMO
 */

#include <Wire.h>
#include <TFT_eSPI.h>
#include "I2C_Driver.h"
#include "TCA9554PWR.h" 
#include "Backlight.h"

// Global variables
TFT_eSPI tft = TFT_eSPI();
bool displayReady = false;

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║       ESP32-S3 HELLO WORLD TEST      ║");
    Serial.println("║         TFT_eSPI Approach v4.0       ║");
    Serial.println("╚═══════════════════════════════════════╝");
    Serial.println();
    
    // Initialize hardware
    Serial.print("🔌 I2C... ");
    I2C_Init();
    Serial.println("✅");
    
    Serial.print("⚡ TCA9554PWR... ");
    TCA9554PWR_Init(0x00);
    Serial.println("✅");
    
    Serial.print("💡 Backlight... ");
    Backlight_Init();
    Serial.println("✅");
    
    Serial.print("🖥️  Display reset... ");
    resetDisplay();
    Serial.println("✅");
    
    Serial.print("🎨 TFT_eSPI init... ");
    tft.init();
    tft.setRotation(0);
    displayReady = true;
    Serial.println("✅");
    
    if (displayReady) {
        showHelloWorld();
        Serial.println("🎉 SUCCESS! Check your display!");
    }
}

void loop() {
    // Simple loop - Hello World is displayed
    delay(1000);
}

void resetDisplay() {
    // Reset display via TCA9554PWR EXIO2
    Set_EXIO(EXIO_PIN2, Low);   // Assert reset
    delay(100);
    Set_EXIO(EXIO_PIN2, High);  // Release reset  
    delay(120);
}

void showHelloWorld() {
    Serial.println("🎨 Drawing Hello World on display...");
    
    // Clear screen to black
    tft.fillScreen(TFT_BLACK);
    
    // Set text properties
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(4);
    
    // Draw "Hello World!" in center
    tft.setCursor(50, 150);
    tft.println("Hello");
    
    tft.setCursor(50, 200);
    tft.println("World!");
    
    // Add colorful circle
    tft.drawCircle(180, 180, 100, TFT_GREEN);
    tft.drawCircle(180, 180, 120, TFT_BLUE);
    tft.drawCircle(180, 180, 140, TFT_RED);
    
    Serial.println("✅ Hello World displayed!");
    Serial.println("👀 Look at your circular display!");
}