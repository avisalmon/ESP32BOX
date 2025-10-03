/*
 * ESP32-S3-Touch-LCD with TFT_eSPI Graphics
 * 
 * VERSION: 5.0 - Professional Text and Graphics
 * LAST UPDATED: October 3, 2025
 * 
 * Uses TFT_eSPI library for proper fonts and graphics
 */

#include <TFT_eSPI.h>

// Initialize TFT_eSPI for 360x360 circular display
TFT_eSPI tft = TFT_eSPI();

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("ESP32-S3 TFT_eSPI Graphics Demo");
    Serial.println("================================");
    
    // Initialize the display
    Serial.println("1. Initializing TFT display...");
    tft.init();
    tft.setRotation(0);  // Portrait orientation
    
    // Clear screen with black background
    tft.fillScreen(TFT_BLACK);
    
    // Display "Hello World" with proper fonts
    drawHelloWorld();
    
    Serial.println("🎉 Professional Hello World displayed!");
    Serial.println("📱 Check the display for proper text and graphics");
}

void drawHelloWorld() {
    // Set text properties
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  // White text on black background
    tft.setTextSize(2);                      // 2x size
    
    // Draw title
    tft.setCursor(50, 50);
    tft.println("ESP32-S3");
    
    // Draw main message
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(3);
    tft.setCursor(30, 120);
    tft.println("Hello");
    tft.setCursor(30, 160);
    tft.println("World!");
    
    // Draw some graphics
    tft.drawRect(20, 40, 320, 180, TFT_WHITE);     // Border
    tft.fillCircle(180, 250, 30, TFT_RED);         // Red circle
    tft.fillTriangle(100, 280, 130, 320, 70, 320, TFT_GREEN); // Green triangle
    tft.fillRect(200, 280, 60, 40, TFT_BLUE);      // Blue rectangle
    
    // Add status text
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(1);
    tft.setCursor(50, 300);
    tft.println("TFT_eSPI Graphics Working!");
}

void loop() {
    // Simple color animation
    static unsigned long lastUpdate = 0;
    static int colorIndex = 0;
    uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_MAGENTA, TFT_CYAN};
    
    if (millis() - lastUpdate >= 1000) {
        // Change circle color every second
        tft.fillCircle(180, 250, 30, colors[colorIndex]);
        colorIndex = (colorIndex + 1) % 6;
        lastUpdate = millis();
    }
    
    delay(50);
}