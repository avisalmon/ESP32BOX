# Display System - ST77916 Controller

## Hardware Specifications

### LCD Panel
- **Size**: 1.85 inch round display
- **Resolution**: 360×360 pixels
- **Color Depth**: 16-bit RGB565
- **Viewing Angle**: Full circle
- **Brightness**: Adjustable via PWM backlight

### Controller: ST77916
- **Interface**: QSPI (Quad SPI)
- **Max Clock**: 80MHz
- **Color Format**: RGB565 (16-bit)
- **Display RAM**: Built-in frame buffer

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

### Common Issues
1. **White/Black Screen**
   - Check power sequence (TCA9554PWR → Reset → Backlight)
   - Verify QSPI pin connections
   - Ensure proper voltage levels

2. **Flickering Display**
   - Increase SPI clock speed
   - Use double buffering
   - Check ground connections

3. **Color Issues**
   - Verify RGB565 format
   - Check color depth settings
   - Ensure proper byte order

4. **Slow Updates**
   - Optimize LVGL buffer size
   - Use partial updates
   - Enable DMA transfers

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