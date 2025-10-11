# SD Card and Audio Playback Guide

Complete guide for using the SD card interface and audio playback system on the ESP32-S3 Touch LCD 1.85".

---

## 📀 SD Card Interface

### Hardware Specifications

**SD Card Slot:** SDHC/MMC compatible, 1-bit mode
**Supported Cards:**
- SD (Standard Capacity)
- SDHC (High Capacity) - Recommended
- MMC (MultiMediaCard)

**Pin Configuration:**
| Signal | GPIO | Description |
|--------|------|-------------|
| CLK | 14 | Clock signal |
| CMD | 17 | Command line |
| D0 | 16 | Data line 0 |
| Power | Via EXIO_PIN4 | Card power control |

**Maximum Capacity:** 32GB (FAT32 formatted)
**File Systems:** FAT16, FAT32

---

## 🔧 Setup and Initialization

### Basic Initialization

The SD card is automatically initialized in `setup()`:

```cpp
void setup() {
    // ... other initialization ...
    
    SD_Init();  // Initializes SD card interface
    
    // Check if card is present
    if (SD_MMC.cardType() != CARD_NONE) {
        Serial.println("SD Card detected!");
    } else {
        Serial.println("No SD card");
    }
}
```

### Initialization Output

When successful, you'll see:
```
SD card initialization successful!
SD Card Type: SDHC
Total space: 31902400512
Used space: 32768
Free space: 31902367744
```

---

## 📁 File Operations

### Check if File Exists

```cpp
if (SD_MMC.exists("/myfile.mp3")) {
    Serial.println("File exists!");
}
```

### Search for File in Directory

```cpp
bool File_Search(const char* directory, const char* fileName);

// Example usage:
if (File_Search("/music", "song.mp3")) {
    Serial.println("Found song.mp3 in /music folder");
}
```

### List All Files with Extension

```cpp
char fileList[50][100];  // Array to store filenames
uint16_t count = Folder_retrieval("/music", ".mp3", fileList, 50);

Serial.printf("Found %d MP3 files:\n", count);
for (int i = 0; i < count; i++) {
    Serial.printf("  %d. %s\n", i + 1, fileList[i]);
}
```

### Read File Contents

```cpp
File file = SD_MMC.open("/readme.txt");
if (file) {
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}
```

### Write to File

```cpp
File file = SD_MMC.open("/log.txt", FILE_WRITE);
if (file) {
    file.println("Log entry");
    file.close();
    Serial.println("Data written");
}
```

### Append to File

```cpp
File file = SD_MMC.open("/log.txt", FILE_APPEND);
if (file) {
    file.println("New log entry");
    file.close();
}
```

### Delete File

```cpp
if (SD_MMC.remove("/oldfile.txt")) {
    Serial.println("File deleted");
}
```

### Get Card Info

```cpp
uint64_t totalBytes = SD_MMC.totalBytes();
uint64_t usedBytes = SD_MMC.usedBytes();
uint64_t freeBytes = totalBytes - usedBytes;

Serial.printf("Total: %.2f MB\n", totalBytes / (1024.0 * 1024.0));
Serial.printf("Used: %.2f MB\n", usedBytes / (1024.0 * 1024.0));
Serial.printf("Free: %.2f MB\n", freeBytes / (1024.0 * 1024.0));
```

---

## 🎵 Audio Playback System

### Hardware

**DAC:** PCM5101A High-Performance Audio DAC
**Interface:** I2S (Inter-IC Sound)
**Sample Rates:** Up to 384 kHz
**Bit Depth:** 16/24/32-bit

**I2S Pin Configuration:**
| Signal | GPIO | Description |
|--------|------|-------------|
| BCLK | 48 | Bit Clock |
| LRC | 38 | Left/Right Clock (Word Select) |
| DOUT | 47 | Data Output |

### Supported Audio Formats

- ✅ **MP3** - MPEG Audio Layer 3 (Most common)
- ✅ **WAV** - Waveform Audio Format (Uncompressed)
- ✅ **AAC** - Advanced Audio Coding
- ✅ **M4A** - MPEG-4 Audio
- ✅ **FLAC** - Free Lossless Audio Codec
- ✅ **OGG** - Ogg Vorbis
- ✅ **OPUS** - Opus Audio Codec

**Recommended Format:** MP3 (320 kbps or lower for best compatibility)

---

## 🎼 Audio Playback Functions

### Initialize Audio System

Already called in `setup()`:

```cpp
Audio_Init();  // Initializes I2S and PCM5101A DAC
```

### Volume Control

```cpp
// Set volume (0-21)
Volume_adjustment(15);  // Medium volume

// Global variable
extern uint8_t Volume;
Volume = 10;  // Direct access
```

**Volume Levels:**
- `0` - Muted
- `10-12` - Low
- `15-18` - Medium (recommended)
- `21` - Maximum

### Play Music File

**Method 1: Play Specific File**

```cpp
void Play_Music(const char* directory, const char* fileName);

// Examples:
Play_Music("/", "song.mp3");           // Root directory
Play_Music("/music", "track01.mp3");   // In /music folder
Play_Music("/albums/rock", "best.mp3"); // Nested folders
```

**Method 2: Quick Test (plays /A.mp3)**

```cpp
void Play_Music_test();

// Usage:
Play_Music_test();  // Plays /A.mp3 if it exists
```

### Playback Control

**Pause/Resume:**

```cpp
// Pause playback
Music_pause();

// Resume playback
Music_resume();

// Toggle pause/resume
audio.pauseResume();
```

**Stop Playback:**

```cpp
audio.stopSong();
```

### Get Audio Information

**Duration:**

```cpp
uint32_t Music_Duration();

// Returns duration in seconds
uint32_t duration = Music_Duration();
if (duration > 60) {
    Serial.printf("Duration: %d:%02d\n", duration / 60, duration % 60);
} else {
    Serial.printf("Duration: %d seconds\n", duration);
}
```

**Current Playback Position:**

```cpp
uint32_t Music_Elapsed();

// Returns elapsed time in seconds
uint32_t elapsed = Music_Elapsed();
Serial.printf("Elapsed: %d seconds\n", elapsed);
```

**Audio Properties:**

```cpp
uint32_t sampleRate = audio.getSampleRate();
uint8_t bitDepth = audio.getBitsPerSample();
uint8_t channels = audio.getChannels();
uint32_t bitrate = audio.getBitRate();

Serial.printf("Sample Rate: %d Hz\n", sampleRate);
Serial.printf("Bit Depth: %d bits\n", bitDepth);
Serial.printf("Channels: %d\n", channels);
Serial.printf("Bitrate: %d kbps\n", bitrate / 1000);
```

### Check Playback Status

```cpp
if (audio.isRunning()) {
    Serial.println("Music is playing");
} else {
    Serial.println("Music is stopped");
}
```

---

## 🎯 Complete Examples

### Example 1: Simple Music Player

```cpp
void playMusicFromSD() {
    // Check SD card
    if (SD_MMC.cardType() == CARD_NONE) {
        Serial.println("No SD card");
        return;
    }
    
    // Check if file exists
    if (!SD_MMC.exists("/song.mp3")) {
        Serial.println("File not found");
        return;
    }
    
    // Set volume
    Volume_adjustment(15);
    
    // Play music
    Play_Music("/", "song.mp3");
    
    // Show duration
    delay(1000);  // Wait for file to be read
    uint32_t duration = Music_Duration();
    Serial.printf("Playing: song.mp3 (%d:%02d)\n", 
                  duration / 60, duration % 60);
}
```

### Example 2: Playlist Player

```cpp
void playPlaylist() {
    char playlist[50][100];
    uint16_t count = Folder_retrieval("/music", ".mp3", playlist, 50);
    
    if (count == 0) {
        Serial.println("No MP3 files found");
        return;
    }
    
    Serial.printf("Found %d songs\n", count);
    
    for (int i = 0; i < count; i++) {
        Serial.printf("Playing %d/%d: %s\n", i + 1, count, playlist[i]);
        
        Play_Music("/music", playlist[i]);
        
        // Wait for song to finish (simplified)
        uint32_t duration = Music_Duration();
        delay(duration * 1000);
        
        // Small pause between songs
        delay(1000);
    }
    
    Serial.println("Playlist finished!");
}
```

### Example 3: Progress Monitor

```cpp
void monitorPlayback() {
    if (!audio.isRunning()) {
        Serial.println("No music playing");
        return;
    }
    
    uint32_t duration = Music_Duration();
    
    while (audio.isRunning()) {
        uint32_t elapsed = Music_Elapsed();
        float progress = (float)elapsed / duration * 100.0;
        
        Serial.printf("\r[");
        int barWidth = 40;
        int pos = barWidth * elapsed / duration;
        for (int i = 0; i < barWidth; i++) {
            if (i < pos) Serial.print("=");
            else if (i == pos) Serial.print(">");
            else Serial.print(" ");
        }
        Serial.printf("] %d:%02d / %d:%02d (%.0f%%)", 
                     elapsed / 60, elapsed % 60,
                     duration / 60, duration % 60,
                     progress);
        
        delay(1000);
    }
    Serial.println("\nPlayback finished");
}
```

### Example 4: Audio with UI Button

```cpp
static lv_obj_t * play_button;
static bool is_playing = false;

static void play_button_handler(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (is_playing) {
            Music_pause();
            lv_label_set_text(lv_obj_get_child(play_button, 0), "Play");
            is_playing = false;
        } else {
            if (audio.isRunning()) {
                Music_resume();
            } else {
                Play_Music("/", "song.mp3");
            }
            lv_label_set_text(lv_obj_get_child(play_button, 0), "Pause");
            is_playing = true;
        }
    }
}

void createMusicUI(lv_obj_t * parent) {
    play_button = lv_btn_create(parent);
    lv_obj_set_size(play_button, 120, 50);
    lv_obj_center(play_button);
    
    lv_obj_t * label = lv_label_create(play_button);
    lv_label_set_text(label, "Play");
    lv_obj_center(label);
    
    lv_obj_add_event_cb(play_button, play_button_handler, LV_EVENT_CLICKED, NULL);
}
```

---

## 🎚️ Audio Callbacks (Advanced)

The Audio library supports callback functions for various events:

### Available Callbacks

```cpp
// Called when MP3 file ends
void audio_eof_mp3(const char *info) {
    Serial.println("MP3 playback finished");
}

// Called for general audio info
void audio_info(const char *info) {
    Serial.print("Audio Info: ");
    Serial.println(info);
}

// Called for ID3 tag data (artist, title, etc.)
void audio_id3data(const char *info) {
    Serial.print("ID3 Tag: ");
    Serial.println(info);
}

// Called with bitrate information
void audio_bitrate(const char *info) {
    Serial.print("Bitrate: ");
    Serial.println(info);
}
```

---

## 📝 Best Practices

### SD Card

✅ **Do:**
- Use high-quality, brand-name SD cards
- Format as FAT32 for cards >32GB
- Safely eject before removing
- Keep filenames under 32 characters
- Use lowercase for better compatibility
- Organize music into folders

❌ **Don't:**
- Remove card while reading/writing
- Use special characters in filenames (stick to A-Z, 0-9, -, _)
- Exceed 32GB without proper formatting
- Use exFAT or NTFS formats

### Audio Playback

✅ **Do:**
- Use MP3 format for best compatibility
- Keep bitrate ≤ 320 kbps
- Set appropriate volume (15-18 recommended)
- Call `audio.loop()` regularly (done automatically in timer)
- Wait 1 second after Play_Music() before checking duration

❌ **Don't:**
- Play extremely high bitrate files (> 320 kbps)
- Change files while playing without stopping first
- Set volume to maximum (21) - may cause distortion
- Block the main loop for too long

---

## 🐛 Troubleshooting

### SD Card Issues

**Problem:** "SD card initialization failed!"
- ✓ Check if SD card is properly inserted
- ✓ Verify card is FAT32 formatted
- ✓ Try a different SD card
- ✓ Check if card is write-protected

**Problem:** "No SD card attached"
- ✓ Card may be inserted incorrectly
- ✓ Card slot may have poor contact
- ✓ Try reinserting the card

**Problem:** Files not found
- ✓ Check filename exactly (case-sensitive on some systems)
- ✓ Ensure file is in correct directory
- ✓ Verify file isn't corrupted
- ✓ Check file extension (.mp3, not .MP3)

### Audio Playback Issues

**Problem:** No sound
- ✓ Check volume: `Volume_adjustment(15)`
- ✓ Verify speaker is connected
- ✓ Check if file format is supported
- ✓ Ensure audio.loop() is being called
- ✓ Check Serial Monitor for error messages

**Problem:** "Music Read Failed"
- ✓ File may be corrupted
- ✓ File format may not be supported
- ✓ Try a different MP3 file
- ✓ Ensure file is not copy-protected

**Problem:** Distorted sound
- ✓ Reduce volume
- ✓ Check if bitrate is too high
- ✓ Verify speaker connections
- ✓ Try a different audio file

**Problem:** Playback stops unexpectedly
- ✓ Check SD card connection
- ✓ Verify file isn't corrupted
- ✓ Check for system crashes in Serial Monitor
- ✓ Ensure sufficient power supply

---

## 📊 File Format Recommendations

| Format | Bitrate | Quality | File Size | Compatibility |
|--------|---------|---------|-----------|---------------|
| MP3 128 kbps | Low | Good | Small | ✅ Excellent |
| MP3 192 kbps | Medium | Very Good | Medium | ✅ Excellent |
| MP3 320 kbps | High | Excellent | Large | ✅ Excellent |
| WAV 16-bit | Uncompressed | Perfect | Very Large | ✅ Good |
| FLAC | Lossless | Perfect | Large | ✅ Good |
| AAC 256 kbps | High | Excellent | Medium | ⚠️ May vary |

**Recommended:** MP3 @ 192-320 kbps for best balance of quality and compatibility

---

## 🔗 API Reference Quick Link

### Key Functions

| Function | Description | Return |
|----------|-------------|--------|
| `SD_Init()` | Initialize SD card | void |
| `File_Search(dir, file)` | Check if file exists | bool |
| `Folder_retrieval(dir, ext, array, max)` | List files | uint16_t |
| `Audio_Init()` | Initialize audio system | void |
| `Play_Music(dir, file)` | Play audio file | void |
| `Play_Music_test()` | Play /A.mp3 | void |
| `Volume_adjustment(vol)` | Set volume 0-21 | void |
| `Music_pause()` | Pause playback | void |
| `Music_resume()` | Resume playback | void |
| `Music_Duration()` | Get song length | uint32_t |
| `Music_Elapsed()` | Get current position | uint32_t |
| `audio.isRunning()` | Check if playing | bool |
| `audio.stopSong()` | Stop playback | uint32_t |

---

## 📚 Related Documentation

- [LVGL Tab Design Guide](LVGL_TAB_DESIGN_GUIDE.md) - Create music player UI
- [Flash Storage Guide](FLASH_STORAGE_GUIDE.md) - Save playlists and settings
- [Main Project README](../../../../../README.md) - Project overview

---

## 💡 Example: Complete Music Player Tab

See `LVGL_Music.cpp` and `LVGL_Music.h` for a complete implementation of a music player UI with:
- Play/Pause buttons
- Progress bar
- Track information display
- Playlist navigation
- Volume control

---

*SD Card and Audio Guide for ESP32-S3 Touch LCD 1.85" - Making music playback simple!* 🎵📀
