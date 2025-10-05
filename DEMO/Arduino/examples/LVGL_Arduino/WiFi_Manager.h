#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

#define MAX_PASSWORDS 10

class WiFiManager {
private:
    String passwords[MAX_PASSWORDS];
    int passwordCount;
    String connectedSSID;
    bool isConnected;
    Preferences preferences;
    
    // Load/Save preferences
    void loadPasswordsFromFlash();
    void savePasswordsToFlash();

public:
    WiFiManager();
    
    // Connection management
    bool connectToBestNetwork();
    bool connectToNetwork(const char* ssid, const char* password);
    
    // Password management
    void addPassword(const String& password);
    void clearPasswords();
    int getPasswordCount();
    void printStoredData();
    
    // Status getters
    bool getConnectionStatus();
    String getConnectedSSID();
    String getIPAddress();
};
