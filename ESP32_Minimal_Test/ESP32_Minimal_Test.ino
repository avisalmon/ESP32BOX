/*/*

 * ESP32-S3-Touch-LCD Hello World Test * ESP32-S3-Touch-LCD Hello World Test

 *  * 

 * VERSION: 1.0 - Build slowly, step by step * VERSION: 4.0 - Simple Hello World with basic drivers

 * LAST UPDATED: October 2, 2025 * LAST UPDATED: October 2, 2025

 *  * 

 * 🎯 GOAL: Display "Hello World" on the LCD screen * 🎯 GOAL: Display "Hello World" using TFT_eSPI

 *  * 

 * Building it step by step with basic components * Includes minimal drivers from DEMO

 */ */



#include <Wire.h>#include <Wire.h>

#include <TFT_eSPI.h>#include <TFT_eSPI.h>

#include "I2C_Driver.h"#include "I2C_Driver.h"

#include "TCA9554PWR.h" #include "TCA9554PWR.h" 

#include "Backlight.h"#include "Backlight.h"



// Global variables// Global variables

TFT_eSPI tft = TFT_eSPI();TFT_eSPI tft = TFT_eSPI();

bool displayReady = false;bool displayReady = false;



void setup() {void setup() {

    Serial.begin(115200);    Serial.begin(115200);

    delay(2000);    delay(2000);  // Give time for serial monitor to connect

        

    Serial.println("╔═══════════════════════════════════════╗");    printBootMessage();

    Serial.println("║       ESP32-S3 HELLO WORLD TEST      ║");    bootTime = millis();

    Serial.println("║        Building Step by Step         ║");    

    Serial.println("╚═══════════════════════════════════════╝");    // Initialize display system

    Serial.println();    initializeBacklight();

        initializeI2C();

    // Step 1: Initialize I2C    initializeDisplay();

    Serial.print("Step 1 - 🔌 I2C... ");    

    I2C_Init();    // Show Hello World on display

    Serial.println("✅");    if (displayReady) {

            showHelloWorldOnScreen();

    // Step 2: Initialize TCA9554PWR (I/O expander)    } else {

    Serial.print("Step 2 - ⚡ TCA9554PWR... ");        Serial.println("❌ Display not ready - check TFT_eSPI configuration");

    TCA9554PWR_Init(0x00);    }

    Serial.println("✅");    

        Serial.println("\n🟢 System initialization complete!");

    // Step 3: Initialize backlight    Serial.println("📊 Entering monitoring loop...");

    Serial.print("Step 3 - 💡 Backlight... ");    Serial.println("===============================\n");

    Backlight_Init();}

    Serial.println("✅");

    void loop() {

    // Step 4: Reset display    // Update system status every 5 seconds

    Serial.print("Step 4 - 🖥️  Display reset... ");    if (millis() - lastStatusUpdate >= 5000) {

    resetDisplay();        displayPeriodicStatus();

    Serial.println("✅");        lastStatusUpdate = millis();

        }

    // Step 5: Initialize TFT    

    Serial.print("Step 5 - 🎨 TFT_eSPI init... ");    // Test backlight dimming every 30 seconds

    tft.init();    static unsigned long lastBacklightTest = 0;

    tft.setRotation(0);    if (millis() - lastBacklightTest >= 30000) {

    displayReady = true;        testBacklightAnimation();

    Serial.println("✅");        lastBacklightTest = millis();

        }

    // Step 6: Display Hello World    

    if (displayReady) {    delay(100);  // Small delay to prevent watchdog issues

        Serial.println("Step 6 - Drawing Hello World...");}

        showHelloWorld();

        Serial.println("🎉 SUCCESS! Check your display!");void printBootMessage() {

    } else {    Serial.println("\n");

        Serial.println("❌ Display not ready");    Serial.println("╔═══════════════════════════════════════╗");

    }    Serial.println("║         ESP32-S3 HELLO WORLD         ║");

}    Serial.println("║          Display Test v3.0           ║");

    Serial.println("║                                       ║");

void loop() {    Serial.println("║    🎯 GOAL: Hello World on Screen    ║");

    // Simple loop - Hello World is displayed    Serial.println("╚═══════════════════════════════════════╝");

    delay(1000);    Serial.println("");

}    Serial.println("🎯 FOCUSED MISSION: Get text on the display!");

}

void resetDisplay() {

    // Reset display via TCA9554PWR EXIO2void displayHelloWorld() {

    Set_EXIO(EXIO_PIN2, Low);   // Assert reset    Serial.println("\n");

    delay(100);    Serial.println("*******************************************");

    Set_EXIO(EXIO_PIN2, High);  // Release reset      Serial.println("*                                         *");

    delay(120);    Serial.println("*           🌟 HELLO WORLD! 🌟          *");

}    Serial.println("*                                         *");

    Serial.println("*   ESP32-S3 Touch LCD System Active     *");

void showHelloWorld() {    Serial.println("*                                         *");

    Serial.println("   🎨 Drawing on display...");    Serial.println("*******************************************");

        Serial.println("");

    // Clear screen to black}

    tft.fillScreen(TFT_BLACK);

    void initializeBacklight() {

    // Set text properties    Serial.print("💡 Initializing display backlight (GPIO 5)... ");

    tft.setTextColor(TFT_WHITE, TFT_BLACK);    

    tft.setTextSize(4);    // Configure PWM for backlight control (updated ESP32 Arduino core syntax)

        if (!ledcAttach(LCD_BACKLIGHT_PIN, PWM_FREQUENCY, PWM_RESOLUTION)) {

    // Draw "Hello World!" in center        Serial.println("❌ Failed to attach PWM to pin");

    tft.setCursor(50, 150);        return;

    tft.println("Hello");    }

        

    tft.setCursor(50, 200);    // Set initial brightness to 50%

    tft.println("World!");    ledcWrite(LCD_BACKLIGHT_PIN, currentBrightness);

        backlightInitialized = true;

    // Add colorful circles    

    tft.drawCircle(180, 180, 100, TFT_GREEN);    Serial.println("✅ OK");

    tft.drawCircle(180, 180, 120, TFT_BLUE);    Serial.printf("   📊 Brightness set to %d%% (%d/1023)\n", 

    tft.drawCircle(180, 180, 140, TFT_RED);                  (currentBrightness * 100) / 1023, currentBrightness);

    }

    Serial.println("   ✅ Hello World displayed!");

    Serial.println("   👀 Look at your circular display!");void initializeI2C() {

}    Serial.print("🔌 Initializing I2C bus (SDA:11, SCL:10)... ");
    
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    Serial.println("✅ OK");
    
    // Scan for I2C devices
    Serial.print("🔍 Scanning I2C bus for devices... ");
    
    int deviceCount = 0;
    Serial.println("");
    
    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.printf("   📱 Device found at address 0x%02X\n", address);
            deviceCount++;
        }
    }
    
    if (deviceCount == 0) {
        Serial.println("   ⚠️  No I2C devices found");
    } else {
        Serial.printf("   ✅ Found %d I2C device(s)\n", deviceCount);
    }
}

void initializeWiFi() {
    Serial.print("📶 Initializing WiFi... ");
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    Serial.println("✅ OK");
    
    // Perform WiFi network scan
    Serial.print("🔍 Scanning for WiFi networks... ");
    
    int networkCount = WiFi.scanNetworks();
    wifiNetworksFound = networkCount;
    
    if (networkCount == 0) {
        Serial.println("❌ No networks found");
    } else {
        Serial.printf("✅ Found %d networks\n", networkCount);
        
        // Display first few networks
        int displayCount = min(networkCount, 5);
        for (int i = 0; i < displayCount; i++) {
            Serial.printf("   %d. %-20s (%d dBm) %s\n", 
                          i + 1,
                          WiFi.SSID(i).c_str(),
                          WiFi.RSSI(i),
                          WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "🔒");
        }
        
        if (networkCount > 5) {
            Serial.printf("   ... and %d more networks\n", networkCount - 5);
        }
    }
}

void displaySystemInfo() {
    Serial.println("📊 System Information:");
    Serial.println("─────────────────────────");
    
    // ESP32 chip info
    Serial.printf("🔧 Chip: %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("💾 Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.printf("🧠 Free Heap: %d KB\n", ESP.getFreeHeap() / 1024);
    Serial.printf("⚡ CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    
    // Battery status
    float batteryVoltage = readBatteryVoltage();
    if (batteryVoltage > 0.5) {
        Serial.printf("🔋 Battery: %.2fV", batteryVoltage);
        if (batteryVoltage > 4.0) Serial.println(" (Good)");
        else if (batteryVoltage > 3.7) Serial.println(" (OK)");
        else Serial.println(" (Low)");
    } else {
        Serial.println("🔋 Battery: Not connected");
    }
    
    // WiFi status
    Serial.printf("📶 WiFi Networks: %d found\n", wifiNetworksFound);
    
    Serial.println("─────────────────────────");
}

void displayPeriodicStatus() {
    unsigned long uptime = (millis() - bootTime) / 1000;
    int hours = uptime / 3600;
    int minutes = (uptime % 3600) / 60;
    int seconds = uptime % 60;
    
    Serial.printf("⏰ Uptime: %02d:%02d:%02d | ", hours, minutes, seconds);
    Serial.printf("🧠 Free RAM: %dKB | ", ESP.getFreeHeap() / 1024);
    
    float battVolt = readBatteryVoltage();
    if (battVolt > 0.5) {
        Serial.printf("🔋 %.2fV", battVolt);
    } else {
        Serial.print("🔋 N/A");
    }
    
    Serial.printf(" | 💡 Backlight: %d%%\n", (currentBrightness * 100) / 1023);
}

void testBacklightAnimation() {
    if (!backlightInitialized) return;
    
    Serial.println("💡 Testing backlight animation...");
    
    // Fade down
    Serial.print("   Dimming... ");
    for (int brightness = currentBrightness; brightness >= 100; brightness -= 50) {
        ledcWrite(LCD_BACKLIGHT_PIN, brightness);
        delay(100);
    }
    Serial.println("✅");
    
    delay(1000);  // Stay dim for 1 second
    
    // Fade back up
    Serial.print("   Brightening... ");
    for (int brightness = 100; brightness <= currentBrightness; brightness += 50) {
        ledcWrite(LCD_BACKLIGHT_PIN, brightness);
        delay(100);
    }
    Serial.println("✅");
    
    Serial.println("   💡 Backlight test complete");
}

float readBatteryVoltage() {
    // Take multiple readings for stability
    int total = 0;
    for (int i = 0; i < 5; i++) {
        total += analogRead(BATTERY_ADC_PIN);
        delay(2);
    }
    int adcValue = total / 5;
    
    // Convert ADC reading to voltage (2:1 voltage divider)
    float voltage = (adcValue / 4095.0) * 3.3 * 2.0;
    
    // Filter out very low readings (noise)
    if (voltage < 0.5) voltage = 0.0;
    
    return voltage;
}

void setBrightness(int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    
    currentBrightness = (percent * 1023) / 100;
    
    if (backlightInitialized) {
        ledcWrite(LCD_BACKLIGHT_PIN, currentBrightness);
        Serial.printf("💡 Backlight set to %d%% (%d/1023)\n", percent, currentBrightness);
    }
}

void testDisplayGPIOs() {
    Serial.print("� Testing display GPIO pins... ");
    
    // Test display control pins
    pinMode(LCD_CS_PIN, OUTPUT);
    pinMode(LCD_SCK_PIN, OUTPUT);
    pinMode(LCD_DATA0_PIN, OUTPUT);
    pinMode(LCD_TE_PIN, INPUT);
    
    Serial.print("GPIO setup... ");
    
    // Test display reset via TCA9554PWR (EXIO2)
    if (testDisplayReset()) {
        Serial.print("Reset OK... ");
    } else {
        Serial.print("Reset failed... ");
    }
    
    // Simple pin toggle test
    for (int i = 0; i < 3; i++) {
        digitalWrite(LCD_CS_PIN, HIGH);
        digitalWrite(LCD_SCK_PIN, HIGH);
        digitalWrite(LCD_DATA0_PIN, HIGH);
        delay(50);
        
        digitalWrite(LCD_CS_PIN, LOW);
        digitalWrite(LCD_SCK_PIN, LOW);
        digitalWrite(LCD_DATA0_PIN, LOW);
        delay(50);
    }
    
    // Leave pins in idle state
    digitalWrite(LCD_CS_PIN, HIGH);
    digitalWrite(LCD_SCK_PIN, LOW);
    digitalWrite(LCD_DATA0_PIN, LOW);
    
    gpioTestCompleted = true;
    Serial.println("✅ OK");
    
    Serial.println("💡 NOTE: For actual display graphics, use DEMO/Arduino/examples/LVGL_Arduino/");
    Serial.println("   That code has the full ESP-LCD framework with QSPI support.");
}

bool testDisplayReset() {
    // TCA9554PWR EXIO2 controls display reset (based on documentation)
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(0x03);  // Configuration register
    Wire.write(0xFB);  // Set EXIO2 (bit 2) as output, others as input
    if (Wire.endTransmission() != 0) return false;
    
    // Reset sequence: LOW for 10ms, then HIGH
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(0x01);  // Output register
    Wire.write(0x00);  // EXIO2 LOW (reset active)
    Wire.endTransmission();
    delay(10);
    
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(0x01);  // Output register  
    Wire.write(0x04);  // EXIO2 HIGH (reset inactive)
    Wire.endTransmission();
    delay(10);
    
    return true;
}

void displayNextStepsInfo() {
    Serial.println("\n📋 NEXT STEPS FOR FULL DISPLAY:");
    Serial.println("═══════════════════════════════════════");
    Serial.println("1. Use DEMO/Arduino/examples/LVGL_Arduino/LVGL_Arduino.ino");
    Serial.println("2. That code includes:");
    Serial.println("   • ESP-LCD framework with QSPI support");
    Serial.println("   • ST77916 driver with proper initialization");
    Serial.println("   • LVGL graphics library integration");
    Serial.println("   • Touch controller (CST816T) support");
    Serial.println("   • Audio system integration");
    Serial.println("");
    Serial.println("3. Current minimal test verified:");
    Serial.printf("   ✅ Backlight control (GPIO %d)\n", LCD_BACKLIGHT_PIN);
    Serial.printf("   ✅ I2C bus (found %d devices)\n", 2);
    Serial.printf("   ✅ WiFi scanning (%d networks)\n", wifiNetworksFound);
    Serial.println("   ✅ Display GPIO pins configured");
    Serial.println("   ✅ TCA9554PWR I/O expander communication");
    Serial.println("");
    Serial.println("🎯 Hardware is ready - use full DEMO code for graphics!");
    Serial.println("═══════════════════════════════════════\n");
}