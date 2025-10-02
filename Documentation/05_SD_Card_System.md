# SD Card System - SDIO Interface

## Hardware Specifications

### SD Card Support

- **Interface**: SDIO (1-bit mode)
- **Supported Cards**: Micro SD, Micro SDHC
- **Capacity**: Up to 32GB (FAT32)
- **Speed Class**: Class 10 recommended
- **Voltage**: 3.3V
- **File Systems**: FAT16, FAT32

### Performance Specifications

- **Read Speed**: Up to 25 MB/s (depending on card)
- **Write Speed**: Up to 10 MB/s (depending on card)
- **Simultaneous Operations**: Read/Write with audio playback
- **File Operations**: Standard POSIX-like API

## Pin Configuration

### SDIO Interface

```cpp
#define SD_CLK_PIN      14      // SD Clock
#define SD_CMD_PIN      17      // SD Command
#define SD_D0_PIN       16      // SD Data 0
```

**Note**: Uses 1-bit SDIO mode (only DATA0 line used)

### SD Card Detection

SD card presence is detected via TCA9554PWR I/O expander:

```cpp
#define TCA9554_ADDRESS 0x20    // I/O expander address
// SD card detect pin is managed through I/O expander
```

## Global Variables

```cpp
extern uint16_t SDCard_Size;    // SD card size in MB
extern uint16_t Flash_Size;     // Internal flash size in MB
```

## Initialization

### SD Card Setup

```cpp
void SD_Init() {
    Serial.println("Initializing SD card...");
    
    // Initialize SDIO pins
    SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN);
    
    // Start SD card in 1-bit mode
    if (!SD_MMC.begin("/sdcard", true)) {  // true = 1-bit mode
        Serial.println("❌ SD card initialization failed");
        SDCard_Size = 0;
        return;
    }
    
    // Get card information
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("❌ No SD card attached");
        SDCard_Size = 0;
        return;
    }
    
    // Display card information
    Serial.printf("✓ SD card detected: ");
    switch (cardType) {
        case CARD_MMC:
            Serial.print("MMC");
            break;
        case CARD_SD:
            Serial.print("SDSC");
            break;
        case CARD_SDHC:
            Serial.print("SDHC");
            break;
        default:
            Serial.print("Unknown");
            break;
    }
    
    // Get and display card size
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    SDCard_Size = (uint16_t)cardSize;
    Serial.printf(" - Size: %d MB\n", SDCard_Size);
    
    // Display used/available space
    uint64_t usedBytes = SD_MMC.usedBytes() / (1024 * 1024);
    uint64_t totalBytes = SD_MMC.totalBytes() / (1024 * 1024);
    Serial.printf("Used: %llu MB / Total: %llu MB\n", usedBytes, totalBytes);
    
    Serial.println("✓ SD card initialization complete");
}
```

### Flash Memory Information

```cpp
void Flash_test() {
    // Get internal flash information
    Flash_Size = ESP.getFlashChipSize() / (1024 * 1024);
    
    Serial.printf("Internal Flash: %d MB\n", Flash_Size);
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    
    // Display partition information
    Serial.println("Flash Partitions:");
    Serial.printf("  Sketch: %d bytes\n", ESP.getSketchSize());
    Serial.printf("  Free Sketch Space: %d bytes\n", ESP.getFreeSketchSpace());
}
```

## File Operations

### File Search Functions

```cpp
bool File_Search(const char* directory, const char* fileName) {
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, fileName);
    
    File file = SD_MMC.open(fullPath);
    if (file) {
        file.close();
        return true;
    }
    return false;
}
```

### Directory Listing

```cpp
uint16_t Folder_retrieval(const char* directory, const char* fileExtension, 
                         char File_Name[][100], uint16_t maxFiles) {
    
    File dir = SD_MMC.open(directory);
    if (!dir) {
        Serial.printf("Failed to open directory: %s\n", directory);
        return 0;
    }
    
    if (!dir.isDirectory()) {
        Serial.printf("Not a directory: %s\n", directory);
        dir.close();
        return 0;
    }
    
    uint16_t fileCount = 0;
    
    while (fileCount < maxFiles) {
        File entry = dir.openNextFile();
        if (!entry) break;
        
        String fileName = entry.name();
        
        // Check file extension
        if (fileExtension && strlen(fileExtension) > 0) {
            if (!fileName.toLowerCase().endsWith(String(fileExtension).toLowerCase())) {
                entry.close();
                continue;
            }
        }
        
        // Skip directories unless specifically requested
        if (entry.isDirectory()) {
            entry.close();
            continue;
        }
        
        // Copy filename to array
        strncpy(File_Name[fileCount], fileName.c_str(), 99);
        File_Name[fileCount][99] = '\0';  // Ensure null termination
        
        fileCount++;
        entry.close();
    }
    
    dir.close();
    return fileCount;
}
```

### Advanced File Operations

```cpp
void create_directory(const char* path) {
    if (SD_MMC.mkdir(path)) {
        Serial.printf("✓ Directory created: %s\n", path);
    } else {
        Serial.printf("❌ Failed to create directory: %s\n", path);
    }
}

void delete_file(const char* path) {
    if (SD_MMC.remove(path)) {
        Serial.printf("✓ File deleted: %s\n", path);
    } else {
        Serial.printf("❌ Failed to delete file: %s\n", path);
    }
}

void rename_file(const char* oldPath, const char* newPath) {
    if (SD_MMC.rename(oldPath, newPath)) {
        Serial.printf("✓ File renamed: %s -> %s\n", oldPath, newPath);
    } else {
        Serial.printf("❌ Failed to rename file: %s -> %s\n", oldPath, newPath);
    }
}

void copy_file(const char* sourcePath, const char* destPath) {
    File sourceFile = SD_MMC.open(sourcePath);
    if (!sourceFile) {
        Serial.printf("❌ Failed to open source: %s\n", sourcePath);
        return;
    }
    
    File destFile = SD_MMC.open(destPath, FILE_WRITE);
    if (!destFile) {
        Serial.printf("❌ Failed to create destination: %s\n", destPath);
        sourceFile.close();
        return;
    }
    
    // Copy file contents
    uint8_t buffer[1024];
    while (sourceFile.available()) {
        size_t bytesRead = sourceFile.read(buffer, sizeof(buffer));
        destFile.write(buffer, bytesRead);
    }
    
    sourceFile.close();
    destFile.close();
    
    Serial.printf("✓ File copied: %s -> %s\n", sourcePath, destPath);
}
```

## Data Logging Functions

### CSV Data Logging

```cpp
void log_sensor_data(float temperature, float humidity, int battery) {
    File logFile = SD_MMC.open("/data/sensors.csv", FILE_APPEND);
    if (!logFile) {
        Serial.println("❌ Failed to open log file");
        return;
    }
    
    // Get timestamp (you would get this from RTC)
    unsigned long timestamp = millis() / 1000;
    
    // Write CSV data
    logFile.printf("%lu,%.2f,%.2f,%d\n", timestamp, temperature, humidity, battery);
    logFile.close();
    
    Serial.println("✓ Sensor data logged");
}

void create_csv_header() {
    File logFile = SD_MMC.open("/data/sensors.csv", FILE_WRITE);
    if (logFile) {
        logFile.println("Timestamp,Temperature,Humidity,Battery");
        logFile.close();
        Serial.println("✓ CSV header created");
    }
}
```

### Configuration File Management

```cpp
void save_configuration() {
    File configFile = SD_MMC.open("/config.json", FILE_WRITE);
    if (!configFile) {
        Serial.println("❌ Failed to create config file");
        return;
    }
    
    // Write JSON configuration
    configFile.println("{");
    configFile.println("  \"wifi_ssid\": \"MyNetwork\",");
    configFile.println("  \"wifi_password\": \"MyPassword\",");
    configFile.println("  \"volume\": 15,");
    configFile.println("  \"brightness\": 80,");
    configFile.println("  \"auto_sleep\": true");
    configFile.println("}");
    
    configFile.close();
    Serial.println("✓ Configuration saved");
}

void load_configuration() {
    File configFile = SD_MMC.open("/config.json");
    if (!configFile) {
        Serial.println("❌ Config file not found, using defaults");
        return;
    }
    
    Serial.println("Loading configuration:");
    while (configFile.available()) {
        String line = configFile.readStringUntil('\n');
        Serial.println(line);
        // Parse JSON configuration here
    }
    
    configFile.close();
    Serial.println("✓ Configuration loaded");
}
```

## Audio File Management

### Music Library Functions

```cpp
void scan_music_library() {
    const char* musicExtensions[] = {".mp3", ".wav", ".aac", ".flac"};
    const int numExtensions = 4;
    
    Serial.println("Scanning music library...");
    
    for (int ext = 0; ext < numExtensions; ext++) {
        char musicFiles[50][100];
        uint16_t count = Folder_retrieval("/music", musicExtensions[ext], 
                                         musicFiles, 50);
        
        if (count > 0) {
            Serial.printf("Found %d %s files:\n", count, musicExtensions[ext]);
            for (int i = 0; i < count; i++) {
                Serial.printf("  %d: %s\n", i + 1, musicFiles[i]);
            }
        }
    }
}

void create_playlist_file() {
    File playlist = SD_MMC.open("/playlists/favorites.m3u", FILE_WRITE);
    if (!playlist) {
        Serial.println("❌ Failed to create playlist");
        return;
    }
    
    playlist.println("#EXTM3U");
    playlist.println("#EXTINF:-1,Song Title 1");
    playlist.println("/music/song1.mp3");
    playlist.println("#EXTINF:-1,Song Title 2");
    playlist.println("/music/song2.mp3");
    
    playlist.close();
    Serial.println("✓ Playlist created");
}
```

### Image File Management

```cpp
void list_images() {
    const char* imageExtensions[] = {".jpg", ".jpeg", ".png", ".bmp"};
    const int numExtensions = 4;
    
    Serial.println("Image files:");
    
    for (int ext = 0; ext < numExtensions; ext++) {
        char imageFiles[20][100];
        uint16_t count = Folder_retrieval("/images", imageExtensions[ext], 
                                         imageFiles, 20);
        
        for (int i = 0; i < count; i++) {
            File imageFile = SD_MMC.open(("/images/" + String(imageFiles[i])).c_str());
            if (imageFile) {
                Serial.printf("  🇮 %s (%d bytes)\n", imageFiles[i], imageFile.size());
                imageFile.close();
            }
        }
    }
}
```

## Performance Optimization

### Buffered File Operations

```cpp
class BufferedWriter {
private:
    File file;
    uint8_t buffer[4096];
    size_t bufferPos;
    
public:
    bool begin(const char* filename) {
        file = SD_MMC.open(filename, FILE_WRITE);
        bufferPos = 0;
        return file;
    }
    
    void write(const uint8_t* data, size_t length) {
        for (size_t i = 0; i < length; i++) {
            buffer[bufferPos++] = data[i];
            
            if (bufferPos >= sizeof(buffer)) {
                flush();
            }
        }
    }
    
    void flush() {
        if (bufferPos > 0 && file) {
            file.write(buffer, bufferPos);
            bufferPos = 0;
        }
    }
    
    void close() {
        flush();
        if (file) {
            file.close();
        }
    }
};
```

### Directory Caching

```cpp
struct DirectoryCache {
    char path[64];
    char files[20][64];
    int count;
    unsigned long lastUpdate;
};

DirectoryCache musicCache = {"/music", {}, 0, 0};

void update_directory_cache(DirectoryCache* cache, const char* extension) {
    if (millis() - cache->lastUpdate < 30000) {  // Cache for 30 seconds
        return;
    }
    
    char fileList[20][100];
    cache->count = Folder_retrieval(cache->path, extension, fileList, 20);
    
    for (int i = 0; i < cache->count; i++) {
        strncpy(cache->files[i], fileList[i], 63);
        cache->files[i][63] = '\0';
    }
    
    cache->lastUpdate = millis();
}
```

## Troubleshooting

### Common Issues

1. **SD Card Not Detected**
   - Check physical connection and card insertion
   - Verify SDIO pin connections (14, 17, 16)
   - Test with different SD card
   - Check card format (FAT32 recommended)

2. **Slow File Operations**
   - Use Class 10 or better SD cards
   - Implement buffered I/O
   - Avoid frequent small writes
   - Use appropriate cluster sizes

3. **File Corruption**
   - Always close files properly
   - Handle power loss gracefully
   - Use checksums for critical data
   - Implement wear leveling

4. **Out of Space Errors**
   - Monitor available space regularly
   - Implement automatic cleanup
   - Use compression for large files
   - Archive old data

### Debug Functions

```cpp
void sd_card_benchmark() {
    Serial.println("=== SD Card Benchmark ===");
    
    const char* testFile = "/benchmark.txt";
    const int testSize = 10240;  // 10KB test
    uint8_t testData[1024];
    
    // Fill test data
    for (int i = 0; i < 1024; i++) {
        testData[i] = i % 256;
    }
    
    // Write test
    unsigned long startTime = millis();
    File file = SD_MMC.open(testFile, FILE_WRITE);
    
    if (file) {
        for (int i = 0; i < testSize / 1024; i++) {
            file.write(testData, 1024);
        }
        file.close();
        
        unsigned long writeTime = millis() - startTime;
        float writeSpeed = (float)testSize / writeTime;  // KB/s
        Serial.printf("Write Speed: %.2f KB/s\n", writeSpeed);
    }
    
    // Read test
    startTime = millis();
    file = SD_MMC.open(testFile);
    
    if (file) {
        while (file.available()) {
            file.read(testData, min(1024, file.available()));
        }
        file.close();
        
        unsigned long readTime = millis() - startTime;
        float readSpeed = (float)testSize / readTime;  // KB/s
        Serial.printf("Read Speed: %.2f KB/s\n", readSpeed);
    }
    
    // Cleanup
    SD_MMC.remove(testFile);
    
    Serial.println("Benchmark complete");
}

void sd_card_health_check() {
    Serial.println("=== SD Card Health Check ===");
    
    // Basic information
    Serial.printf("Card Type: %d\n", SD_MMC.cardType());
    Serial.printf("Card Size: %llu MB\n", SD_MMC.cardSize() / (1024 * 1024));
    Serial.printf("Used Space: %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024));
    Serial.printf("Free Space: %llu MB\n", 
                 (SD_MMC.totalBytes() - SD_MMC.usedBytes()) / (1024 * 1024));
    
    // Test write/read operation
    const char* testFile = "/health_test.tmp";
    
    File file = SD_MMC.open(testFile, FILE_WRITE);
    if (file) {
        file.println("Health check test data");
        file.close();
        Serial.println("✓ Write operation successful");
        
        file = SD_MMC.open(testFile);
        if (file) {
            String data = file.readString();
            file.close();
            
            if (data.indexOf("Health check") >= 0) {
                Serial.println("✓ Read operation successful");
            } else {
                Serial.println("❌ Read data corruption detected");
            }
        } else {
            Serial.println("❌ Read operation failed");
        }
        
        SD_MMC.remove(testFile);
    } else {
        Serial.println("❌ Write operation failed");
    }
    
    Serial.println("Health check complete");
}
```

## Example Implementation

### Data Logger Example

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize SD card
    SD_Init();
    
    // Create data directory
    if (!SD_MMC.exists("/data")) {
        SD_MMC.mkdir("/data");
    }
    
    // Create CSV header
    create_csv_header();
    
    Serial.println("Data logger ready");
}

void loop() {
    // Simulate sensor readings
    float temperature = 20.0 + random(-50, 150) / 10.0;
    float humidity = 50.0 + random(-200, 200) / 10.0;
    int battery = random(0, 100);
    
    // Log data every 30 seconds
    static unsigned long lastLog = 0;
    if (millis() - lastLog > 30000) {
        lastLog = millis();
        log_sensor_data(temperature, humidity, battery);
    }
    
    delay(1000);
}
```

*Last Updated: October 2, 2025*