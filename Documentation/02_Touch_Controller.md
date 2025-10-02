# Touch Controller - CST816T

## Hardware Specifications

### Touch Controller: CST816T

- **Interface**: I2C
- **I2C Address**: 0x15 (7-bit)
- **Touch Points**: Single touch
- **Gestures**: Multiple gesture support
- **Power**: Low power consumption with auto-sleep

### Supported Gestures

```cpp
enum GESTURE {
    NONE = 0x00,
    SWIPE_UP = 0x01,
    SWIPE_DOWN = 0x02,
    SWIPE_LEFT = 0x03,
    SWIPE_RIGHT = 0x04,
    SINGLE_CLICK = 0x05,
    DOUBLE_CLICK = 0x0B,
    LONG_PRESS = 0x0C
};
```

## Pin Configuration

### Touch Pins

```cpp
#define CST816_ADDR           0x15      // I2C Address
#define CST816_INT_PIN        4         // Interrupt pin (GPIO 4)
#define CST816_RST_PIN        -1        // Reset via EXIO1 (TCA9554PWR)
#define I2C_MASTER_FREQ_HZ    400000    // I2C Clock: 400kHz
```

### I2C Bus Connection

**Note**: Touch uses the MAIN I2C bus shared with other devices:

```cpp
#define I2C_SCL_PIN           10        // Main I2C Clock
#define I2C_SDA_PIN           11        // Main I2C Data
```

### Control via TCA9554PWR

```cpp
// Touch reset is controlled via I/O expander
#define TCA9554_ADDRESS       0x20      // I/O expander address
#define TCA9554_EXIO1         0x01      // EXIO1 = Touch Reset
```

## Register Map

### Key Registers

```cpp
#define CST816_REG_GestureID      0x01  // Gesture ID register
#define CST816_REG_Version        0x15  // Firmware version
#define CST816_REG_ChipID         0xA7  // Chip ID
#define CST816_REG_ProjID         0xA8  // Project ID
#define CST816_REG_FwVersion      0xA9  // Firmware version
#define CST816_REG_AutoSleepTime  0xF9  // Auto sleep time
#define CST816_REG_DisAutoSleep   0xFE  // Disable auto sleep
```

### Touch Data Structure

```cpp
struct CST816_Touch {
    uint8_t points;         // Number of touch points (0 or 1)
    GESTURE gesture;        // Detected gesture
    uint16_t x;            // X coordinate (0-359)
    uint16_t y;            // Y coordinate (0-359)
};
```

## Initialization Sequence

### 1. I2C Bus Setup

```cpp
void I2C_Init(void) {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_MASTER_FREQ_HZ);
    Wire.setTimeout(1000);  // 1 second timeout
}
```

### 2. TCA9554PWR Initialization

```cpp
void TCA9554PWR_Init(uint8_t PinState) {
    // Initialize I/O expander first
    // EXIO1 will control touch reset
    Mode_EXIO(EXIO_PIN1, 0);  // Set as output
    Set_EXIO(EXIO_PIN1, High); // Release reset
}
```

### 3. Touch Controller Reset

```cpp
uint8_t CST816_Touch_Reset(void) {
    // Reset via TCA9554PWR EXIO1
    Set_EXIO(EXIO_PIN1, Low);   // Assert reset
    delay(100);
    Set_EXIO(EXIO_PIN1, High);  // Release reset
    delay(200);
    
    // Configure interrupt pin
    pinMode(CST816_INT_PIN, INPUT_PULLUP);
    
    return 1;
}
```

### 4. Touch Initialization

```cpp
uint8_t Touch_Init() {
    // Reset touch controller
    CST816_Touch_Reset();
    
    // Test I2C communication
    Wire.beginTransmission(CST816_ADDR);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
        Serial.println("✓ Touch controller detected");
        
        // Configure auto-sleep (optional)
        CST816_AutoSleep(false);  // Disable auto-sleep
        
        return 1;
    } else {
        Serial.printf("❌ Touch controller not found (error: %d)\n", error);
        return 0;
    }
}
```

## Touch Reading Functions

### Main Touch Loop

```cpp
void Touch_Loop(void) {
    // Check if interrupt pin is low (touch event)
    if (digitalRead(CST816_INT_PIN) == LOW) {
        
        // Read touch data from registers
        uint8_t data[6];
        if (I2C_Read(CST816_ADDR, 0x00, data, 6)) {
            
            // Parse touch data
            touch_data.points = data[0] & 0x0F;
            touch_data.gesture = (GESTURE)data[1];
            
            if (touch_data.points > 0) {
                touch_data.x = ((data[2] & 0x0F) << 8) | data[3];
                touch_data.y = ((data[4] & 0x0F) << 8) | data[5];
                
                // Coordinate bounds checking
                if (touch_data.x >= EXAMPLE_LCD_WIDTH) 
                    touch_data.x = EXAMPLE_LCD_WIDTH - 1;
                if (touch_data.y >= EXAMPLE_LCD_HEIGHT) 
                    touch_data.y = EXAMPLE_LCD_HEIGHT - 1;
            }
        }
    }
}
```

### Gesture Recognition

```cpp
String Touch_GestureName(void) {
    switch (touch_data.gesture) {
        case SWIPE_UP:     return "Swipe Up";
        case SWIPE_DOWN:   return "Swipe Down";
        case SWIPE_LEFT:   return "Swipe Left";
        case SWIPE_RIGHT:  return "Swipe Right";
        case SINGLE_CLICK: return "Single Click";
        case DOUBLE_CLICK: return "Double Click";
        case LONG_PRESS:   return "Long Press";
        default:           return "None";
    }
}
```

## LVGL Integration

### Touch Input Device Setup

```cpp
void lvgl_touch_init() {
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
}
```

### LVGL Touch Callback

```cpp
void my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data) {
    // Read current touch state
    Touch_Loop();
    
    if (touch_data.points > 0) {
        data->state = LV_INDEV_STATE_PR;  // Pressed
        data->point.x = touch_data.x;
        data->point.y = touch_data.y;
    } else {
        data->state = LV_INDEV_STATE_REL; // Released
    }
}
```

## Power Management

### Auto-Sleep Configuration

```cpp
void CST816_AutoSleep(bool Sleep_State) {
    uint8_t data = Sleep_State ? 0x03 : 0x00;
    I2C_Write(CST816_ADDR, CST816_REG_DisAutoSleep, &data, 1);
    
    if (!Sleep_State) {
        // Set auto-sleep time to maximum (optional)
        data = 0xFF;
        I2C_Write(CST816_ADDR, CST816_REG_AutoSleepTime, &data, 1);
    }
}
```

### Wake-Up from Sleep

```cpp
void touch_wake_up() {
    // Touch controller wakes up automatically on touch
    // No special wake-up sequence needed
    
    // Re-initialize if needed
    if (digitalRead(CST816_INT_PIN) == LOW) {
        Touch_Loop();  // Read and clear interrupt
    }
}
```

## Troubleshooting

### Common Issues

1. **Touch Not Detected**
   - Check I2C address (0x15)
   - Verify TCA9554PWR initialization
   - Ensure proper reset sequence
   - Check I2C bus sharing with other devices

2. **Inaccurate Touch Coordinates**
   - Verify display orientation
   - Check coordinate bounds (0-359)
   - Calibrate if necessary

3. **Missing Gestures**
   - Ensure gesture recognition is enabled
   - Check gesture sensitivity settings
   - Verify interrupt pin connection

4. **Intermittent Touch Response**
   - Check power supply stability
   - Verify I2C timing and pull-up resistors
   - Monitor for I2C bus conflicts

### Debug Functions

```cpp
void touch_debug_info() {
    uint8_t chipID, version, projID;
    
    // Read chip information
    I2C_Read(CST816_ADDR, CST816_REG_ChipID, &chipID, 1);
    I2C_Read(CST816_ADDR, CST816_REG_Version, &version, 1);
    I2C_Read(CST816_ADDR, CST816_REG_ProjID, &projID, 1);
    
    Serial.printf("Touch Controller Info:\n");
    Serial.printf("  Chip ID: 0x%02X\n", chipID);
    Serial.printf("  Version: 0x%02X\n", version);
    Serial.printf("  Project ID: 0x%02X\n", projID);
    Serial.printf("  Interrupt Pin: %s\n", 
                  digitalRead(CST816_INT_PIN) ? "HIGH" : "LOW");
}

void touch_test_loop() {
    static unsigned long lastPrint = 0;
    
    Touch_Loop();
    
    if (millis() - lastPrint > 100) {  // Print every 100ms
        lastPrint = millis();
        
        if (touch_data.points > 0) {
            Serial.printf("Touch: X=%d, Y=%d, Gesture=%s\n",
                         touch_data.x, touch_data.y, 
                         Touch_GestureName().c_str());
        }
    }
}
```

## Example Code

### Basic Touch Test

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize I2C and touch
    I2C_Init();
    TCA9554PWR_Init(0x00);
    
    if (Touch_Init()) {
        Serial.println("Touch controller ready!");
    } else {
        Serial.println("Touch controller failed!");
    }
}

void loop() {
    touch_test_loop();
    delay(10);
}
```

### LVGL Touch Integration

```cpp
void setup() {
    // Initialize display and LVGL first
    LCD_Init();
    Lvgl_Init();
    
    // Initialize touch
    I2C_Init();
    TCA9554PWR_Init(0x00);
    Touch_Init();
    
    // Register touch input device
    lvgl_touch_init();
    
    // Create UI elements
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Touch Me!");
    lv_obj_center(btn);
}

void loop() {
    lv_task_handler();  // LVGL handles touch automatically
    delay(5);
}
```

*Last Updated: October 2, 2025*