# ESP32-S3-Touch-LCD-1.85C Development Project

![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Touch%20LCD-blue)
![Arduino](https://img.shields.io/badge/Arduino-IDE-green)
![LVGL](https://img.shields.io/badge/LVGL-8.3.10-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

## Overview

Comprehensive development project for the **ESP32-S3-Touch-LCD-1.85C** development board featuring a 360x360 circular touch display, audio capabilities, and extensive I/O options.

### 🌟 Key Features

- **Display**: 1.85" 360x360 circular touch LCD with ST77916 controller
- **Touch**: CST816T capacitive touch with gesture recognition
- **Audio**: PCM5101 DAC for high-quality audio output
- **Microphone**: MSM261S4030H0 for audio input and recording
- **Storage**: SD card support via SDIO interface
- **Connectivity**: WiFi 802.11 b/g/n with web server capabilities
- **Power**: Battery monitoring and advanced power management
- **RTC**: PCF85063 real-time clock with alarm functionality
- **I/O Expansion**: TCA9554PWR I2C I/O expander for additional GPIO

## 🚀 Quick Start

### Prerequisites

1. **Arduino IDE 2.0.2+** with ESP32 package installed
2. **ESP32-S3-Touch-LCD-1.85C** development board
3. **USB-C cable** for programming and power
4. **MicroSD card** (optional, for storage)

### Arduino IDE Configuration

```
Board: ESP32S3 Dev Module
Upload Speed: 921600
USB Mode: Hardware CDC and JTAG
CPU Frequency: 240MHz (WiFi/BT)
Flash Size: 16MB (128Mb)
Partition Scheme: 16M Flash (3MB APP/9.9MB FATFS)
PSRAM: Enabled
```

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/ESP32BOX.git
   cd ESP32BOX
   ```

2. **Install required libraries** (see `requirements.txt`)

3. **Configure credentials**:
   ```bash
   cp credentials.h.template credentials.h
   # Edit credentials.h with your WiFi and API credentials
   ```

4. **Open project in Arduino IDE**:
   - Open `sketch_oct2b/sketch_oct2b.ino`
   - Select correct board and port
   - Upload to device

## 📁 Project Structure

```
ESP32BOX/
├── Documentation/           # Comprehensive system documentation
│   ├── 00_System_Overview.md   # Hardware specs and setup
│   ├── 01_Display_System.md    # Display and LVGL integration
│   ├── 02_Touch_Controller.md  # Touch interface and gestures
│   ├── 03_Audio_System.md      # Audio playback system
│   ├── 04_Microphone_System.md # Audio recording system
│   ├── 05_SD_Card_System.md    # File system and storage
│   ├── 06_Battery_Monitoring.md # Power monitoring
│   ├── 07_RTC_System.md        # Real-time clock
│   ├── 08_WiFi_System.md       # WiFi and networking
│   ├── 09_Power_Management.md  # Power control system
│   └── 10_External_IO_Connections.md # I/O expansion
├── sketch_oct2b/            # Main Arduino sketch
│   └── sketch_oct2b.ino       # Main application code
├── credentials.h.template   # Credentials template file
├── requirements.txt         # Project dependencies
├── .gitignore              # Git ignore rules
└── README.md               # This file
```

## 🛠️ Hardware Specifications

### Main Controller
- **MCU**: ESP32-S3 (Dual-core Xtensa LX7, 240MHz)
- **Flash**: 16MB
- **PSRAM**: 8MB
- **WiFi**: 802.11 b/g/n (2.4GHz)
- **Bluetooth**: 5.0 LE

### Display System
- **Screen**: 1.85" circular TFT LCD
- **Resolution**: 360x360 pixels
- **Controller**: ST77916
- **Interface**: QSPI
- **Backlight**: GPIO 5 controlled

### Touch Interface
- **Controller**: CST816T
- **Type**: Capacitive touch
- **Features**: Multi-touch, gesture recognition
- **Interface**: I2C (address 0x15)

### Audio System
- **DAC**: PCM5101 (24-bit, 192kHz)
- **Microphone**: MSM261S4030H0
- **Interface**: I2S
- **Features**: High-quality playback and recording

### Storage & Connectivity
- **SD Card**: SDIO interface, up to 32GB
- **WiFi**: Built-in ESP32-S3
- **RTC**: PCF85063 with battery backup
- **I/O Expander**: TCA9554PWR (8 additional GPIO)

## 📖 Documentation

Comprehensive documentation is available in the `Documentation/` folder:

- **[System Overview](Documentation/00_System_Overview.md)** - Complete hardware specifications
- **[Display System](Documentation/01_Display_System.md)** - LVGL graphics and display control
- **[Touch Controller](Documentation/02_Touch_Controller.md)** - Touch input and gesture handling
- **[Audio System](Documentation/03_Audio_System.md)** - Audio playback and processing
- **[Microphone System](Documentation/04_Microphone_System.md)** - Audio recording capabilities
- **[SD Card System](Documentation/05_SD_Card_System.md)** - File system and storage management
- **[Battery Monitoring](Documentation/06_Battery_Monitoring.md)** - Power monitoring and management
- **[RTC System](Documentation/07_RTC_System.md)** - Real-time clock and timekeeping
- **[WiFi System](Documentation/08_WiFi_System.md)** - Network connectivity and web services
- **[Power Management](Documentation/09_Power_Management.md)** - Advanced power control
- **[External I/O](Documentation/10_External_IO_Connections.md)** - GPIO expansion and interfacing

## 💡 Example Applications

### Basic Display Test
```cpp
#include "credentials.h"

void setup() {
    Serial.begin(115200);
    
    // Initialize display
    Display_Init();
    
    // Initialize LVGL
    LVGL_Init();
    
    // Create simple UI
    create_main_ui();
}

void loop() {
    lv_task_handler();
    delay(5);
}
```

### WiFi Web Server
```cpp
#include "credentials.h"
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

void setup() {
    // Connect to WiFi
    WiFi.begin(WIFI_SSID_PRIMARY, WIFI_PASSWORD_PRIMARY);
    
    // Setup web server
    server.on("/", handleRoot);
    server.begin();
}

void loop() {
    server.handleClient();
}
```

### Audio Playback
```cpp
void setup() {
    // Initialize audio system
    Audio_Init();
    
    // Play startup sound
    Audio_Play_File("/startup.wav");
}
```

## 🔧 Development Tips

### Avoiding Common Issues

1. **Watchdog Timer**: Use `delay()` or `yield()` in loops to prevent watchdog resets
2. **Power Management**: Monitor battery voltage for low-power conditions
3. **I2C Conflicts**: Check device addresses before adding new I2C devices
4. **Memory Management**: Use PSRAM for large buffers and graphics

### Performance Optimization

- Enable PSRAM for LVGL buffers
- Use DMA for SPI/I2S transfers
- Implement proper task scheduling
- Optimize graphics for circular display

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow existing code style and naming conventions
- Add comprehensive documentation for new features
- Test on actual hardware before submitting
- Update relevant documentation files
- Include example code for new functionality

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **LVGL Team** for the excellent graphics library
- **Espressif Systems** for the ESP32-S3 platform
- **Arduino Community** for the development environment
- **Hardware Manufacturer** for the development board

## 📞 Support

- **Issues**: Please use GitHub Issues for bug reports
- **Documentation**: Check the Documentation folder first
- **Community**: Join discussions in GitHub Discussions
- **Examples**: See the sketch_oct2b folder for working code

## 🔄 Version History

- **v1.0.0** (October 2024) - Initial release with full system documentation
- **v0.9.0** - Beta release with basic functionality
- **v0.5.0** - Alpha release with display and touch support

## 🏗️ Roadmap

- [ ] Advanced LVGL widgets and themes
- [ ] Bluetooth Low Energy support
- [ ] Advanced audio processing features
- [ ] Machine learning integration
- [ ] Cloud service integrations
- [ ] Mobile app companion
- [ ] Advanced power management modes
- [ ] Over-the-air update system

---

**Made with ❤️ for the ESP32 community**

*Last updated: October 2, 2024*