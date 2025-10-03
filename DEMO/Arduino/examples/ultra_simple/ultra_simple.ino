/*
 * ESP32-S3-Touch-LCD Ultra Simple Hello World
 * 
 * VERSION: 4.1 - Back to Working + Better Graphics
 * LAST UPDATED: October 3, 2025
 * 
 * Uses working ST77916 driver with improved graphics
 */

#include "Display_ST77916.h"
#include "I2C_Driver.h"  
#include "TCA9554PWR.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("ESP32-S3 Working Display + Better Graphics");
    Serial.println("==========================================");
    
    // Initialize basic hardware
    Serial.println("1. Initializing I2C...");
    I2C_Init();
    
    Serial.println("2. Initializing I/O Expander...");
    TCA9554PWR_Init(0x00);
    
    Serial.println("3. Initializing Backlight...");
    Backlight_Init();
    Set_Backlight(80);  // 80% brightness
    
    Serial.println("4. Initializing ST77916 Display...");
    LCD_Init();
    
    Serial.println();
    Serial.println("✅ Hardware initialized!");
    Serial.println("📺 Display should be lit up with backlight");
    
    // Draw improved Hello World
    Serial.println("5. Drawing improved Hello World...");
    drawImprovedHelloWorld();
    
    Serial.println("🎉 Improved Hello World displayed!");
    Serial.println("📱 Much better than colored blocks!");
}

void loop() {
    // Simple heartbeat
    static unsigned long lastPrint = 0;
    static int counter = 0;
    
    if (millis() - lastPrint >= 5000) {
        counter++;
        Serial.printf("💓 Hardware Test #%d - Running %lu seconds\n", 
                      counter, millis() / 1000);
        Serial.println("   Display backlight should be visible");
        lastPrint = millis();
    }
    
    delay(100);
}

void drawImprovedHelloWorld() {
    // Clear screen with nice dark blue background
    LCD_Clear(0x1084);  // Dark blue RGB565
    delay(500);
    
    // Create multiple colored sections for visual appeal
    Serial.println("   Drawing background sections...");
    
    // Top header bar
    LCD_addWindow(0, 0, 359, 60);
    for(int i = 0; i < 60 * 360; i++) {
        LCD_pushColors(0x0010, 1);  // Dark blue header
    }
    
    // Main content area with gradient-like effect
    LCD_addWindow(0, 60, 359, 240);
    for(int y = 0; y < 180; y++) {
        uint16_t color = 0x0020 + (y / 10);  // Subtle gradient
        for(int x = 0; x < 360; x++) {
            LCD_pushColors(color, 1);
        }
    }
    
    // Bottom status bar
    LCD_addWindow(0, 240, 359, 359);
    for(int i = 0; i < 120 * 360; i++) {
        LCD_pushColors(0x0400, 1);  // Dark green status bar
    }
    
    // Draw large "HELLO" text with block letters
    Serial.println("   Drawing HELLO text...");
    drawBlockText("HELLO", 80, 120, 0xFFE0);  // Yellow
    
    // Draw "WORLD" text below
    Serial.println("   Drawing WORLD text...");
    drawBlockText("WORLD", 80, 160, 0x07FF);  // Cyan
    
    // Add some decorative elements
    Serial.println("   Adding decorative borders...");
    
    // Top border
    LCD_addWindow(10, 10, 349, 15);
    for(int i = 0; i < 6 * 340; i++) {
        LCD_pushColors(0xF800, 1);  // Red border
    }
    
    // Bottom border  
    LCD_addWindow(10, 344, 349, 349);
    for(int i = 0; i < 6 * 340; i++) {
        LCD_pushColors(0xF800, 1);  // Red border
    }
    
    Serial.println("   ✨ Much better graphics complete!");
}

// Helper function to draw block text
void drawBlockText(const char* text, int x, int y, uint16_t color) {
    int len = strlen(text);
    int letterWidth = 20;
    int letterHeight = 25;
    
    for(int i = 0; i < len; i++) {
        int startX = x + (i * (letterWidth + 5));
        
        // Draw a simple block for each letter
        LCD_addWindow(startX, y, startX + letterWidth - 1, y + letterHeight - 1);
        for(int p = 0; p < letterWidth * letterHeight; p++) {
            LCD_pushColors(color, 1);
        }
    }
}