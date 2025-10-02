# WiFi System - ESP32-S3 Wireless

## Hardware Specifications

### WiFi Capabilities

- **Standard**: IEEE 802.11 b/g/n
- **Frequency**: 2.4 GHz
- **Modes**: Station (STA), Access Point (AP), Station+AP
- **Security**: WEP, WPA/WPA2-PSK, WPA2-Enterprise
- **Antenna**: Built-in PCB antenna
- **Range**: ~100m (open space)
- **Power**: Adjustable transmission power

### Bluetooth Capabilities (ESP32-S3)

- **Standard**: Bluetooth 5.0 LE
- **Modes**: Central, Peripheral, Broadcaster, Observer
- **Profiles**: GAP, GATT, SPP equivalent
- **Coexistence**: WiFi + Bluetooth simultaneous operation

## WiFi Configuration

### Basic WiFi Setup

```cpp
#include <WiFi.h>

// WiFi credentials (stored in credentials.h)
#include "credentials.h"
const char* ssid = WIFI_SSID_PRIMARY;
const char* password = WIFI_PASSWORD_PRIMARY;

// WiFi configuration
#define WIFI_TIMEOUT_MS     10000   // 10 second timeout
#define WIFI_RETRY_DELAY    1000    // 1 second between retries
#define MAX_WIFI_RETRIES    5       // Maximum connection attempts
```

### WiFi Status Variables

```cpp
bool wifiConnected = false;
int wifiNetworks = 0;
String currentSSID = "";
int currentRSSI = 0;
IPAddress currentIP;
```

## WiFi Functions

### Network Scanning

```cpp
void startWiFiScan() {
    Serial.println("Scanning for WiFi networks...");
    
    int n = WiFi.scanNetworks();
    wifiNetworks = n;
    
    if (n == 0) {
        Serial.println("No networks found");
    } else {
        Serial.printf("Found %d networks:\n", n);
        
        for (int i = 0; i < min(n, 15); i++) {  // Show first 15
            String security = "";
            switch (WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN:
                    security = "Open";
                    break;
                case WIFI_AUTH_WEP:
                    security = "WEP";
                    break;
                case WIFI_AUTH_WPA_PSK:
                    security = "WPA";
                    break;
                case WIFI_AUTH_WPA2_PSK:
                    security = "WPA2";
                    break;
                case WIFI_AUTH_WPA_WPA2_PSK:
                    security = "WPA/WPA2";
                    break;
                case WIFI_AUTH_WPA2_ENTERPRISE:
                    security = "WPA2-Enterprise";
                    break;
                default:
                    security = "Unknown";
                    break;
            }
            
            Serial.printf("  %2d: %-20s (%3d dBm) [%s] %s\n", 
                         i + 1,
                         WiFi.SSID(i).c_str(),
                         WiFi.RSSI(i),
                         WiFi.BSSIDstr(i).c_str(),
                         security.c_str());
        }
        
        if (n > 15) {
            Serial.printf("  ... and %d more networks\n", n - 15);
        }
    }
    
    // Clean up scan results
    WiFi.scanDelete();
}

void continuousWiFiScan() {
    static unsigned long lastScan = 0;
    const unsigned long scanInterval = 30000;  // 30 seconds
    
    if (millis() - lastScan > scanInterval) {
        lastScan = millis();
        
        // Perform async scan to avoid blocking
        WiFi.scanNetworksAsync([](int networks) {
            Serial.printf("Scan complete: %d networks found\n", networks);
            wifiNetworks = networks;
            
            // Process results here if needed
            WiFi.scanDelete();  // Clean up
        });
    }
}
```

### Connection Management

```cpp
bool connectToWiFi(const char* ssid, const char* password, int timeout_ms = 10000) {
    Serial.printf("Connecting to WiFi: %s\n", ssid);
    
    // Set WiFi mode to station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();  // Clear any previous connection
    delay(100);
    
    // Begin connection
    WiFi.begin(ssid, password);
    
    unsigned long startTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && 
           (millis() - startTime) < timeout_ms) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        currentSSID = String(ssid);
        currentRSSI = WiFi.RSSI();
        currentIP = WiFi.localIP();
        
        Serial.println("✓ WiFi connected successfully!");
        Serial.printf("  SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("  IP Address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI());
        Serial.printf("  MAC Address: %s\n", WiFi.macAddress().c_str());
        
        return true;
    } else {
        wifiConnected = false;
        Serial.printf("❌ Failed to connect to %s\n", ssid);
        
        // Print failure reason
        switch (WiFi.status()) {
            case WL_NO_SSID_AVAIL:
                Serial.println("  Reason: SSID not found");
                break;
            case WL_CONNECT_FAILED:
                Serial.println("  Reason: Connection failed (wrong password?)");
                break;
            case WL_CONNECTION_LOST:
                Serial.println("  Reason: Connection lost");
                break;
            case WL_DISCONNECTED:
                Serial.println("  Reason: Disconnected");
                break;
            default:
                Serial.printf("  Reason: Unknown (%d)\n", WiFi.status());
                break;
        }
        
        return false;
    }
}

void disconnectWiFi() {
    if (wifiConnected) {
        WiFi.disconnect();
        wifiConnected = false;
        currentSSID = "";
        currentRSSI = 0;
        currentIP = IPAddress(0, 0, 0, 0);
        
        Serial.println("✓ WiFi disconnected");
    }
}

void WiFi_Monitor() {
    static unsigned long lastCheck = 0;
    const unsigned long checkInterval = 5000;  // Check every 5 seconds
    
    if (millis() - lastCheck > checkInterval) {
        lastCheck = millis();
        
        wl_status_t status = WiFi.status();
        
        if (status == WL_CONNECTED) {
            if (!wifiConnected) {
                // Connection restored
                wifiConnected = true;
                currentRSSI = WiFi.RSSI();
                currentIP = WiFi.localIP();
                Serial.println("✓ WiFi connection restored");
            }
            
            // Update signal strength
            currentRSSI = WiFi.RSSI();
            
        } else {
            if (wifiConnected) {
                // Connection lost
                wifiConnected = false;
                Serial.println("❌ WiFi connection lost");
                
                // Attempt reconnection
                Serial.println("Attempting to reconnect...");
                WiFi.reconnect();
            }
        }
    }
}
```

### Smart Config (ESP-Touch)

```cpp
void startSmartConfig() {
    Serial.println("Starting SmartConfig...");
    Serial.println("Use ESP-Touch app to configure WiFi");
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    
    unsigned long startTime = millis();
    const unsigned long timeout = 120000;  // 2 minutes timeout
    
    while (!WiFi.smartConfigDone() && (millis() - startTime) < timeout) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();
    
    if (WiFi.smartConfigDone()) {
        Serial.println("✓ SmartConfig received");
        
        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
        
        Serial.println("✓ WiFi connected via SmartConfig");
        Serial.printf("  SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("  IP: %s\n", WiFi.localIP().toString().c_str());
        
        wifiConnected = true;
        currentSSID = WiFi.SSID();
        currentRSSI = WiFi.RSSI();
        currentIP = WiFi.localIP();
        
    } else {
        Serial.println("❌ SmartConfig timeout");
        WiFi.stopSmartConfig();
    }
}
```

## Access Point Mode

### WiFi Hotspot

```cpp
bool startAccessPoint(const char* ssid, const char* password = nullptr) {
    Serial.printf("Starting Access Point: %s\n", ssid);
    
    WiFi.mode(WIFI_AP);
    
    bool result;
    if (password && strlen(password) >= 8) {
        result = WiFi.softAP(ssid, password);
        Serial.printf("  Security: WPA2-PSK (password: %s)\n", password);
    } else {
        result = WiFi.softAP(ssid);
        Serial.println("  Security: Open (no password)");
    }
    
    if (result) {
        IPAddress IP = WiFi.softAPIP();
        Serial.printf("✓ Access Point started\n");
        Serial.printf("  IP Address: %s\n", IP.toString().c_str());
        Serial.printf("  MAC Address: %s\n", WiFi.softAPmacAddress().c_str());
        
        return true;
    } else {
        Serial.println("❌ Failed to start Access Point");
        return false;
    }
}

void stopAccessPoint() {
    WiFi.softAPdisconnect(true);
    Serial.println("✓ Access Point stopped");
}

void monitorAccessPoint() {
    static unsigned long lastCheck = 0;
    
    if (millis() - lastCheck > 10000) {  // Check every 10 seconds
        lastCheck = millis();
        
        int clients = WiFi.softAPgetStationNum();
        Serial.printf("AP Status: %d client(s) connected\n", clients);
        
        if (clients > 0) {
            // You can get more detailed client info with WiFi.softAPgetStationList()
            Serial.println("Client details would be listed here...");
        }
    }
}
```

## Web Server Integration

### Simple Web Server

```cpp
#include <WebServer.h>

WebServer server(80);

void setupWebServer() {
    // Root page
    server.on("/", HTTP_GET, []() {
        String html = "<html><body>";
        html += "<h1>ESP32-S3 Touch LCD</h1>";
        html += "<p>WiFi Status: " + String(wifiConnected ? "Connected" : "Disconnected") + "</p>";
        html += "<p>SSID: " + currentSSID + "</p>";
        html += "<p>RSSI: " + String(currentRSSI) + " dBm</p>";
        html += "<p>IP: " + currentIP.toString() + "</p>";
        html += "<p>Uptime: " + String(millis() / 1000) + " seconds</p>";
        html += "<p><a href='/scan'>Scan Networks</a></p>";
        html += "<p><a href='/status'>System Status</a></p>";
        html += "</body></html>";
        
        server.send(200, "text/html", html);
    });
    
    // Network scan page
    server.on("/scan", HTTP_GET, []() {
        String html = "<html><body><h1>WiFi Networks</h1>";
        
        int n = WiFi.scanNetworks();
        if (n == 0) {
            html += "<p>No networks found</p>";
        } else {
            html += "<table border='1'>";
            html += "<tr><th>SSID</th><th>RSSI</th><th>Security</th></tr>";
            
            for (int i = 0; i < n; i++) {
                html += "<tr>";
                html += "<td>" + WiFi.SSID(i) + "</td>";
                html += "<td>" + String(WiFi.RSSI(i)) + " dBm</td>";
                html += "<td>" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "Secured") + "</td>";
                html += "</tr>";
            }
            html += "</table>";
        }
        
        html += "<p><a href='/'>Back</a></p>";
        html += "</body></html>";
        
        server.send(200, "text/html", html);
        WiFi.scanDelete();
    });
    
    // System status API
    server.on("/status", HTTP_GET, []() {
        String json = "{";
        json += "\"wifi_connected\":" + String(wifiConnected ? "true" : "false") + ",";
        json += "\"ssid\":\"" + currentSSID + "\",";
        json += "\"rssi\":" + String(currentRSSI) + ",";
        json += "\"ip\":\"" + currentIP.toString() + "\",";
        json += "\"uptime\":" + String(millis() / 1000) + ",";
        json += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
        json += "\"battery_voltage\":" + String(BAT_Get_Volts()) + ",";
        json += "\"networks_found\":" + String(wifiNetworks);
        json += "}";
        
        server.send(200, "application/json", json);
    });
    
    // 404 handler
    server.onNotFound([]() {
        server.send(404, "text/plain", "Not found");
    });
    
    server.begin();
    Serial.println("✓ Web server started");
}

void handleWebServer() {
    server.handleClient();
}
```

## Network Time Synchronization

### NTP Client

```cpp
#include <time.h>

void syncTimeWithNTP() {
    if (!wifiConnected) {
        Serial.println("WiFi not connected - cannot sync time");
        return;
    }
    
    Serial.println("Synchronizing time with NTP servers...");
    
    // Configure NTP
    configTime(0, 0, "pool.ntp.org", "time.nist.gov", "time.google.com");
    
    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 15;
    
    while (timeinfo.tm_year < (2024 - 1900) && ++retry < retry_count) {
        Serial.printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
        delay(1000);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    
    if (timeinfo.tm_year >= (2024 - 1900)) {
        Serial.println("✓ Time synchronized successfully");
        Serial.printf("Current time: %s", asctime(&timeinfo));
        
        // Update RTC if available
        if (/* RTC available check */) {
            // sync_rtc_with_ntp(&timeinfo);
        }
    } else {
        Serial.println("❌ Failed to synchronize time");
    }
}
```

## Power Management

### WiFi Power Saving

```cpp
void enableWiFiPowerSaving() {
    WiFi.setSleep(WIFI_PS_MIN_MODEM);  // Minimum power saving
    Serial.println("✓ WiFi power saving enabled");
}

void disableWiFiPowerSaving() {
    WiFi.setSleep(WIFI_PS_NONE);  // No power saving
    Serial.println("✓ WiFi power saving disabled");
}

void setWiFiTxPower(wifi_power_t power) {
    WiFi.setTxPower(power);
    
    const char* powerNames[] = {
        "19.5dBm", "19dBm", "18.5dBm", "17dBm", 
        "16dBm", "15dBm", "13dBm", "11dBm",
        "8.5dBm", "7dBm", "5dBm", "2dBm", "-1dBm"
    };
    
    Serial.printf("✓ WiFi TX power set to %s\n", powerNames[power]);
}
```

## LVGL WiFi UI Integration

### WiFi Status Display

```cpp
lv_obj_t* wifi_icon;
lv_obj_t* wifi_label;

void create_wifi_ui() {
    // WiFi status icon
    wifi_icon = lv_label_create(lv_scr_act());
    lv_obj_align(wifi_icon, LV_ALIGN_TOP_LEFT, 10, 10);
    
    // WiFi status label
    wifi_label = lv_label_create(lv_scr_act());
    lv_obj_align_to(wifi_label, wifi_icon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    
    update_wifi_ui();
}

void update_wifi_ui() {
    if (wifiConnected) {
        // Set icon based on signal strength
        if (currentRSSI > -50) {
            lv_label_set_text(wifi_icon, LV_SYMBOL_WIFI);
            lv_obj_set_style_text_color(wifi_icon, lv_color_hex(0x00FF00), 0);
        } else if (currentRSSI > -70) {
            lv_label_set_text(wifi_icon, LV_SYMBOL_WIFI);
            lv_obj_set_style_text_color(wifi_icon, lv_color_hex(0xFFAA00), 0);
        } else {
            lv_label_set_text(wifi_icon, LV_SYMBOL_WIFI);
            lv_obj_set_style_text_color(wifi_icon, lv_color_hex(0xFF0000), 0);
        }
        
        lv_label_set_text_fmt(wifi_label, "%s (%d dBm)", currentSSID.c_str(), currentRSSI);
    } else {
        lv_label_set_text(wifi_icon, LV_SYMBOL_CLOSE);
        lv_obj_set_style_text_color(wifi_icon, lv_color_hex(0x808080), 0);
        lv_label_set_text(wifi_label, "Disconnected");
    }
}
```

## Troubleshooting

### Common Issues

1. **Connection Failures**
   - Verify SSID and password
   - Check network security type
   - Ensure network is in range
   - Try different channels
   - Check for MAC address filtering

2. **Weak Signal Issues**
   - Move closer to router
   - Check for interference
   - Use external antenna (if available)
   - Adjust router antenna position

3. **Frequent Disconnections**
   - Check power supply stability
   - Disable power saving if needed
   - Update router firmware
   - Check for network congestion

4. **Slow Performance**
   - Check network bandwidth
   - Reduce concurrent connections
   - Optimize antenna placement
   - Use 5GHz if available (ESP32-S3 is 2.4GHz only)

### Debug Functions

```cpp
void wifi_diagnostic() {
    Serial.println("=== WiFi Diagnostic ===");
    
    Serial.printf("WiFi Mode: %d\n", WiFi.getMode());
    Serial.printf("WiFi Status: %d\n", WiFi.status());
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("BSSID: %s\n", WiFi.BSSIDstr().c_str());
        Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
        Serial.printf("Channel: %d\n", WiFi.channel());
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Subnet: %s\n", WiFi.subnetMask().toString().c_str());
        Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("DNS1: %s\n", WiFi.dnsIP(0).toString().c_str());
        Serial.printf("DNS2: %s\n", WiFi.dnsIP(1).toString().c_str());
    }
    
    Serial.printf("MAC Address (STA): %s\n", WiFi.macAddress().c_str());
    Serial.printf("MAC Address (AP): %s\n", WiFi.softAPmacAddress().c_str());
    
    // Test connection to Google DNS
    if (wifiConnected) {
        Serial.println("Testing internet connectivity...");
        WiFiClient client;
        if (client.connect("8.8.8.8", 53)) {
            Serial.println("✓ Internet connectivity OK");
            client.stop();
        } else {
            Serial.println("❌ No internet connectivity");
        }
    }
}

void wifi_performance_test() {
    if (!wifiConnected) {
        Serial.println("WiFi not connected - cannot run performance test");
        return;
    }
    
    Serial.println("=== WiFi Performance Test ===");
    
    // Test DNS resolution speed
    unsigned long startTime = millis();
    IPAddress ip;
    bool resolved = WiFi.hostByName("www.google.com", ip);
    unsigned long dnsTime = millis() - startTime;
    
    if (resolved) {
        Serial.printf("DNS Resolution: %lu ms (resolved to %s)\n", dnsTime, ip.toString().c_str());
    } else {
        Serial.printf("DNS Resolution: FAILED (%lu ms)\n", dnsTime);
    }
    
    // Test HTTP request speed
    startTime = millis();
    WiFiClient client;
    
    if (client.connect("httpbin.org", 80)) {
        client.print("GET /get HTTP/1.1\r\nHost: httpbin.org\r\nConnection: close\r\n\r\n");
        
        while (client.connected() && !client.available()) {
            delay(1);
        }
        
        unsigned long responseTime = millis() - startTime;
        Serial.printf("HTTP Request: %lu ms\n", responseTime);
        
        // Read response (first few bytes)
        String response = "";
        int bytesRead = 0;
        while (client.available() && bytesRead < 200) {
            response += (char)client.read();
            bytesRead++;
        }
        
        Serial.printf("Response preview: %s...\n", response.substring(0, 50).c_str());
        client.stop();
    } else {
        Serial.println("HTTP Request: FAILED (connection failed)");
    }
}
```

## Example Implementation

### WiFi Manager

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize WiFi
    WiFi.mode(WIFI_STA);
    
    // Try to connect to saved network
    if (connectToWiFi(WIFI_SSID_PRIMARY, WIFI_PASSWORD_PRIMARY)) {
        // Connected successfully
        syncTimeWithNTP();
        setupWebServer();
    } else {
        // Connection failed, start SmartConfig
        startSmartConfig();
    }
    
    Serial.println("WiFi Manager Ready");
}

void loop() {
    // Monitor WiFi connection
    WiFi_Monitor();
    
    // Handle web server requests
    if (wifiConnected) {
        handleWebServer();
    }
    
    // Periodic network scan
    continuousWiFiScan();
    
    delay(100);
}
```

*Last Updated: October 2, 2025*