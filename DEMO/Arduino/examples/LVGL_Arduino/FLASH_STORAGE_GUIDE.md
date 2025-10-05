# ESP32-S3 Flash Storage Guide

## Overview

The ESP32-S3 has built-in flash memory that can be used for persistent data storage. This guide covers three main approaches for storing data in flash on Arduino:

1. **Preferences Library** (NVS - Non-Volatile Storage) - Best for small key-value pairs
2. **SPIFFS** (SPI Flash File System) - Legacy filesystem, being phased out
3. **LittleFS** (Little File System) - Modern, recommended filesystem

---

## 1. Preferences Library (NVS)

### What is it?
The Preferences library is a wrapper around ESP32's NVS (Non-Volatile Storage) system. It's perfect for storing configuration data, settings, and small key-value pairs that survive reboots.

### When to Use
- ✅ Storing settings (WiFi credentials, volume levels, user preferences)
- ✅ Counters and flags
- ✅ Small amounts of data (individual values, not files)
- ✅ Data that needs wear-leveling
- ❌ NOT for large files or structured data

### Key Features
- Automatic wear-leveling
- Namespace support (organize data into groups)
- Type-safe storage (int, float, string, blob)
- Fast read/write
- No file system overhead

### Basic Usage

**Include Header:**
```cpp
#include <Preferences.h>
```

**Initialize:**
```cpp
Preferences preferences;
```

**Write Data:**
```cpp
void saveSettings() {
    // Open namespace (create if doesn't exist)
    preferences.begin("settings", false);  // false = read/write mode
    
    // Store different types
    preferences.putInt("volume", 75);
    preferences.putFloat("temperature", 23.5);
    preferences.putString("wifi_ssid", "MyNetwork");
    preferences.putBool("enabled", true);
    
    // Close when done
    preferences.end();
}
```

**Read Data:**
```cpp
void loadSettings() {
    preferences.begin("settings", true);  // true = read-only mode
    
    int volume = preferences.getInt("volume", 50);  // 50 is default if not found
    float temp = preferences.getFloat("temperature", 20.0);
    String ssid = preferences.getString("wifi_ssid", "");
    bool enabled = preferences.getBool("enabled", false);
    
    preferences.end();
    
    Serial.printf("Loaded: volume=%d, temp=%.1f, ssid=%s\n", 
                  volume, temp, ssid.c_str());
}
```

**Delete Data:**
```cpp
void clearSettings() {
    preferences.begin("settings", false);
    
    preferences.remove("volume");       // Remove single key
    preferences.clear();                // Remove all keys in namespace
    
    preferences.end();
}
```

### Complete Example: Volume Control with Persistence

```cpp
#include <Preferences.h>

Preferences pref;

int current_volume = 50;
const int max_volume = 100;
const int min_volume = 0;

void setup() {
    Serial.begin(115200);
    
    // Load saved volume on startup
    pref.begin("audio", false);
    current_volume = pref.getInt("volume", 50);  // Default to 50
    pref.end();
    
    Serial.printf("Restored volume: %d\n", current_volume);
}

void volume_up() {
    if (current_volume < max_volume) {
        current_volume++;
        
        // Save immediately
        pref.begin("audio", false);
        pref.putInt("volume", current_volume);
        pref.end();
        
        Serial.printf("Volume: %d\n", current_volume);
    }
}

void volume_down() {
    if (current_volume > min_volume) {
        current_volume--;
        
        pref.begin("audio", false);
        pref.putInt("volume", current_volume);
        pref.end();
        
        Serial.printf("Volume: %d\n", current_volume);
    }
}

void loop() {
    // Your code here
}
```

### Supported Data Types

| Method | Type | Example |
|--------|------|---------|
| `putChar()` / `getChar()` | char | `preferences.putChar('key', 'A')` |
| `putUChar()` / `getUChar()` | unsigned char | `preferences.putUChar('key', 255)` |
| `putShort()` / `getShort()` | int16_t | `preferences.putShort('key', -1000)` |
| `putUShort()` / `getUShort()` | uint16_t | `preferences.putUShort('key', 1000)` |
| `putInt()` / `getInt()` | int32_t | `preferences.putInt('key', 123456)` |
| `putUInt()` / `getUInt()` | uint32_t | `preferences.putUInt('key', 123456)` |
| `putLong()` / `getLong()` | int32_t | `preferences.putLong('key', 123456)` |
| `putULong()` / `getULong()` | uint32_t | `preferences.putULong('key', 123456)` |
| `putLong64()` / `getLong64()` | int64_t | `preferences.putLong64('key', 123456789)` |
| `putULong64()` / `getULong64()` | uint64_t | `preferences.putULong64('key', 123456789)` |
| `putFloat()` / `getFloat()` | float | `preferences.putFloat('key', 3.14)` |
| `putDouble()` / `getDouble()` | double | `preferences.putDouble('key', 3.14159)` |
| `putBool()` / `getBool()` | bool | `preferences.putBool('key', true)` |
| `putString()` / `getString()` | String | `preferences.putString('key', "text")` |
| `putBytes()` / `getBytes()` | byte array | `preferences.putBytes('key', data, len)` |

### Binary Data (Blobs)

```cpp
// Write binary data
uint8_t data[100] = {1, 2, 3, ...};
pref.begin("storage", false);
pref.putBytes("binary_key", data, sizeof(data));
pref.end();

// Read binary data
uint8_t buffer[100];
pref.begin("storage", true);
size_t len = pref.getBytesLength("binary_key");
pref.getBytes("binary_key", buffer, len);
pref.end();
```

### Multiple Namespaces

Organize data into logical groups:

```cpp
// WiFi settings
preferences.begin("wifi", false);
preferences.putString("ssid", "MyNetwork");
preferences.putString("password", "mypass");
preferences.end();

// User preferences  
preferences.begin("user", false);
preferences.putString("name", "John");
preferences.putInt("age", 30);
preferences.end();

// App settings
preferences.begin("app", false);
preferences.putBool("dark_mode", true);
preferences.putInt("language", 1);
preferences.end();
```

### Checking if Key Exists

```cpp
pref.begin("settings", true);
bool exists = pref.isKey("volume");
if (exists) {
    int vol = pref.getInt("volume");
} else {
    Serial.println("Key 'volume' not found");
}
pref.end();
```

---

## 2. LittleFS (Recommended Filesystem)

### What is it?
LittleFS is a modern, power-fail-safe filesystem designed for microcontrollers. It's the recommended filesystem for ESP32 (replacing SPIFFS).

### When to Use
- ✅ Storing configuration files (JSON, XML, CSV)
- ✅ Logging data to files
- ✅ Web server static files (HTML, CSS, JS)
- ✅ Audio/image files
- ✅ Any structured file-based storage

### Key Features
- Power-fail safe
- Wear-leveling
- Dynamic bad block detection
- Small RAM/ROM footprint
- Better performance than SPIFFS

### Basic Usage

**Include Headers:**
```cpp
#include <FS.h>
#include <LittleFS.h>
```

**Initialize:**
```cpp
void setup() {
    Serial.begin(115200);
    
    // Mount LittleFS
    if (!LittleFS.begin(true)) {  // true = format on mount failure
        Serial.println("LittleFS Mount Failed");
        return;
    }
    
    Serial.println("LittleFS Mounted Successfully");
    
    // Get filesystem info
    size_t total = LittleFS.totalBytes();
    size_t used = LittleFS.usedBytes();
    Serial.printf("Total: %u bytes, Used: %u bytes\n", total, used);
}
```

**Write to File:**
```cpp
void writeFile(const char *path, const char *message) {
    Serial.printf("Writing file: %s\n", path);
    
    File file = LittleFS.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    
    file.close();
}
```

**Read from File:**
```cpp
void readFile(const char *path) {
    Serial.printf("Reading file: %s\n", path);
    
    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
    
    Serial.println("File content:");
    while (file.available()) {
        Serial.write(file.read());
    }
    
    file.close();
}
```

**Append to File:**
```cpp
void appendFile(const char *path, const char *message) {
    Serial.printf("Appending to file: %s\n", path);
    
    File file = LittleFS.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    
    if (file.print(message)) {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    
    file.close();
}
```

**Delete File:**
```cpp
void deleteFile(const char *path) {
    Serial.printf("Deleting file: %s\n", path);
    
    if (LittleFS.remove(path)) {
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}
```

**Check if File Exists:**
```cpp
bool fileExists(const char *path) {
    return LittleFS.exists(path);
}
```

**List Directory Contents:**
```cpp
void listDir(const char *dirname) {
    Serial.printf("Listing directory: %s\n", dirname);
    
    File root = LittleFS.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }
    
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.printf("  DIR : %s\n", file.name());
        } else {
            Serial.printf("  FILE: %s  SIZE: %d\n", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}
```

**Create Directory:**
```cpp
void createDir(const char *path) {
    Serial.printf("Creating Dir: %s\n", path);
    
    if (LittleFS.mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}
```

**Remove Directory:**
```cpp
void removeDir(const char *path) {
    Serial.printf("Removing Dir: %s\n", path);
    
    if (LittleFS.rmdir(path)) {
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}
```

### Complete Example: Configuration File

```cpp
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>  // For JSON parsing

void setup() {
    Serial.begin(115200);
    
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS Mount Failed");
        return;
    }
    
    // Save configuration
    saveConfig();
    
    // Load configuration
    loadConfig();
}

void saveConfig() {
    // Create JSON document
    StaticJsonDocument<200> doc;
    doc["wifi_ssid"] = "MyNetwork";
    doc["wifi_pass"] = "password123";
    doc["volume"] = 75;
    doc["brightness"] = 128;
    
    // Open file for writing
    File file = LittleFS.open("/config.json", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to create file");
        return;
    }
    
    // Serialize JSON to file
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write to file");
    } else {
        Serial.println("Configuration saved");
    }
    
    file.close();
}

void loadConfig() {
    // Open file for reading
    File file = LittleFS.open("/config.json", FILE_READ);
    if (!file) {
        Serial.println("Failed to open config file");
        return;
    }
    
    // Parse JSON
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, file);
    
    if (error) {
        Serial.println("Failed to parse config file");
        file.close();
        return;
    }
    
    // Read values
    const char* wifi_ssid = doc["wifi_ssid"];
    const char* wifi_pass = doc["wifi_pass"];
    int volume = doc["volume"];
    int brightness = doc["brightness"];
    
    Serial.printf("Loaded config: SSID=%s, Volume=%d, Brightness=%d\n", 
                  wifi_ssid, volume, brightness);
    
    file.close();
}

void loop() {
    // Your code
}
```

### Logging Example

```cpp
void logMessage(const char* message) {
    File file = LittleFS.open("/log.txt", FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open log file");
        return;
    }
    
    // Add timestamp
    unsigned long time = millis();
    file.printf("[%lu] %s\n", time, message);
    
    file.close();
}

void readLog() {
    File file = LittleFS.open("/log.txt", FILE_READ);
    if (!file) {
        Serial.println("No log file found");
        return;
    }
    
    Serial.println("=== LOG FILE ===");
    while (file.available()) {
        Serial.write(file.read());
    }
    Serial.println("=== END LOG ===");
    
    file.close();
}
```

---

## 3. Flash Size Information

### Get Flash Chip Size

```cpp
#include <ESP.h>

void printFlashInfo() {
    uint32_t flashSize = ESP.getFlashChipSize();
    uint32_t flashSpeed = ESP.getFlashChipSpeed();
    FlashMode_t flashMode = ESP.getFlashChipMode();
    
    Serial.printf("Flash Size: %u MB\n", flashSize / (1024 * 1024));
    Serial.printf("Flash Speed: %u MHz\n", flashSpeed / (1000000));
    Serial.printf("Flash Mode: %s\n", 
                  flashMode == FM_QIO ? "QIO" :
                  flashMode == FM_QOUT ? "QOUT" :
                  flashMode == FM_DIO ? "DIO" :
                  flashMode == FM_DOUT ? "DOUT" : "UNKNOWN");
}
```

---

## Comparison: Preferences vs LittleFS

| Feature | Preferences (NVS) | LittleFS |
|---------|-------------------|----------|
| **Best For** | Settings, config values | Files, structured data |
| **Storage Type** | Key-value pairs | Filesystem |
| **Data Size** | Small (KB) | Large (MB) |
| **Wear Leveling** | ✅ Built-in | ✅ Built-in |
| **Power-Fail Safe** | ✅ Yes | ✅ Yes |
| **Overhead** | Low | Medium |
| **Speed** | Very fast | Fast |
| **Organization** | Namespaces | Directories/Files |
| **Easy of Use** | Very simple | Simple |

---

## Best Practices

### 1. Choose the Right Tool
- Use **Preferences** for simple settings (volume, brightness, flags)
- Use **LittleFS** for files (configs, logs, media)

### 2. Error Handling
Always check return values:
```cpp
if (!LittleFS.begin()) {
    // Handle mount failure
}

File file = LittleFS.open("/file.txt", FILE_READ);
if (!file) {
    // Handle open failure
}
```

### 3. Close Files
Always close files when done:
```cpp
File file = LittleFS.open("/file.txt", FILE_READ);
// ... use file ...
file.close();  // Important!
```

### 4. Avoid Excessive Writes
Flash has limited write cycles:
- Don't write on every loop iteration
- Batch writes when possible
- Use write counters for critical data

### 5. Format on First Use
```cpp
if (!LittleFS.begin(true)) {  // true = format if mount fails
    Serial.println("Format failed");
}
```

---

## Integration with Your Project

### Storing WiFi Credentials

Instead of hardcoding WiFi passwords, store them:

```cpp
#include <Preferences.h>

Preferences prefs;

void saveWiFiCredentials(const char* ssid, const char* password) {
    prefs.begin("wifi", false);
    prefs.putString("ssid", ssid);
    prefs.putString("pass", password);
    prefs.end();
}

bool loadWiFiCredentials(String &ssid, String &password) {
    prefs.begin("wifi", true);
    ssid = prefs.getString("ssid", "");
    password = prefs.getString("pass", "");
    prefs.end();
    
    return (ssid.length() > 0 && password.length() > 0);
}
```

### Storing Application Settings

```cpp
void saveAppSettings() {
    prefs.begin("app", false);
    prefs.putInt("volume", current_volume);
    prefs.putInt("brightness", current_brightness);
    prefs.putBool("dark_mode", dark_mode_enabled);
    prefs.end();
}

void loadAppSettings() {
    prefs.begin("app", true);
    current_volume = prefs.getInt("volume", 50);
    current_brightness = prefs.getInt("brightness", 128);
    dark_mode_enabled = prefs.getBool("dark_mode", false);
    prefs.end();
}
```

---

## Additional Resources

- **ESP32 Preferences Library**: Built-in to Arduino ESP32 core
- **LittleFS**: Built-in to Arduino ESP32 core v2.0.0+
- **ArduinoJson**: For JSON parsing with LittleFS files
  - Install via Library Manager: "ArduinoJson" by Benoit Blanchon

---

*Generated for ESP32-S3 Touch LCD 1.85" (360x360) - Arduino Framework*
