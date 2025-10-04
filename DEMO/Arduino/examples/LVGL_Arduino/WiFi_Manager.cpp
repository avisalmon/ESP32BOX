#include "WiFi_Manager.h"
#include "LVGL_Example.h"
#include "WiFi_Config.h"  // Local config file with passwords (not in git)

WiFiManager::WiFiManager() {
    // Load passwords from WiFi_Config.h (compiled in, not in git)
    passwords[0] = WIFI_PASSWORD_1;
    passwords[1] = WIFI_PASSWORD_2;
    passwordCount = WIFI_PASSWORD_COUNT;
    
    isConnected = false;
    connectedSSID = "";
    
    Serial.println("WiFi Manager initialized from WiFi_Config.h");
}

bool WiFiManager::connectToBestNetwork() {
    Serial.println("\n=== WiFi Manager Starting ===");
    
    // Force reset WiFi to clear any previous state
    Serial.println("Resetting WiFi...");
    WiFi.disconnect(true);  // Disconnect and erase credentials
    WiFi.mode(WIFI_OFF);
    delay(500);
    WiFi.mode(WIFI_STA);
    delay(500);
    
    Serial.println("Attempting to connect to best available network...");
    Serial.printf("Hardcoded passwords: %d\n", passwordCount);
    for (int i = 0; i < passwordCount; i++) {
        Serial.printf("  Password %d: [%s]\n", i+1, passwords[i].c_str());
    }
    
    LVGL_WiFi_Display("Scanning WiFi...");
    
    Serial.println("Starting WiFi scan...");
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        Serial.println("ERROR: No networks found");
        LVGL_WiFi_Display("No WiFi found");
        return false;
    }
    
    Serial.printf("\n=== Found %d networks ===\n", n);
    
    // List all networks first
    for (int i = 0; i < n; i++) {
        Serial.printf("  %d: %-20s (RSSI: %d dBm, Ch: %d, Enc: %d)\n", 
                      i+1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), 
                      WiFi.channel(i), WiFi.encryptionType(i));
    }
    
    // Prioritize network from config - move it to the front if found
    const char* prioritySSID = WIFI_PRIORITY_SSID;
    int priorityIndex = -1;
    
    for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i).equalsIgnoreCase(prioritySSID)) {
            priorityIndex = i;
            Serial.printf("\n*** Priority network '%s' found at position %d - will try first! ***\n", prioritySSID, i+1);
            break;
        }
    }
    
    Serial.printf("\n=== Trying passwords on %d networks (Hot1 prioritized) ===\n", n);
    
    // Try Hot1 first if found
    if (priorityIndex >= 0) {
        String ssid = WiFi.SSID(priorityIndex);
        int8_t rssi = WiFi.RSSI(priorityIndex);
        
        Serial.printf("\n--- PRIORITY Network: %s (%d dBm) ---\n", ssid.c_str(), rssi);
        
        // Try each password on Hot1
        for (int p = 0; p < passwordCount; p++) {
            char displayBuf[50];
            snprintf(displayBuf, sizeof(displayBuf), "Try %s [%d/%d]", ssid.c_str(), p+1, passwordCount);
            LVGL_WiFi_Display(displayBuf);
            
            Serial.printf("  Attempt %d/%d: Trying password '%s'...\n", p+1, passwordCount, passwords[p].c_str());
            
            if (connectToNetwork(ssid.c_str(), passwords[p].c_str())) {
                snprintf(displayBuf, sizeof(displayBuf), "WiFi: %s", ssid.c_str());
                LVGL_WiFi_Display(displayBuf);
                Serial.println("\n*** SUCCESS! ***");
                Serial.printf("Connected to: %s\n", ssid.c_str());
                Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
                Serial.printf("Signal strength: %d dBm\n", WiFi.RSSI());
                Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
                Serial.printf("DNS: %s\n", WiFi.dnsIP().toString().c_str());
                Serial.println("=== WiFi Connected ===\n");
                return true;
            } else {
                Serial.println("  FAILED - wrong password or connection issue");
            }
        }
        
        Serial.println("Hot1 connection failed, trying other networks...\n");
    }
    
    // Try remaining networks from strongest to weakest
    for (int i = 0; i < n; i++) {
        // Skip Hot1 if we already tried it
        if (i == priorityIndex) {
            continue;
        }
        String ssid = WiFi.SSID(i);
        int8_t rssi = WiFi.RSSI(i);
        
        Serial.printf("\n--- Network %d/%d: %s (%d dBm) ---\n", i+1, n, ssid.c_str(), rssi);
        
        // Try each password
        for (int p = 0; p < passwordCount; p++) {
            char displayBuf[50];
            snprintf(displayBuf, sizeof(displayBuf), "Try %s [%d/%d]", ssid.c_str(), p+1, passwordCount);
            LVGL_WiFi_Display(displayBuf);
            
            Serial.printf("  Attempt %d/%d: Trying password '%s'...\n", p+1, passwordCount, passwords[p].c_str());
            
            if (connectToNetwork(ssid.c_str(), passwords[p].c_str())) {
                snprintf(displayBuf, sizeof(displayBuf), "WiFi: %s", ssid.c_str());
                LVGL_WiFi_Display(displayBuf);
                Serial.println("\n*** SUCCESS! ***");
                Serial.printf("Connected to: %s\n", ssid.c_str());
                Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
                Serial.printf("Signal strength: %d dBm\n", WiFi.RSSI());
                Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
                Serial.printf("DNS: %s\n", WiFi.dnsIP().toString().c_str());
                Serial.println("=== WiFi Connected ===\n");
                return true;
            } else {
                Serial.println("  FAILED - wrong password or connection issue");
            }
        }
    }
    
    Serial.println("\n=== FAILED: Could not connect to any network ===\n");
    LVGL_WiFi_Display("WiFi failed");
    return false;
}

bool WiFiManager::connectToNetwork(const char* ssid, const char* password) {
    // Add a small delay before attempting connection
    delay(1000);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    Serial.print("    Connecting");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {  // Increased to 30 attempts (15 seconds)
        delay(500);
        Serial.print(".");
        attempts++;
        
        // Check if we're stuck
        if (attempts % 10 == 0) {
            wl_status_t currentStatus = WiFi.status();
            Serial.printf(" [Status: %d] ", currentStatus);
        }
    }
    Serial.println();
    
    wl_status_t status = WiFi.status();
    
    if (status == WL_CONNECTED) {
        isConnected = true;
        connectedSSID = ssid;
        Serial.printf("    Status: CONNECTED (took %d attempts)\n", attempts);
        return true;
    } else {
        isConnected = false;
        connectedSSID = "";
        
        // Print detailed failure reason
        Serial.printf("    Status: FAILED after %d attempts - ", attempts);
        switch (status) {
            case WL_NO_SSID_AVAIL:
                Serial.println("SSID not available");
                break;
            case WL_CONNECT_FAILED:
                Serial.println("Connection failed (likely wrong password)");
                break;
            case WL_CONNECTION_LOST:
                Serial.println("Connection lost");
                break;
            case WL_DISCONNECTED:
                Serial.println("Disconnected (wrong password or incompatible encryption)");
                break;
            case WL_IDLE_STATUS:
                Serial.println("Idle (timeout)");
                break;
            default:
                Serial.printf("Unknown status: %d\n", status);
                break;
        }
        
        // Force disconnect and wait before next attempt
        WiFi.disconnect(true);
        delay(500);
        return false;
    }
}

bool WiFiManager::getConnectionStatus() {
    return isConnected;
}

String WiFiManager::getConnectedSSID() {
    return connectedSSID;
}

String WiFiManager::getIPAddress() {
    if (isConnected) {
        return WiFi.localIP().toString();
    }
    return "Not connected";
}
