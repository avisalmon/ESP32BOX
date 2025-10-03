/*
 * ESP32-S3-Touch-LCD Hello World Test
 * 
 * VERSION: 1.0 - Clean approach with local library
 * LAST UPDATED: October 2, 2025
 * 
 * 🎯 GOAL: Display "Hello World" on LCD using TFT_eSPI
 * 
 * LIBRARIES USED:
 * - TFT_eSPI (install from Library Manager)
 * - ESP32_LCD_Drivers (local library in libraries/ folder)
 * 
 * ARDUINO IDE SETUP:
 * Board: ESP32S3 Dev Module
 * Flash Size: 16MB
 * Partition Scheme: 16M Flash (3MB APP/9.9MB FATFS)
 * PSRAM: Enabled
 */

#include <Wire.h>
#include <TFT_eSPI.h>
#include <I2C_Driver.h>
#include <TCA9554PWR.h>
#include <Backlight.h>

// TFT_eSPI instance
TFT_eSPI tft = TFT_eSPI();

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║     ESP32-S3 HELLO WORLD TEST        ║");
    Serial.println("║      Clean Library Approach          ║");
    Serial.println("╚═══════════════════════════════════════╝");
    Serial.println();
    
    // Initialize hardware step by step
    Serial.println("🚀 Initializing hardware...");
    
    Serial.print("  📡 I2C bus... ");
    I2C_Init();
    Serial.println("✅");
    
    Serial.print("  🔌 TCA9554PWR I/O expander... ");
    TCA9554PWR_Init(0x00);  // All pins as outputs
    Serial.println("✅");
    
    Serial.print("  💡 Backlight PWM... ");
    Backlight_Init();
    Serial.println("✅");
    
    Serial.print("  🖥️  Display reset... ");
    resetDisplay();
    Serial.println("✅");
    
    Serial.print("  🎨 TFT_eSPI initialization... ");
    tft.init();
    tft.setRotation(0);  // Portrait mode for 360x360 circular display
    Serial.println("✅");
    
    // Display Hello World
    Serial.println();
    Serial.println("🎨 Drawing Hello World...");
    displayHelloWorld();
    
    Serial.println();
    Serial.println("🎉 SUCCESS! Check your circular display!");
    Serial.println("👀 You should see 'Hello World' text with colorful circles");
}

void loop() {
    // Static display - Hello World stays on screen
    delay(1000);
}

void resetDisplay() {
    // Reset display via TCA9554PWR EXIO2 pin
    Set_EXIO(EXIO_PIN2, Low);   // Assert reset (active low)
    delay(100);
    Set_EXIO(EXIO_PIN2, High);  // Release reset
    delay(120);                 // Wait for display to initialize
}

void displayHelloWorld() {
    // Clear screen to black
    tft.fillScreen(TFT_BLACK);
    
    // Set text color and size
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    
    // Display "Hello" centered
    tft.setTextDatum(MC_DATUM);  // Middle Center
    tft.drawString("Hello", 180, 150);
    
    // Display "World!" below
    tft.drawString("World!", 180, 200);
    
    // Add decorative circles (for 360x360 circular display)
    tft.drawCircle(180, 180, 80, TFT_GREEN);
    tft.drawCircle(180, 180, 100, TFT_BLUE);
    tft.drawCircle(180, 180, 120, TFT_RED);
    tft.drawCircle(180, 180, 140, TFT_YELLOW);
    
    Serial.println("  ✅ Hello World displayed with decorative circles");
    Serial.println("  🎯 Text positioned for 360x360 circular display");
}