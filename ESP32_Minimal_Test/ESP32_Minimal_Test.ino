/*
 * ESP32-S3-Touch-LCD Minimal Working Example
 * 
 * This version focuses on core functionality:
 * - System info display
 * - WiFi scanning  
 * - Battery monitoring
 * - Basic backlight control
 * - Minimal touch testing (no aggressive polling)
 */

#include <WiFi.h>
#include <SD.h>
#include <Wire.h>

// Pin definitions - Testing multiple backlight pins
#define BATTERY_ADC_PIN 4
#define SD_CS           10
#define LCD_BACKLIGHT_TEST   45  // Original guess
#define LCD_BACKLIGHT_ALT1   2   // Common backlight pin
#define LCD_BACKLIGHT_ALT2   15  // Another possibility
#define LCD_BACKLIGHT_ALT3   46  // Power management pin

// Touch pins (testing only, no continuous polling)
#define TOUCH_I2C_SCL   8
#define TOUCH_I2C_SDA   3
#define TOUCH_INT       9
#define TOUCH_RST       33

// Global variables
unsigned long lastUpdate = 0;
unsigned long startTime = 0;
int wifiNetworks = 0;
bool sdCardPresent = false;
bool touchTested = false;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=================================");
    Serial.println("ESP32-S3-Touch-LCD Minimal Test");
    Serial.println("=================================");
    
    startTime = millis();
    
    // Initialize hardware (without problematic touch polling)
    initializeHardware();
    
    Serial.println("\nSystem ready! Monitoring...");
    Serial.println("---------------------------------");
}

void loop() {
    // Do absolutely nothing
    delay(1000);
}

void initializeHardware() {
    Serial.println("Initializing hardware...");
    
    // Test multiple possible backlight pins
    Serial.println("Testing possible backlight pins...");
    
    int backlightPins[] = {45, 2, 15, 46};
    String pinNames[] = {"Pin 45 (original)", "Pin 2 (common)", "Pin 15 (alt)", "Pin 46 (power)"};
    
    for (int i = 0; i < 4; i++) {
        Serial.printf("\nTesting %s...\n", pinNames[i].c_str());
        
        pinMode(backlightPins[i], OUTPUT);
        
        // Turn on
        digitalWrite(backlightPins[i], HIGH);
        Serial.printf("  %s set HIGH - look for screen change", pinNames[i].c_str());
        delay(2000);  // Long delay to see the change
        
        // Blink test
        Serial.printf("\n  Blinking %s (watch for screen changes)...\n", pinNames[i].c_str());
        for (int blink = 0; blink < 5; blink++) {
            digitalWrite(backlightPins[i], LOW);
            Serial.print("    OFF ");
            delay(500);
            digitalWrite(backlightPins[i], HIGH);
            Serial.print("ON ");
            delay(500);
        }
        
        Serial.printf("\n  %s test complete\n", pinNames[i].c_str());
        
        // Leave it on for now
        digitalWrite(backlightPins[i], HIGH);
    }
    
    Serial.println("\n=== BACKLIGHT PIN TEST RESULTS ===");
    Serial.println("Which pin made the screen brighter/darker?");
    Serial.println("Pin 45, 2, 15, or 46?");
    Serial.println("=====================================");
    
    Serial.println("\nSkipping SD/WiFi tests to focus on backlight debugging");
    Serial.println("Watch your screen during the pin tests above!");
}

void testTouchController() {
    Serial.println("\n--- Testing Touch Controller ---");
    
    // Initialize I2C pins for touch testing
    pinMode(TOUCH_RST, OUTPUT);
    pinMode(TOUCH_INT, INPUT_PULLUP);
    
    // Reset touch controller
    digitalWrite(TOUCH_RST, LOW);
    delay(100);
    digitalWrite(TOUCH_RST, HIGH);
    delay(200);
    
    // Try different I2C configurations
    uint8_t addresses[] = {0x15, 0x5A, 0x14, 0x38};
    uint8_t sclPins[] = {8, 17, 6, 7};
    uint8_t sdaPins[] = {3, 18, 5, 15};
    
    bool found = false;
    
    for (int pinSet = 0; pinSet < 4 && !found; pinSet++) {
        Serial.printf("Trying I2C pins: SCL=%d, SDA=%d\n", sclPins[pinSet], sdaPins[pinSet]);
        Wire.begin(sdaPins[pinSet], sclPins[pinSet], 100000); // Lower frequency
        
        for (int addr = 0; addr < 4 && !found; addr++) {
            Wire.beginTransmission(addresses[addr]);
            uint8_t error = Wire.endTransmission();
            
            if (error == 0) {
                Serial.printf("✓ Touch controller found at address 0x%02X with pins SCL=%d, SDA=%d\n", 
                    addresses[addr], sclPins[pinSet], sdaPins[pinSet]);
                found = true;
            }
        }
        
        if (!found) {
            Serial.printf("⚠ No touch controller found with pins SCL=%d, SDA=%d\n", sclPins[pinSet], sdaPins[pinSet]);
        }
    }
    
    if (!found) {
        Serial.println("❌ Touch controller not detected with any pin combination");
        Serial.println("   This might be normal - touch may work with different pins");
        Serial.println("   or the controller might need specific initialization sequence");
    }
    
    Serial.println("--- Touch Test Complete ---\n");
}

void displaySystemInfo() {
    Serial.println("\n--- System Status ---");
    
    // Uptime
    unsigned long uptime = (millis() - startTime) / 1000;
    Serial.printf("Uptime: %lu seconds\n", uptime);
    
    // Memory info
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    
    // Battery voltage
    float batteryVoltage = readBatteryVoltage();
    if (batteryVoltage > 0.1) {
        Serial.printf("Battery: %.2fV\n", batteryVoltage);
    } else {
        Serial.println("Battery: Not connected");
    }
    
    // SD card status
    if (sdCardPresent) {
        Serial.printf("SD Card: %.1f GB\n", SD.cardSize() / (1024.0 * 1024.0 * 1024.0));
    } else {
        Serial.println("SD Card: Not inserted");
    }
    
    // WiFi networks
    Serial.printf("WiFi networks: %d found\n", wifiNetworks);
    
    // Simple time display
    int hours = (uptime / 3600) % 24;
    int minutes = (uptime / 60) % 60;
    int seconds = uptime % 60;
    Serial.printf("Runtime: %02d:%02d:%02d\n", hours, minutes, seconds);
    
    Serial.println("---------------------");
}

float readBatteryVoltage() {
    // Take multiple readings for stability
    int total = 0;
    for (int i = 0; i < 10; i++) {
        total += analogRead(BATTERY_ADC_PIN);
        delay(1);
    }
    int adcValue = total / 10;
    
    // Convert ADC reading to voltage
    float voltage = (adcValue / 4095.0) * 3.3 * 2.0;
    
    // Filter out very low readings
    if (voltage < 0.5) voltage = 0.0;
    
    return voltage;
}

void startWiFiScan() {
    Serial.println("Starting WiFi scan...");
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        Serial.println("No networks found");
        wifiNetworks = 0;
    } else {
        wifiNetworks = n;
        Serial.printf("Found %d networks:\n", n);
        
        for (int i = 0; i < min(n, 10); i++) {
            Serial.printf("  %d: %s (%d dBm) %s\n", 
                i + 1,
                WiFi.SSID(i).c_str(),
                WiFi.RSSI(i),
                WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "Secured");
        }
        
        if (n > 10) {
            Serial.printf("  ... and %d more\n", n - 10);
        }
    }
}

void listSDContents() {
    File root = SD.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open SD root directory");
        return;
    }
    
    Serial.println("SD Card contents:");
    int fileCount = 0;
    
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        
        if (fileCount < 10) {
            Serial.printf("  %s %s (%d bytes)\n", 
                entry.isDirectory() ? "📁" : "📄",
                entry.name(), 
                entry.size());
        }
        fileCount++;
        entry.close();
    }
    
    if (fileCount > 10) {
        Serial.printf("  ... and %d more items\n", fileCount - 10);
    } else if (fileCount == 0) {
        Serial.println("  (empty)");
    }
    
    root.close();
}