#pragma once
#include <Arduino.h>
#include <WiFi.h>

#define MAX_PASSWORDS 10

class WiFiManager {
private:
    String passwords[MAX_PASSWORDS];
    int passwordCount;
    String connectedSSID;
    bool isConnected;

public:
    WiFiManager();
    
    // Connection management
    bool connectToBestNetwork();
    bool connectToNetwork(const char* ssid, const char* password);
    
    // Status getters
    bool getConnectionStatus();
    String getConnectedSSID();
    String getIPAddress();
};
