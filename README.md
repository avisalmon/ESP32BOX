# ESP32-S3 Touch LCD 1.85" Development Project

A comprehensive development environment for the **ESP32-S3 Touch LCD 1.85"** circular display (360x360 pixels) with touch input, WiFi, Bluetooth, audio, and sensor capabilities.

![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Supported-green)
![LVGL](https://img.shields.io/badge/LVGL-v8.3.0-blue)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-orange)
![ESP-IDF](https://img.shields.io/badge/ESP--IDF-Compatible-red)

---

## 🎯 Features

### Hardware Capabilities
- **Display:** 1.85" Circular TFT LCD (360x360) - ST77916 Driver
- **Touch:** Capacitive touch sensor - CST816 Controller
- **MCU:** ESP32-S3 (Dual-core Xtensa LX7, WiFi, Bluetooth)
- **Audio:** PCM5101A DAC + MEMS Microphone (MSM)
- **Storage:** SD Card slot + 8MB Flash (NVS + LittleFS)
- **Sensors:** Battery management, RTC (PCF85063)
- **I/O:** GPIO expander (TCA9554PWR)

### Software Features
- ✅ **WiFi Manager** with flash-based password storage
- ✅ **LVGL UI Framework** with tabbed interface
- ✅ **Audio Playback** (MP3, WAV via I2S)
- ✅ **Touch Input** with gesture support
- ✅ **Persistent Storage** (Preferences/NVS + LittleFS)
- ✅ **Real-Time Clock** with time synchronization
- ✅ **Battery Monitoring**
- ✅ **Wireless Communication** (WiFi + Bluetooth)

---

## 📁 Project Structure

```
ESP32BOX/
├── DEMO/
│   ├── Arduino/                          # Arduino IDE Projects
│   │   ├── examples/
│   │   │   └── LVGL_Arduino/            # Main Arduino Project ⭐
│   │   │       ├── docs/                # 📚 Complete Documentation
│   │   │       ├── *.cpp/h              # Source files
│   │   │       └── LVGL_Arduino.ino     # Main sketch
│   │   └── libraries/                   # Required libraries
│   │       ├── lvgl/                    # LVGL v8.3.0
│   │       └── ESP32-audioI2S-master/   # Audio library
│   │
│   ├── ESP-IDF/                         # ESP-IDF Projects
│   │   └── ESP32-S3-Touch-LCD-1.85C-Test/
│   │
│   ├── Firmware/                        # Pre-built firmware binaries
│   │   ├── Factory firmware/
│   │   └── Phone To DLNA/
│   │
│   └── ReadMe.txt                       # Original manufacturer documentation
│
└── README.md                            # This file
```

---

## 📚 Documentation

### **🎨 [LVGL Tab Design Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/LVGL_TAB_DESIGN_GUIDE.md)**
**Complete guide for building custom user interface tabs**

**2,100+ lines** covering:
- Tab creation step-by-step tutorial
- All LVGL widgets (buttons, sliders, charts, arcs, keyboards, etc.)
- Grid layout system and positioning
- Event handling and animations
- Advanced widgets (charts, gauges, message boxes)
- Colors, styles, and symbols
- Best practices and debugging

**Perfect for:** Creating custom UI screens on the circular display

---

### **💾 [Flash Storage Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/FLASH_STORAGE_GUIDE.md)**
**Complete guide for ESP32 non-volatile storage**

**846 lines** covering:
- **Preferences Library (NVS):** Key-value storage with examples
- **LittleFS:** Modern filesystem for ESP32
- WiFi password storage implementation
- Settings persistence patterns
- Best practices and limitations

**Perfect for:** Saving settings, WiFi credentials, user preferences

---

### **🧪 [Flash Storage Testing Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/TEST_FLASH_STORAGE.md)**
**Validation and debugging guide for flash storage**

Quick reference for:
- Expected Serial Monitor output
- Verification steps
- Common issues and solutions

---

### **🎵 [SD Card & Audio Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/SD_CARD_AUDIO_GUIDE.md)**
**Complete guide for SD card interface and audio playback**

**600+ lines** covering:
- **SD Card Interface:** Hardware specs, file operations, card management
- **Audio Playback:** PCM5101A DAC, supported formats (MP3, WAV, AAC, FLAC, OGG, OPUS)
- **Volume Control:** 0-21 levels, optimal settings
- **Playback Functions:** Play, pause, resume, duration, elapsed time
- **Complete Examples:** Music player, playlist, progress monitor, UI integration
- **Best Practices:** SD handling, audio optimization, troubleshooting

**Perfect for:** Music players, audio feedback, sound effects, voice playback

**Tested and Working:** Audio playback confirmed operational! 🔊✅

---

### **📖 [Documentation Index](DEMO/Arduino/examples/LVGL_Arduino/docs/README.md)**
**Complete documentation hub with quick start guides and cross-references**

---

## 🚀 Quick Start

### Option 1: Arduino IDE (Recommended for Beginners)

1. **Install Arduino IDE** (version 2.0+)

2. **Install ESP32 Board Support:**
   - Go to: File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
     ```
   - Go to: Tools → Board → Boards Manager
   - Search for "esp32" and install "esp32 by Espressif Systems"

3. **Open the Project:**
   ```
   File → Open → DEMO/Arduino/examples/LVGL_Arduino/LVGL_Arduino.ino
   ```

4. **Configure Board:**
   - **Board:** "ESP32S3 Dev Module"
   - **USB CDC On Boot:** "Enabled"
   - **Flash Size:** "8MB"
   - **Partition Scheme:** "Default 4MB with spiffs"
   - **Upload Speed:** "921600"

5. **Install Required Libraries:**
   The project includes libraries in `DEMO/Arduino/libraries/`:
   - LVGL v8.3.0
   - ESP32-audioI2S

   Copy these to your Arduino libraries folder or add to library path.

6. **Configure WiFi (Optional):**
   - Edit `WiFi_Config.h` or let the system use flash storage
   - Default passwords are stored in flash on first boot

7. **Compile and Upload:**
   - Connect your ESP32-S3 device via USB
   - Click Upload button

8. **Monitor Serial Output:**
   - Open Serial Monitor (115200 baud)
   - Watch for WiFi connection and system initialization

---

### Option 2: ESP-IDF (Advanced Users)

1. **Install ESP-IDF** (version 4.4+)

2. **Open Project:**
   ```bash
   cd DEMO/ESP-IDF/ESP32-S3-Touch-LCD-1.85C-Test
   ```

3. **Configure:**
   ```bash
   idf.py menuconfig
   ```

4. **Build and Flash:**
   ```bash
   idf.py build
   idf.py flash monitor
   ```

---

### Option 3: Pre-built Firmware

Flash pre-compiled binaries using **flash_download_tool_3.9.5**:

1. Download the tool from Espressif
2. Select firmware from `DEMO/Firmware/Factory firmware/`
3. Set flash address to `0x00`
4. Check the checkbox and click "START"

---

## 💡 Key Components

### WiFi Manager
**Files:** `WiFi_Manager.cpp/h`

Features:
- Priority-based network scanning (Hot1 → AviRedmi → others)
- Flash-based password storage (encrypted NVS)
- Automatic reconnection
- Signal strength monitoring

**See:** [Flash Storage Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/FLASH_STORAGE_GUIDE.md) for implementation details

### LVGL UI System
**Files:** `LVGL_Example.cpp/h`, `LVGL_Music.cpp/h`

Features:
- Tabbed interface with swipe gestures
- Custom widgets and layouts
- Grid-based positioning
- Event-driven architecture

**See:** [LVGL Tab Design Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/LVGL_TAB_DESIGN_GUIDE.md) for creating custom tabs

### Audio System
**Files:** `Audio_PCM5101.cpp/h`, `MIC_MSM.cpp/h`

Features:
- I2S audio output via PCM5101A DAC
- MP3/WAV playback via ESP32-audioI2S library
- MEMS microphone input
- Volume control

### Touch Input
**Files:** `Touch_CST816.cpp/h`

Features:
- Single-point capacitive touch
- Gesture detection
- LVGL integration

---

## 🛠️ Development Tips

### Adding a New UI Tab

```cpp
// 1. Declare function in LVGL_Example.cpp
static void MyTab_create(lv_obj_t * parent);

// 2. Add tab in Lvgl_Example1()
lv_obj_t * t_new = lv_tabview_add_tab(tv, "MyTab");
MyTab_create(t_new);

// 3. Implement function
static void MyTab_create(lv_obj_t * parent) {
    lv_obj_t * panel = lv_obj_create(parent);
    // Add your widgets here
}
```

**Full details:** [LVGL Tab Design Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/LVGL_TAB_DESIGN_GUIDE.md)

### Saving Settings to Flash

```cpp
#include <Preferences.h>
#include <nvs_flash.h>

Preferences preferences;

void setup() {
    // Initialize NVS
    nvs_flash_init();
    
    // Save data
    preferences.begin("settings", false);
    preferences.putInt("brightness", 80);
    preferences.putString("name", "MyDevice");
    preferences.end();
    
    // Load data
    preferences.begin("settings", true);
    int brightness = preferences.getInt("brightness", 50);
    String name = preferences.getString("name", "Default");
    preferences.end();
}
```

**Full details:** [Flash Storage Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/FLASH_STORAGE_GUIDE.md)

---

## 📊 System Status

### Current Implementation
- ✅ WiFi connectivity with priority networks
- ✅ Flash-based password storage (NVS)
- ✅ LVGL UI with 2+ tabs (Onboard, Music)
- ✅ Touch input integration
- ✅ Audio playback support
- ✅ Battery monitoring
- ✅ RTC integration
- ✅ SD card support

### Recent Updates
- **Oct 5, 2025:** Added comprehensive documentation (2,100+ lines)
- **Oct 5, 2025:** Implemented flash storage for WiFi passwords
- **Oct 5, 2025:** Fixed NVS initialization for Preferences library
- **Oct 5, 2025:** Organized documentation into docs/ folder

---

## 🔧 Hardware Pinout Reference

| Component | Pin | Description |
|-----------|-----|-------------|
| **Display** | SPI | ST77916 driver |
| **Touch** | I2C | CST816 controller |
| **Audio Out** | I2S | PCM5101A DAC |
| **Microphone** | I2S | MEMS MSM |
| **SD Card** | SPI | Storage expansion |
| **RTC** | I2C | PCF85063 |
| **GPIO Exp** | I2C | TCA9554PWR |
| **Battery** | ADC | Voltage monitoring |

**Detailed pinouts:** See source files in `DEMO/Arduino/examples/LVGL_Arduino/`

---

## 🐛 Troubleshooting

### Compilation Errors
- Ensure ESP32 board support is installed
- Verify LVGL library is in correct location
- Check `lv_conf.h` is present in project folder

### WiFi Not Connecting
- Check Serial Monitor for error messages
- Verify password in flash storage
- Signal strength must be adequate
- See `WiFi_Manager.cpp` for debugging output

### Flash Storage Issues
- Ensure `nvs_flash_init()` is called before Preferences
- Check `preferences.begin()` return value
- NVS partition must exist in partition table
- See [Flash Storage Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/FLASH_STORAGE_GUIDE.md)

### Display Issues
- Verify SPI connections
- Check display initialization in `Display_ST77916.cpp`
- LVGL buffer size in `lv_conf.h`

---

## 📦 Dependencies

### Required Libraries (Included)
- **LVGL** v8.3.0 - Graphics library
- **ESP32-audioI2S** - Audio playback

### Built-in ESP32 Libraries (No installation needed)
- WiFi.h
- Preferences.h
- nvs_flash.h
- SPI.h
- Wire.h (I2C)
- SD.h

---

## 🤝 Contributing

To add or improve documentation:
1. Place `.md` files in `DEMO/Arduino/examples/LVGL_Arduino/docs/`
2. Update the [Documentation Index](DEMO/Arduino/examples/LVGL_Arduino/docs/README.md)
3. Follow existing formatting conventions
4. Include practical code examples
5. Test all code on hardware before documenting

---

## 📄 License

This project contains multiple components with various licenses:
- **LVGL:** MIT License
- **ESP32-audioI2S:** GPL-3.0
- **ESP32 Arduino Core:** LGPL-2.1
- **Project-specific code:** Check individual file headers

---

## 🔗 External Resources

- **LVGL Documentation:** <https://docs.lvgl.io/8.3/>
- **ESP32-S3 Datasheet:** <https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf>
- **ESP32 Arduino Core:** <https://github.com/espressif/arduino-esp32>
- **ESP-IDF Documentation:** <https://docs.espressif.com/projects/esp-idf/>
- **LVGL Forum:** <https://forum.lvgl.io/>

---

## 📞 Support

For hardware-specific questions, refer to the manufacturer's documentation.

For software development:
1. Check the [Documentation](DEMO/Arduino/examples/LVGL_Arduino/docs/)
2. Review working examples in source files
3. Check Serial Monitor output for debugging info
4. See LVGL forum for UI-related questions

---

## ⭐ Quick Links

- **📚 [Complete Documentation Hub](DEMO/Arduino/examples/LVGL_Arduino/docs/README.md)**
- **🎨 [UI Development Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/LVGL_TAB_DESIGN_GUIDE.md)**
- **💾 [Flash Storage Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/FLASH_STORAGE_GUIDE.md)**
- **🧪 [Testing Guide](DEMO/Arduino/examples/LVGL_Arduino/docs/TEST_FLASH_STORAGE.md)**
- **💻 [Main Arduino Project](DEMO/Arduino/examples/LVGL_Arduino/)**

---

*ESP32-S3 Touch LCD 1.85" Development Project - Making circular displays beautiful!* 🎨✨
