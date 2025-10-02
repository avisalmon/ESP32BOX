# Power Management System - TCA9554PWR

## Hardware Specifications

### Power Management IC: TCA9554PWR

- **Type**: 8-bit I2C I/O Expander
- **Interface**: I2C
- **I2C Address**: 0x20 (7-bit)
- **Supply Voltage**: 1.65V to 5.5V
- **I/O Voltage**: Same as VCC
- **Current**: Each I/O can sink 25mA
- **Features**: Interrupt output, reset input

### Power Distribution

- **EXIO1**: Touch controller reset (CST816T)
- **EXIO2**: Display reset (ST77916)
- **EXIO3**: General purpose I/O
- **EXIO4**: General purpose I/O
- **EXIO5**: General purpose I/O
- **EXIO6**: General purpose I/O
- **EXIO7**: General purpose I/O
- **PWR_EN**: Main power enable (GPIO 46)

## Pin Configuration

### I2C Connection

```cpp
#define I2C_SCL_PIN           10        // Shared I2C clock
#define I2C_SDA_PIN           11        // Shared I2C data
#define TCA9554_ADDRESS       0x20      // I2C address
```

### Power Control Pin

```cpp
#define PWR_EN                46        // Main power enable pin
```

### EXIO Pin Definitions

```cpp
#define TCA9554_EXIO1         0x01      // Touch Reset
#define TCA9554_EXIO2         0x02      // Display Reset  
#define TCA9554_EXIO3         0x03      // General Purpose
#define TCA9554_EXIO4         0x04      // General Purpose
#define TCA9554_EXIO5         0x05      // General Purpose
#define TCA9554_EXIO6         0x06      // General Purpose
#define TCA9554_EXIO7         0x07      // General Purpose

#define EXIO_PIN1             1         // Touch Reset
#define EXIO_PIN2             2         // Display Reset
#define EXIO_PIN3             3         // GPIO
#define EXIO_PIN4             4         // GPIO
#define EXIO_PIN5             5         // GPIO
#define EXIO_PIN6             6         // GPIO
#define EXIO_PIN7             7         // GPIO
#define EXIO_PIN8             8         // GPIO (not used)
```

## Register Map

### TCA9554PWR Registers

```cpp
#define TCA9554_INPUT_REG     0x00      // Input port register
#define TCA9554_OUTPUT_REG    0x01      // Output port register
#define TCA9554_Polarity_REG  0x02      // Polarity inversion register
#define TCA9554_CONFIG_REG    0x03      // Configuration register
```

### Pin State Definitions

```cpp
#define Low                   0         // Logic low
#define High                  1         // Logic high
```

## Basic I/O Functions

### Register Access

```cpp
uint8_t Read_REG(uint8_t REG) {
    Wire.beginTransmission(TCA9554_ADDRESS);
    Wire.write(REG);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        Serial.printf("TCA9554 read error: %d\n", error);
        return 0;
    }
    
    Wire.requestFrom(TCA9554_ADDRESS, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

uint8_t Write_REG(uint8_t REG, uint8_t Data) {
    Wire.beginTransmission(TCA9554_ADDRESS);
    Wire.write(REG);
    Wire.write(Data);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        Serial.printf("TCA9554 write error: %d\n", error);
    }
    
    return error;
}
```

### Pin Configuration

```cpp
void Mode_EXIO(uint8_t Pin, uint8_t State) {
    // Set pin mode: 0 = Output, 1 = Input
    uint8_t config = Read_REG(TCA9554_CONFIG_REG);
    
    if (State == 0) {  // Output mode
        config &= ~(1 << (Pin - 1));
    } else {  // Input mode
        config |= (1 << (Pin - 1));
    }
    
    Write_REG(TCA9554_CONFIG_REG, config);
}

void Mode_EXIOS(uint8_t PinState) {
    // Set all pins at once (bit pattern)
    Write_REG(TCA9554_CONFIG_REG, PinState);
}
```

### Pin Control

```cpp
void Set_EXIO(uint8_t Pin, uint8_t State) {
    uint8_t output = Read_REG(TCA9554_OUTPUT_REG);
    
    if (State == High) {
        output |= (1 << (Pin - 1));
    } else {
        output &= ~(1 << (Pin - 1));
    }
    
    Write_REG(TCA9554_OUTPUT_REG, output);
}

void Set_EXIOS(uint8_t PinState) {
    // Set all output pins at once
    Write_REG(TCA9554_OUTPUT_REG, PinState);
}

void Set_Toggle(uint8_t Pin) {
    uint8_t output = Read_REG(TCA9554_OUTPUT_REG);
    output ^= (1 << (Pin - 1));  // Toggle bit
    Write_REG(TCA9554_OUTPUT_REG, output);
}
```

### Pin Reading

```cpp
uint8_t Read_EXIO(uint8_t Pin) {
    uint8_t input = Read_REG(TCA9554_INPUT_REG);
    return (input >> (Pin - 1)) & 0x01;
}

uint8_t Read_EXIOS(uint8_t REG) {
    // Read all pins from specified register
    return Read_REG(REG);
}
```

## Initialization

### Power System Setup

```cpp
void TCA9554PWR_Init(uint8_t PinState = 0x00) {
    Serial.println("Initializing TCA9554PWR I/O expander...");
    
    // Enable main power first
    pinMode(PWR_EN, OUTPUT);
    digitalWrite(PWR_EN, HIGH);
    delay(100);  // Wait for power stabilization
    
    // Test I2C communication
    Wire.beginTransmission(TCA9554_ADDRESS);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        Serial.printf("❌ TCA9554PWR not found at 0x%02X (error: %d)\n", 
                     TCA9554_ADDRESS, error);
        return;
    }
    
    Serial.printf("✓ TCA9554PWR found at address 0x%02X\n", TCA9554_ADDRESS);
    
    // Configure pin modes (0 = Output, 1 = Input)
    Mode_EXIOS(PinState);
    
    // Set default output states (all high for reset pins)
    Set_EXIOS(0xFF);
    
    // Read back configuration
    uint8_t config = Read_REG(TCA9554_CONFIG_REG);
    uint8_t output = Read_REG(TCA9554_OUTPUT_REG);
    uint8_t input = Read_REG(TCA9554_INPUT_REG);
    
    Serial.printf("Configuration: 0x%02X, Output: 0x%02X, Input: 0x%02X\n", 
                 config, output, input);
    
    Serial.println("✓ TCA9554PWR initialized successfully");
}
```

## Device Reset Functions

### Touch Controller Reset

```cpp
void reset_touch_controller() {
    Serial.println("Resetting touch controller...");
    
    // Configure EXIO1 as output
    Mode_EXIO(EXIO_PIN1, 0);
    
    // Assert reset (low)
    Set_EXIO(EXIO_PIN1, Low);
    delay(100);
    
    // Release reset (high)
    Set_EXIO(EXIO_PIN1, High);
    delay(200);
    
    Serial.println("✓ Touch controller reset complete");
}
```

### Display Reset

```cpp
void reset_display_controller() {
    Serial.println("Resetting display controller...");
    
    // Configure EXIO2 as output
    Mode_EXIO(EXIO_PIN2, 0);
    
    // Assert reset (low)
    Set_EXIO(EXIO_PIN2, Low);
    delay(100);
    
    // Release reset (high)
    Set_EXIO(EXIO_PIN2, High);
    delay(100);
    
    Serial.println("✓ Display controller reset complete");
}
```

### System Reset Sequence

```cpp
void system_reset_sequence() {
    Serial.println("Performing system reset sequence...");
    
    // Configure all control pins as outputs
    Mode_EXIO(EXIO_PIN1, 0);  // Touch reset
    Mode_EXIO(EXIO_PIN2, 0);  // Display reset
    
    // Assert all resets
    Set_EXIO(EXIO_PIN1, Low);
    Set_EXIO(EXIO_PIN2, Low);
    delay(200);
    
    // Release resets in sequence
    Set_EXIO(EXIO_PIN2, High);  // Display first
    delay(100);
    Set_EXIO(EXIO_PIN1, High);  // Touch second
    delay(200);
    
    Serial.println("✓ System reset sequence complete");
}
```

## Power Monitoring

### Power State Management

```cpp
struct PowerState {
    bool mainPowerEnabled;
    uint8_t exioConfig;
    uint8_t exioOutput;
    uint8_t exioInput;
    unsigned long lastUpdate;
};

PowerState powerState = {false, 0, 0, 0, 0};

void update_power_state() {
    powerState.mainPowerEnabled = digitalRead(PWR_EN);
    powerState.exioConfig = Read_REG(TCA9554_CONFIG_REG);
    powerState.exioOutput = Read_REG(TCA9554_OUTPUT_REG);
    powerState.exioInput = Read_REG(TCA9554_INPUT_REG);
    powerState.lastUpdate = millis();
}

void print_power_state() {
    update_power_state();
    
    Serial.println("=== Power State ===");
    Serial.printf("Main Power (PWR_EN): %s\n", 
                 powerState.mainPowerEnabled ? "ON" : "OFF");
    Serial.printf("EXIO Config: 0x%02X\n", powerState.exioConfig);
    Serial.printf("EXIO Output: 0x%02X\n", powerState.exioOutput);
    Serial.printf("EXIO Input: 0x%02X\n", powerState.exioInput);
    
    // Decode individual pins
    for (int pin = 1; pin <= 7; pin++) {
        bool isOutput = !(powerState.exioConfig & (1 << (pin - 1)));
        bool pinState;
        
        if (isOutput) {
            pinState = powerState.exioOutput & (1 << (pin - 1));
            Serial.printf("  EXIO%d: OUTPUT - %s\n", pin, pinState ? "HIGH" : "LOW");
        } else {
            pinState = powerState.exioInput & (1 << (pin - 1));
            Serial.printf("  EXIO%d: INPUT - %s\n", pin, pinState ? "HIGH" : "LOW");
        }
    }
}
```

## Advanced Power Features

### Power Sequencing

```cpp
void power_on_sequence() {
    Serial.println("Starting power-on sequence...");
    
    // Step 1: Enable main power
    pinMode(PWR_EN, OUTPUT);
    digitalWrite(PWR_EN, HIGH);
    delay(100);
    Serial.println("  ✓ Main power enabled");
    
    // Step 2: Initialize I/O expander
    TCA9554PWR_Init(0x00);  // All pins as outputs initially
    delay(50);
    Serial.println("  ✓ I/O expander initialized");
    
    // Step 3: Release display reset
    reset_display_controller();
    delay(100);
    Serial.println("  ✓ Display controller ready");
    
    // Step 4: Release touch reset
    reset_touch_controller();
    delay(100);
    Serial.println("  ✓ Touch controller ready");
    
    Serial.println("✓ Power-on sequence complete");
}

void power_off_sequence() {
    Serial.println("Starting power-off sequence...");
    
    // Step 1: Assert all resets
    Set_EXIO(EXIO_PIN1, Low);  // Touch reset
    Set_EXIO(EXIO_PIN2, Low);  // Display reset
    delay(100);
    Serial.println("  ✓ All peripherals reset");
    
    // Step 2: Configure all pins as inputs (high impedance)
    Mode_EXIOS(0xFF);
    delay(50);
    Serial.println("  ✓ I/O pins set to high impedance");
    
    // Step 3: Disable main power
    digitalWrite(PWR_EN, LOW);
    delay(100);
    Serial.println("  ✓ Main power disabled");
    
    Serial.println("✓ Power-off sequence complete");
}
```

### Sleep Mode Support

```cpp
void enter_sleep_mode() {
    Serial.println("Entering sleep mode...");
    
    // Save current power state
    update_power_state();
    
    // Configure wake-up sources
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);  // Touch interrupt
    esp_sleep_enable_timer_wakeup(30 * 1000000);  // 30 seconds
    
    // Reduce power consumption
    Set_EXIO(EXIO_PIN3, Low);  // Turn off optional peripherals
    Set_EXIO(EXIO_PIN4, Low);
    
    // Enter deep sleep
    Serial.println("Going to sleep...");
    Serial.flush();
    esp_deep_sleep_start();
}

void wake_from_sleep() {
    Serial.println("Waking from sleep...");
    
    // Check wake-up cause
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wake-up from touch interrupt");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wake-up from timer");
            break;
        default:
            Serial.println("Wake-up from reset");
            break;
    }
    
    // Restore power state
    power_on_sequence();
}
```

## GPIO Expansion

### Custom GPIO Functions

```cpp
class ExpandedGPIO {
private:
    uint8_t currentOutput;
    uint8_t currentConfig;
    
public:
    ExpandedGPIO() {
        currentOutput = 0xFF;  // All high initially
        currentConfig = 0x00;  // All outputs initially
    }
    
    void pinMode(uint8_t pin, uint8_t mode) {
        if (pin < 1 || pin > 7) return;
        
        if (mode == INPUT) {
            currentConfig |= (1 << (pin - 1));
        } else {  // OUTPUT
            currentConfig &= ~(1 << (pin - 1));
        }
        
        Write_REG(TCA9554_CONFIG_REG, currentConfig);
    }
    
    void digitalWrite(uint8_t pin, uint8_t value) {
        if (pin < 1 || pin > 7) return;
        
        if (value == HIGH) {
            currentOutput |= (1 << (pin - 1));
        } else {
            currentOutput &= ~(1 << (pin - 1));
        }
        
        Write_REG(TCA9554_OUTPUT_REG, currentOutput);
    }
    
    int digitalRead(uint8_t pin) {
        if (pin < 1 || pin > 7) return LOW;
        
        uint8_t input = Read_REG(TCA9554_INPUT_REG);
        return (input & (1 << (pin - 1))) ? HIGH : LOW;
    }
    
    void update() {
        // Refresh current state from hardware
        currentConfig = Read_REG(TCA9554_CONFIG_REG);
        currentOutput = Read_REG(TCA9554_OUTPUT_REG);
    }
};

ExpandedGPIO exGPIO;
```

## Troubleshooting

### Common Issues

1. **I/O Expander Not Responding**
   - Check I2C connections (pins 10, 11)
   - Verify power supply (PWR_EN high)
   - Test I2C address (0x20)
   - Check for bus conflicts

2. **Peripherals Not Resetting**
   - Verify EXIO pin configuration
   - Check reset timing (minimum pulse width)
   - Test with manual reset sequence
   - Monitor with oscilloscope

3. **Power Sequencing Issues**
   - Follow proper power-on sequence
   - Allow sufficient settling time
   - Check power supply stability
   - Monitor current consumption

4. **Pin State Confusion**
   - Read back register values
   - Verify input/output configuration
   - Check for pin conflicts
   - Use diagnostic functions

### Debug Functions

```cpp
void tca9554_diagnostic() {
    Serial.println("=== TCA9554PWR Diagnostic ===");
    
    // Test I2C communication
    Wire.beginTransmission(TCA9554_ADDRESS);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
        Serial.println("✓ I2C communication OK");
    } else {
        Serial.printf("❌ I2C communication failed (error: %d)\n", error);
        return;
    }
    
    // Read all registers
    uint8_t input = Read_REG(TCA9554_INPUT_REG);
    uint8_t output = Read_REG(TCA9554_OUTPUT_REG);
    uint8_t polarity = Read_REG(TCA9554_Polarity_REG);
    uint8_t config = Read_REG(TCA9554_CONFIG_REG);
    
    Serial.printf("Registers:\n");
    Serial.printf("  Input (0x00): 0x%02X\n", input);
    Serial.printf("  Output (0x01): 0x%02X\n", output);
    Serial.printf("  Polarity (0x02): 0x%02X\n", polarity);
    Serial.printf("  Config (0x03): 0x%02X\n", config);
    
    // Test each pin
    Serial.println("Pin States:");
    for (int pin = 1; pin <= 7; pin++) {
        bool isInput = config & (1 << (pin - 1));
        bool state = (isInput ? input : output) & (1 << (pin - 1));
        
        Serial.printf("  EXIO%d: %s - %s\n", 
                     pin, 
                     isInput ? "INPUT " : "OUTPUT",
                     state ? "HIGH" : "LOW");
    }
    
    // Test pin toggling
    Serial.println("Testing pin control...");
    for (int pin = 1; pin <= 3; pin++) {  // Test first 3 pins
        Serial.printf("Testing EXIO%d...\n", pin);
        
        Mode_EXIO(pin, 0);  // Set as output
        
        Set_EXIO(pin, High);
        delay(100);
        bool high_state = Read_EXIO(pin);
        
        Set_EXIO(pin, Low);
        delay(100);
        bool low_state = Read_EXIO(pin);
        
        Serial.printf("  High: %s, Low: %s\n", 
                     high_state ? "OK" : "FAIL",
                     !low_state ? "OK" : "FAIL");
    }
}

void power_consumption_test() {
    Serial.println("=== Power Consumption Test ===");
    
    // Test different power states
    Serial.println("All pins HIGH (normal operation):");
    Set_EXIOS(0xFF);
    delay(1000);
    
    Serial.println("All pins LOW (reduced power):");
    Set_EXIOS(0x00);
    delay(1000);
    
    Serial.println("All pins as inputs (high impedance):");
    Mode_EXIOS(0xFF);
    delay(1000);
    
    Serial.println("Power test complete - measure current at each step");
    
    // Restore normal operation
    TCA9554PWR_Init(0x00);
}
```

## Example Implementation

### Power Management Controller

```cpp
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Starting power management system...");
    
    // Initialize I2C
    I2C_Init();
    
    // Perform power-on sequence
    power_on_sequence();
    
    // Print initial state
    print_power_state();
    
    Serial.println("Power management ready");
    Serial.println("Commands: 'r' = reset, 's' = status, 'd' = diagnostic");
}

void loop() {
    // Handle serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        
        switch (cmd) {
            case 'r':
                system_reset_sequence();
                break;
            case 's':
                print_power_state();
                break;
            case 'd':
                tca9554_diagnostic();
                break;
            case '1':
                Set_Toggle(EXIO_PIN1);
                Serial.println("Toggled EXIO1");
                break;
            case '2':
                Set_Toggle(EXIO_PIN2);
                Serial.println("Toggled EXIO2");
                break;
        }
    }
    
    // Monitor power state periodically
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000) {  // Every 10 seconds
        lastUpdate = millis();
        update_power_state();
    }
    
    delay(100);
}
```

*Last Updated: October 2, 2025*