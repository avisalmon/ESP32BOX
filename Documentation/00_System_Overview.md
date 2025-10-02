# ESP32-S3-Touch-LCD-1.85C System Overview

## Hardware Specifications

### Main Controller
- **MCU**: ESP32-S3 dual-core 240MHz
- **Flash**: 16MB
- **PSRAM**: 8MB
- **WiFi/Bluetooth**: Built-in

### Display
- **LCD**: 1.85" Round Display
- **Resolution**: 360×360 pixels
- **Controller**: ST77916
- **Interface**: QSPI (4-line SPI)
- **Color Depth**: 16-bit (RGB565)

### Touch Controller
- **IC**: CST816T
- **Interface**: I2C
- **Features**: Single touch, gestures
- **Gestures**: Swipe (Up/Down/Left/Right), Single Click, Double Click, Long Press

### Audio System
- **DAC**: PCM5101 (I2S)
- **Microphone**: MSM261S4030H0 (I2S)
- **Features**: Audio playback, voice recording

### Storage
- **SD Card**: Micro SD slot (up to 32GB)
- **Interface**: SDIO (1-bit mode)

### Power Management
- **Battery**: Li-Po connector
- **Charging**: Built-in charging circuit
- **Power Control**: TCA9554PWR I/O expander

### Sensors
- **RTC**: PCF85063 (I2C)
- **IMU**: QMI8658C (6-axis, I2C) - *Not used in demo*

## Pin Configuration Summary

### Core I2C Bus (Main)
- **SDA**: GPIO 11
- **SCL**: GPIO 10
- **Devices**: TCA9554PWR (0x20), RTC PCF85063

### Touch I2C (Separate)
- **Address**: 0x15 (CST816T)
- **Interrupt**: GPIO 4
- **Reset**: EXIO1 (via TCA9554PWR)

### Display QSPI
- **SCK**: GPIO 40
- **DATA0**: GPIO 46
- **DATA1**: GPIO 45  
- **DATA2**: GPIO 42
- **DATA3**: GPIO 41
- **CS**: GPIO 21
- **TE**: GPIO 18
- **Reset**: EXIO2 (via TCA9554PWR)
- **Backlight**: GPIO 5 (PWM)

### Audio I2S (Playback)
- **BCLK**: GPIO 48
- **LRCK**: GPIO 38
- **DOUT**: GPIO 47

### Microphone I2S (Recording)
- **BCLK**: GPIO 15
- **LRCK**: GPIO 2
- **DIN**: GPIO 39

### SD Card SDIO
- **CLK**: GPIO 14
- **CMD**: GPIO 17
- **DATA0**: GPIO 16

### Battery Monitoring
- **ADC**: GPIO 8
- **Voltage Divider**: 2:1 ratio

## System Architecture

### Task Structure
1. **Main Loop**: LVGL processing (5ms cycle)
2. **Driver Task**: RTC updates, battery monitoring (100ms cycle)
3. **Audio Task**: Background audio processing
4. **Touch Task**: Touch event handling

### Memory Management
- **LVGL Buffer**: 2 × 32KB (double buffering)
- **Audio Buffer**: Configurable size
- **File System**: SPIFFS + SD Card

### Power Management
- **Deep Sleep**: Supported
- **Auto Sleep**: Touch controller configurable
- **Battery Monitoring**: Continuous via ADC

## Development Environment

### Arduino IDE Setup

#### Board Configuration
- **Board**: ESP32S3 Dev Module
- **Upload Speed**: 921600
- **USB Mode**: Hardware CDC and JTAG
- **USB CDC On Boot**: Enabled
- **USB Firmware MSC On Boot**: Disabled
- **USB DFU On Boot**: Disabled
- **Upload Mode**: UART0 / Hardware CDC
- **CPU Frequency**: 240MHz (WiFi/BT)
- **Flash Mode**: QIO
- **Flash Frequency**: 80MHz
- **Flash Size**: 16MB (128Mb)
- **Partition Scheme**: 16M Flash (3MB APP/9.9MB FATFS)
- **Core Debug Level**: None
- **PSRAM**: Enabled
- **Arduino Runs On**: Core 1
- **Events Run On**: Core 1

#### Required Libraries
```
lvgl==8.3.10
TFT_eSPI
Wire
SPI
SD
WiFi
WebServer
ArduinoJson
ESP32Time
ESP32Audio
```

#### Board Manager URLs
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

#### Programmer Configuration
- **Programmer**: Esptool
- **Port**: COM4 (or your specific COM port)
- **Before Upload**: Default Reset
- **After Upload**: Default Reset

#### Memory Layout
- **Code Partition**: 3MB (Application)
- **SPIFFS/FATFS**: 9.9MB (File System)
- **OTA**: ~3MB (Over-the-Air Updates)
- **NVS**: 20KB (Non-Volatile Storage)
- **Bootloader**: ~48KB
- **Partition Table**: 4KB

### Required Libraries
- **LVGL**: v8.3.x (included in demo)
- **ESP32-audioI2S**: For audio functionality
- **ESP_SR**: For speech recognition (optional)

### Programming Notes
- **Watchdog Sensitivity**: Very sensitive to blocking operations
- **I2C Timing**: Use proper delays between operations
- **Memory**: Monitor heap usage with LVGL
- **SD Card**: Format as FAT32 for compatibility

## Quick Start
1. Install Arduino IDE with ESP32 package
2. Copy libraries from DEMO/Arduino/libraries
3. Open DEMO/Arduino/examples/LVGL_Arduino/LVGL_Arduino.ino
4. Select correct board settings
5. Compile and upload

## Troubleshooting
- **Watchdog Resets**: Reduce blocking operations, add yield() calls
- **Touch Issues**: Check I2C wiring and TCA9554PWR initialization
- **Display Issues**: Verify QSPI connections and power sequence
- **Audio Issues**: Check I2S pin assignments and sample rates
- **SD Card Issues**: Format as FAT32, check SDIO connections

*Last Updated: October 2, 2025*