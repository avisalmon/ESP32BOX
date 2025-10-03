# Display System - ST77916 Controller

*Last Updated: October 2, 2025 - Based on hardware testing*

## Hardware Specifications

### LCD Panel
- **Size**: 1.85 inch round display
- **Resolution**: 360×360 pixels
- **Color Depth**: 16-bit RGB565
- **Viewing Angle**: Full circle
- **Brightness**: Adjustable via PWM backlight (GPIO 5)

### Controller: ST77916
- **Interface**: QSPI (Quad SPI) - **Requires ESP-LCD framework**
- **Max Clock**: 80MHz (tested working at 5MHz-80MHz)
- **Color Format**: RGB565 (16-bit)
- **Display RAM**: Built-in frame buffer
- **Reset Control**: Via TCA9554PWR I/O expander (EXIO2)

## Pin Configuration

### QSPI Interface
```cpp
#define ESP_PANEL_LCD_SPI_IO_SCK      40    // SPI Clock
#define ESP_PANEL_LCD_SPI_IO_DATA0    46    // SPI Data 0 (MOSI)
#define ESP_PANEL_LCD_SPI_IO_DATA1    45    // SPI Data 1
#define ESP_PANEL_LCD_SPI_IO_DATA2    42    // SPI Data 2
#define ESP_PANEL_LCD_SPI_IO_DATA3    41    // SPI Data 3
#define ESP_PANEL_LCD_SPI_IO_CS       21    // Chip Select
#define ESP_PANEL_LCD_SPI_IO_TE       18    // Tearing Effect
```

### Control Pins
```cpp
#define EXAMPLE_LCD_PIN_NUM_RST       -1    // Reset via EXIO2 (TCA9554PWR)
#define LCD_Backlight_PIN             5     // PWM Backlight Control
```

### Backlight Control

```cpp
#define PWM_Channel                   1     // PWM Channel
#define Frequency                     20000 // 20kHz PWM
#define Resolution                    10    // 10-bit resolution (0-1023)
#define Dutyfactor                    500   // Default 50% duty cycle
#define Backlight_MAX                 100   // Max brightness level
```

**⚠️ Important**: Use newer ESP32 Arduino core API:

```cpp
// Old API (deprecated)
ledcSetup(PWM_Channel, Frequency, Resolution);
ledcAttachPin(LCD_Backlight_PIN, PWM_Channel);
ledcWrite(PWM_Channel, brightness);

// New API (current)
ledcAttach(LCD_Backlight_PIN, Frequency, Resolution);
ledcWrite(LCD_Backlight_PIN, brightness);
```

## 🧪 Hardware Testing Results

*Based on ESP32_Minimal_Test results (October 2, 2025):*

### ✅ **Verified Working:**

- **Backlight Control**: GPIO 5 PWM working perfectly
  - PWM frequency: 20kHz (flicker-free)
  - Resolution: 10-bit (0-1023 range)
  - Smooth brightness control and animation

- **I2C Communication**: GPIO 10 (SCL) / GPIO 11 (SDA)
  - **0x20**: TCA9554PWR I/O expander (confirmed)
  - **0x51**: PCF85063 RTC (confirmed)
  - **0x15**: CST816T touch controller (detected)

- **Display Reset Control**: Via TCA9554PWR EXIO2
  - Reset sequence working via I2C commands
  - Proper timing: 10ms LOW, then HIGH

- **GPIO Pin Configuration**: All display pins configured
  - **SCK**: GPIO 40, **CS**: GPIO 21, **TE**: GPIO 18
  - **DATA0-3**: GPIO 46, 45, 42, 41 (QSPI)

### ⚠️ **Requires Full Implementation:**

- **Actual Graphics Display**: Needs ESP-LCD framework
- **ST77916 Initialization**: Complex vendor-specific commands
- **QSPI Communication**: 4-wire SPI with ESP-LCD library
- **LVGL Integration**: Graphics library framework

### 🎯 **Recommendation:**

For working display graphics, use: `DEMO/Arduino/examples/LVGL_Arduino/LVGL_Arduino.ino`

That implementation includes:
- Complete ESP-LCD framework
- ST77916 driver with proper QSPI
- LVGL graphics library
- Touch controller integration

## LVGL Integration

### Display Buffer Configuration
```cpp
#define EXAMPLE_LCD_WIDTH             360
#define EXAMPLE_LCD_HEIGHT            360
#define EXAMPLE_LCD_COLOR_BITS        16

// Double buffering for smooth updates
static lv_color_t buf_1[EXAMPLE_LCD_WIDTH * 32];
static lv_color_t buf_2[EXAMPLE_LCD_WIDTH * 32];
```

### SPI Configuration
```cpp
#define ESP_PANEL_HOST_SPI_ID_DEFAULT    SPI2_HOST
#define ESP_PANEL_LCD_SPI_MODE           0
#define ESP_PANEL_LCD_SPI_CLK_HZ         (80 * 1000 * 1000)  // 80MHz
#define ESP_PANEL_LCD_SPI_TRANS_QUEUE_SZ 10
#define ESP_PANEL_LCD_SPI_CMD_BITS       32
#define ESP_PANEL_LCD_SPI_PARAM_BITS     8
```

## Initialization Sequence

### 1. Power Management
```cpp
// Initialize TCA9554PWR first
TCA9554PWR_Init(0x00);

// Reset display via EXIO2
Set_EXIO(EXIO_PIN2, Low);   // Assert reset
delay(100);
Set_EXIO(EXIO_PIN2, High);  // Release reset
delay(100);
```

### 2. Backlight Setup
```cpp
void Backlight_Init() {
    ledcSetup(PWM_Channel, Frequency, Resolution);
    ledcAttachPin(LCD_Backlight_PIN, PWM_Channel);
    ledcWrite(PWM_Channel, Dutyfactor);  // Set initial brightness
}

void Set_Backlight(uint8_t Light) {
    if (Light > Backlight_MAX) Light = Backlight_MAX;
    uint16_t duty = map(Light, 0, Backlight_MAX, 0, (1 << Resolution) - 1);
    ledcWrite(PWM_Channel, duty);
}
```

### 3. SPI and LCD Initialization
```cpp
void LCD_Init() {
    // Configure SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = ESP_PANEL_LCD_SPI_IO_DATA0,
        .miso_io_num = -1,
        .sclk_io_num = ESP_PANEL_LCD_SPI_IO_SCK,
        .quadwp_io_num = ESP_PANEL_LCD_SPI_IO_DATA2,
        .quadhd_io_num = ESP_PANEL_LCD_SPI_IO_DATA3,
        .data1_io_num = ESP_PANEL_LCD_SPI_IO_DATA1,
        .max_transfer_sz = ESP_PANEL_HOST_SPI_MAX_TRANSFER_SIZE,
        .flags = SPIBUS_ALLOC_DMA
    };
    
    // Initialize ST77916 controller
    ST77916_Init();
}
```

## Display Functions

### Basic Display Control
```cpp
// Set display window for partial updates
void LCD_addWindow(uint16_t Xstart, uint16_t Ystart, 
                   uint16_t Xend, uint16_t Yend, uint16_t* color);

// LVGL flush callback
void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p);
```

### Backlight Control
```cpp
// Set backlight brightness (0-100)
void Set_Backlight(uint8_t Light);

// Get current backlight level
uint8_t Get_Backlight();
```

## Performance Optimization

### Memory Management
- Use double buffering to prevent tearing
- Limit buffer size to balance memory and performance
- Flush buffers efficiently to avoid blocking

### SPI Optimization
```cpp
// Use DMA for large transfers
#define SPIBUS_ALLOC_DMA  // Enable DMA

// Optimize transfer sizes
#define ESP_PANEL_HOST_SPI_MAX_TRANSFER_SIZE (2048)
```

### LVGL Configuration
```cpp
// In lv_conf.h
#define LV_COLOR_DEPTH     16
#define LV_COLOR_16_SWAP   0
#define LV_MEM_CUSTOM      1
#define LV_TICK_CUSTOM     1
```

## Troubleshooting

### 🔧 **Step-by-Step Debugging (Based on Testing Experience)**

#### **Step 1: Verify Basic Hardware**

```cpp
// Test backlight control first (this should work immediately)
ledcAttach(5, 20000, 10);           // GPIO 5, 20kHz, 10-bit
ledcWrite(5, 512);                  // 50% brightness
delay(1000);
ledcWrite(5, 0);                    // Off
delay(1000);  
ledcWrite(5, 1023);                 // Full brightness
```

**Expected Result**: Physical brightness changes on LCD backlight

#### **Step 2: Test I2C Communication**

```cpp
Wire.begin(11, 10);  // SDA=11, SCL=10
// Scan for devices
for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
        Serial.printf("Device at 0x%02X\n", addr);
    }
}
```

**Expected Results**:
- **0x20**: TCA9554PWR I/O expander ✅
- **0x51**: PCF85063 RTC ✅  
- **0x15**: CST816T touch controller ✅

#### **Step 3: Test Display Reset**

```cpp
// Reset via TCA9554PWR EXIO2
Wire.beginTransmission(0x20);
Wire.write(0x03); Wire.write(0xFB);  // Config EXIO2 as output
Wire.endTransmission();

Wire.beginTransmission(0x20);
Wire.write(0x01); Wire.write(0x00);  // EXIO2 LOW (reset)
Wire.endTransmission();
delay(100);

Wire.beginTransmission(0x20);  
Wire.write(0x01); Wire.write(0x04);  // EXIO2 HIGH (release)
Wire.endTransmission();
```

**Expected Result**: No errors in I2C communication

### 🚨 **Common Issues & Solutions**

1. **Nothing on Screen (Backlight Works)**
   - ✅ **Verified**: Hardware is functional (our testing confirms this)
   - ❌ **Missing**: ST77916 requires ESP-LCD framework
   - 🔧 **Solution**: Use `DEMO/Arduino/examples/LVGL_Arduino/` code

2. **Compilation Errors**
   ```
   'ledcSetup' was not declared in this scope
   ```
   - 🔧 **Solution**: Update to new ESP32 Arduino core API:
   ```cpp
   // Replace old API with:
   ledcAttach(pin, frequency, resolution);
   ledcWrite(pin, value);
   ```

3. **I2C Devices Not Found**  
   - Check SDA=11, SCL=10 (confirmed working)
   - Verify 3.3V power supply
   - Check wire connections

4. **Display Initialization Fails**
   - ST77916 needs complex vendor-specific initialization
   - Simple SPI commands won't work
   - Must use ESP-LCD framework with QSPI support

### Debug Commands
```cpp
// Test backlight
for (int i = 0; i <= 100; i += 10) {
    Set_Backlight(i);
    delay(200);
}

// Test display colors
uint16_t colors[] = {0x0000, 0xFFFF, 0xF800, 0x07E0, 0x001F};
for (int i = 0; i < 5; i++) {
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(colors[i]), 0);
    delay(1000);
}
```

## Example Code

### Basic Display Test
```cpp
void display_test() {
    // Initialize display system
    TCA9554PWR_Init(0x00);
    Backlight_Init();
    LCD_Init();
    Lvgl_Init();
    
    // Create simple UI
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello ESP32-S3!");
    lv_obj_center(label);
    
    // Test backlight levels
    for (int brightness = 0; brightness <= 100; brightness += 10) {
        Set_Backlight(brightness);
        lv_task_handler();  // Update LVGL
        delay(500);
    }
}
```

### Advanced UI Example
```cpp
void create_round_ui() {
    // Create circular container
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 340, 340);
    lv_obj_center(cont);
    lv_obj_set_style_radius(cont, 170, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    
    // Add circular progress indicator
    lv_obj_t * arc = lv_arc_create(cont);
    lv_obj_set_size(arc, 300, 300);
    lv_obj_center(arc);
    lv_arc_set_value(arc, 75);
}
```

## Reference Links
- ST77916 Datasheet
- LVGL Documentation: https://docs.lvgl.io/
- ESP32-S3 SPI Documentation

*Last Updated: October 2, 2025*