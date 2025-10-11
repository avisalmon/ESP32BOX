# ESP32-S3 Touch LCD Documentation

This folder contains comprehensive documentation for developing with the ESP32-S3 Touch LCD 1.85" (360x360 circular display).

## 📚 Documentation Index

### [LVGL_TAB_DESIGN_GUIDE.md](LVGL_TAB_DESIGN_GUIDE.md)
**Complete guide for building custom LVGL user interface tabs**

**Contents:**
- Tab structure and architecture
- Step-by-step tab creation tutorial
- Basic widgets (labels, buttons, sliders, switches, checkboxes, textareas, dropdowns, button matrices)
- Object positioning and alignment
- Grid layout system
- Animations and transitions
- Event handling
- Timers and periodic updates
- Colors, opacity, and symbols
- **Advanced widgets:**
  - Charts (line and bar charts)
  - Arc (circular progress/gauge)
  - Spinbox (numeric input)
  - Roller (scrollable picker)
  - LED indicators
  - Message boxes (modal dialogs)
  - Keyboard (on-screen input)
- Drawing and canvas operations
- Object flags and states
- Best practices and debugging tips

**2,100+ lines** | Perfect for circular display optimization

---

### [FLASH_STORAGE_GUIDE.md](FLASH_STORAGE_GUIDE.md)
**Complete guide for ESP32 non-volatile storage systems**

**Contents:**
- **Preferences Library (NVS):**
  - NVS initialization and error handling
  - Key-value storage for all data types
  - Namespaces for data organization
  - Read/write operations with examples
  - WiFi password storage implementation
  - Best practices and limitations
  
- **LittleFS Filesystem:**
  - Modern filesystem for ESP32
  - File operations (create, read, write, delete)
  - Directory management
  - File listing and info retrieval
  - Binary and text file handling
  
- **Comparison Table:** Preferences vs LittleFS
- **Practical Examples:** Settings persistence, configuration storage
- **Integration Examples:** Complete working code

**846 lines** | Production-ready code examples

---

### [TEST_FLASH_STORAGE.md](TEST_FLASH_STORAGE.md)
**Flash storage testing and validation guide**

**Contents:**
- Expected Serial Monitor output
- Verification steps for flash persistence
- Debugging checklist
- Common issues and solutions
- Reboot persistence testing

Quick reference for validating flash storage implementations.

---

### [SD_CARD_AUDIO_GUIDE.md](SD_CARD_AUDIO_GUIDE.md)
**Complete guide for SD card interface and audio playback**

**Contents:**
- **SD Card Interface:**
  - Hardware specifications and pin configuration
  - Supported card types (SD, SDHC, MMC)
  - File operations (read, write, search, list)
  - Card information and formatting
  
- **Audio Playback System:**
  - PCM5101A DAC specifications
  - Supported formats (MP3, WAV, AAC, FLAC, OGG, OPUS)
  - Volume control and playback functions
  - Audio callbacks and event handling
  
- **Complete Examples:**
  - Simple music player
  - Playlist player
  - Progress monitor
  - UI integration with buttons
  
- **Best Practices:** SD card handling, audio optimization
- **Troubleshooting:** Common issues and solutions
- **API Reference:** Quick function lookup table

**600+ lines** | Full audio system documentation

**Perfect for:** Music player apps, audio feedback, sound effects

---

## 🚀 Quick Start

### Adding a New Tab
1. Read: [LVGL_TAB_DESIGN_GUIDE.md](LVGL_TAB_DESIGN_GUIDE.md) - Section "How to Add a New Tab"
2. Use the Quick Reference Checklist
3. Copy/paste example code from relevant widget sections
4. Test on hardware

### Implementing Flash Storage
1. Read: [FLASH_STORAGE_GUIDE.md](FLASH_STORAGE_GUIDE.md) - Setup Requirements
2. Initialize NVS with `nvs_flash_init()`
3. Use Preferences for key-value storage
4. Validate with [TEST_FLASH_STORAGE.md](TEST_FLASH_STORAGE.md)
5. See `WiFi_Manager.cpp` for real-world example

### Playing Audio from SD Card
1. Read: [SD_CARD_AUDIO_GUIDE.md](SD_CARD_AUDIO_GUIDE.md) - SD Card Setup
2. Insert SD card (FAT32 formatted)
3. Put MP3 files on card
4. Use `Play_Music("/", "song.mp3")` to play
5. Control with `Music_pause()` / `Music_resume()`
6. See `Audio_PCM5101.cpp` and `LVGL_Music.cpp` for examples

---

## 📖 Related Files

### Working Examples in Main Project
- **`WiFi_Manager.cpp/h`** - Production flash storage implementation
  - NVS initialization
  - Password persistence
  - Error handling
  - Verification steps

- **`LVGL_Example.cpp`** - Tab creation and widget examples
  - Onboard tab (system info)
  - Music tab (complex UI)
  - Grid layouts
  - Event handlers

- **`LVGL_Music.cpp`** - Advanced UI example
  - Audio player interface
  - Complex interactions
  - State management

- **`Audio_PCM5101.cpp/h`** - Audio playback system
  - PCM5101A DAC initialization
  - Music playback functions
  - Volume control
  - Duration and elapsed time

- **`SD_Card.cpp/h`** - SD card interface
  - Card initialization
  - File search and listing
  - Directory operations

### Configuration Files
- **`WiFi_Config_DEPRECATED.txt`** - Migration notice (old hardcoded system)
- **`lv_conf.h`** - LVGL library configuration
- **`password.json.example`** - Example configuration structure

---

## 🛠️ Development Environment

- **Platform:** ESP32-S3
- **Display:** 1.85" Circular Touch LCD (360x360)
- **UI Library:** LVGL v8.3.0
- **Storage:** NVS (Non-Volatile Storage) + LittleFS
- **IDE:** Arduino IDE / PlatformIO

---

## 📝 Document Status

| Document | Lines | Status | Last Updated |
|----------|-------|--------|--------------|
| LVGL_TAB_DESIGN_GUIDE.md | 2,100+ | ✅ Complete | Oct 5, 2025 |
| FLASH_STORAGE_GUIDE.md | 846 | ✅ Complete | Oct 4, 2025 |
| TEST_FLASH_STORAGE.md | ~100 | ✅ Complete | Oct 4, 2025 |
| SD_CARD_AUDIO_GUIDE.md | 600+ | ✅ Complete | Oct 11, 2025 |

---

## 🤝 Contributing

When adding new documentation:
1. Place `.md` files in this `docs/` folder
2. Update this README.md index
3. Use clear section headers and code examples
4. Include practical, tested code snippets
5. Add to the Document Status table

---

## 🔗 External Resources

- **LVGL Documentation:** <https://docs.lvgl.io/8.3/>
- **ESP32 Arduino Core:** <https://github.com/espressif/arduino-esp32>
- **Preferences Library:** <https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences>
- **LVGL Forum:** <https://forum.lvgl.io/>

---

*Documentation for ESP32-S3 Touch LCD 1.85" Project*
