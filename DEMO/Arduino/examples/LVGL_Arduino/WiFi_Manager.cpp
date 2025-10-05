#include "WiFi_Manager.h"
#include "LVGL_Example.h"

WiFiManager::WiFiManager() {
    isConnected = false;
    connectedSSID = "";
    passwordCount = 0;
    
    Serial.println("\n=== WiFi Manager Initializing ===");
    
    // Load passwords from flash (Preferences/NVS)
    loadPasswordsFromFlash();
    
    // If no passwords stored, add defaults
    if (passwordCount == 0) {
        Serial.println("No passwords in flash - adding defaults");
        addPassword("10203040");
        addPassword("aviaviavi");
        savePasswordsToFlash();
        Serial.println("Default passwords saved to flash");
    }
    
    Serial.println("=== WiFi Manager Ready ===\n");
}

void WiFiManager::loadPasswordsFromFlash() {
    preferences.begin("wifi", true);  // Read-only mode
    
    passwordCount = preferences.getInt("pass_count", 0);
    
    Serial.printf("Loading %d passwords from flash storage...\n", passwordCount);
    
    for (int i = 0; i < passwordCount && i < MAX_PASSWORDS; i++) {
        String key = "pass_" + String(i);
        passwords[i] = preferences.getString(key.c_str(), "");
        Serial.printf("  Password %d: [%s] (length: %d)\n", i+1, passwords[i].c_str(), passwords[i].length());
    }
    
    preferences.end();
    Serial.println("Passwords loaded from flash");
}

void WiFiManager::savePasswordsToFlash() {
    preferences.begin("wifi", false);  // Read-write mode
    
    Serial.printf("Saving %d passwords to flash storage...\n", passwordCount);
    
    preferences.putInt("pass_count", passwordCount);
    
    for (int i = 0; i < passwordCount; i++) {
        String key = "pass_" + String(i);
        preferences.putString(key.c_str(), passwords[i]);
        Serial.printf("  Saved: %s = [%s]\n", key.c_str(), passwords[i].c_str());
    }
    
    preferences.end();
    Serial.println("Passwords saved to flash successfully");
}

void WiFiManager::addPassword(const String& password) {
    if (passwordCount < MAX_PASSWORDS) {
        passwords[passwordCount] = password;
        passwordCount++;
        Serial.printf("Added password: [%s] (total: %d)\n", password.c_str(), passwordCount);
    } else {
        Serial.println("Cannot add password - maximum reached");
    }
}

void WiFiManager::clearPasswords() {
    preferences.begin("wifi", false);
    preferences.clear();
    preferences.end();
    
    passwordCount = 0;
    Serial.println("All passwords cleared from flash");
}

int WiFiManager::getPasswordCount() {
    return passwordCount;
}

void WiFiManager::printStoredData() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║   WiFi Flash Storage Contents         ║");
    Serial.println("╚════════════════════════════════════════╝");
    
    preferences.begin("wifi", true);  // Read-only
    
    int count = preferences.getInt("pass_count", 0);
    Serial.printf("Password Count: %d\n\n", count);
    
    if (count == 0) {
        Serial.println("❌ No passwords stored in flash");
    } else {
        Serial.println("Stored Passwords:");
        for (int i = 0; i < count; i++) {
            String key = "pass_" + String(i);
            String pass = preferences.getString(key.c_str(), "");
            Serial.printf("  %d. [%s] (length: %d chars)\n", i+1, pass.c_str(), pass.length());
        }
    }
    
    preferences.end();
    
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║   Current Runtime Status              ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.printf("Active Passwords in Memory: %d\n", passwordCount);
    Serial.printf("WiFi Connected: %s\n", isConnected ? "YES" : "NO");
    if (isConnected) {
        Serial.printf("Connected SSID: %s\n", connectedSSID.c_str());
        Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Signal Strength: %d dBm\n", WiFi.RSSI());
    }
    Serial.println("════════════════════════════════════════\n");
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
    
    // Define priority networks to try in order
    const char* priorityNetworks[] = {"Hot1", "AviRedmi"};
    const int priorityCount = 2;
    int priorityIndices[priorityCount];
    
    // Find priority networks in scan results
    for (int p = 0; p < priorityCount; p++) {
        priorityIndices[p] = -1;
        for (int i = 0; i < n; i++) {
            if (WiFi.SSID(i).equalsIgnoreCase(priorityNetworks[p])) {
                priorityIndices[p] = i;
                Serial.printf("\n*** Priority network #%d '%s' found at position %d ***\n", 
                              p+1, priorityNetworks[p], i+1);
                break;
            }
        }
    }
    
    Serial.printf("\n=== Trying passwords on %d networks (Hot1 → AviRedmi → others) ===\n", n);
    
    // Try priority networks in order
    for (int p = 0; p < priorityCount; p++) {
        int priorityIndex = priorityIndices[p];
        
        if (priorityIndex >= 0) {
            String ssid = WiFi.SSID(priorityIndex);
            int8_t rssi = WiFi.RSSI(priorityIndex);
            
            Serial.printf("\n--- PRIORITY #%d Network: %s (%d dBm) ---\n", p+1, ssid.c_str(), rssi);
            
            // Try each password on this priority network
            for (int pw = 0; pw < passwordCount; pw++) {
                char displayBuf[50];
                snprintf(displayBuf, sizeof(displayBuf), "Try %s [%d/%d]", ssid.c_str(), pw+1, passwordCount);
                LVGL_WiFi_Display(displayBuf);
                
                Serial.printf("  Attempt %d/%d: Trying password '%s'...\n", pw+1, passwordCount, passwords[pw].c_str());
                
                if (connectToNetwork(ssid.c_str(), passwords[pw].c_str())) {
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
            
            Serial.printf("%s connection failed, trying next priority network...\n\n", ssid.c_str());
        }
    }
    
    // Try remaining networks from strongest to weakest
    for (int i = 0; i < n; i++) {
        // Skip priority networks if we already tried them
        bool isPriority = false;
        for (int p = 0; p < priorityCount; p++) {
            if (i == priorityIndices[p]) {
                isPriority = true;
                break;
            }
        }
        if (isPriority) {
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
