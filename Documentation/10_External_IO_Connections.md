# External I/O Connections - ESP32-S3-Touch-LCD-1.85C

## Overview

This document provides detailed information about external I/O connections, expansion capabilities, and interfacing options for the ESP32-S3-Touch-LCD-1.85C development board.

## Available I/O Pins

### Direct GPIO Pins (Limited)

Most GPIO pins are used for internal peripherals. Available pins for external connections:

```cpp
// Available GPIO pins for external use
#define EXT_GPIO_1            3         // General purpose I/O
#define EXT_GPIO_2            8         // General purpose I/O  
#define EXT_GPIO_3            9         // General purpose I/O
#define EXT_GPIO_4            16        // General purpose I/O
#define EXT_GPIO_5            17        // General purpose I/O
#define EXT_GPIO_6            20        // General purpose I/O

// Note: These pins may have internal pull-ups/pull-downs
// Check schematic before using for sensitive applications
```

### Extended I/O via TCA9554PWR

```cpp
// Extended GPIO through I2C I/O expander
#define EXT_IO_1              1         // Available via TCA9554PWR
#define EXT_IO_2              2         // Available via TCA9554PWR
#define EXT_IO_3              3         // Available via TCA9554PWR
#define EXT_IO_4              4         // Available via TCA9554PWR
#define EXT_IO_5              5         // Available via TCA9554PWR
#define EXT_IO_6              6         // Available via TCA9554PWR
#define EXT_IO_7              7         // Available via TCA9554PWR

// Note: EXIO1 and EXIO2 are used for reset control
// EXIO3-EXIO7 are available for external connections
```

## I2C Interface Expansion

### Shared I2C Bus

```cpp
#define I2C_SCL_PIN           10        // Shared I2C clock
#define I2C_SDA_PIN           11        // Shared I2C data
#define I2C_FREQUENCY         100000    // 100kHz (can go up to 400kHz)

// Existing I2C devices:
// - TCA9554PWR (0x20) - I/O Expander
// - CST816T (0x15) - Touch Controller  
// - PCF85063 (0x51) - RTC

// Available I2C addresses for external devices:
// 0x08-0x14, 0x16-0x1F, 0x21-0x50, 0x52-0x77
```

### I2C Device Connection

```cpp
class ExternalI2CDevice {
private:
    uint8_t deviceAddress;
    
public:
    ExternalI2CDevice(uint8_t addr) : deviceAddress(addr) {}
    
    bool begin() {
        Wire.beginTransmission(deviceAddress);
        uint8_t error = Wire.endTransmission();
        return (error == 0);
    }
    
    uint8_t readRegister(uint8_t reg) {
        Wire.beginTransmission(deviceAddress);
        Wire.write(reg);
        Wire.endTransmission();
        
        Wire.requestFrom(deviceAddress, (uint8_t)1);
        if (Wire.available()) {
            return Wire.read();
        }
        return 0;
    }
    
    bool writeRegister(uint8_t reg, uint8_t data) {
        Wire.beginTransmission(deviceAddress);
        Wire.write(reg);
        Wire.write(data);
        uint8_t error = Wire.endTransmission();
        return (error == 0);
    }
};
```

## SPI Interface (Limited)

### SPI Pins (Mostly Used)

```cpp
// Primary SPI (used by display)
#define SPI_SCK               40        // Used by display
#define SPI_MISO              46        // Used by display  
#define SPI_MOSI              45        // Used by display
#define SPI_CS_DISPLAY        42        // Display chip select

// Secondary SPI option (limited pins available)
#define SPI2_SCK              EXT_GPIO_1  // Alternative SPI clock
#define SPI2_MISO             EXT_GPIO_2  // Alternative SPI MISO
#define SPI2_MOSI             EXT_GPIO_3  // Alternative SPI MOSI
#define SPI2_CS               EXT_GPIO_4  // Alternative SPI CS
```

## UART Interfaces

### Available UART

```cpp
// UART0 (used for programming/debugging)
#define UART0_TX              43        // USB CDC TX
#define UART0_RX              44        // USB CDC RX

// UART1 (available for external use)
#define UART1_TX              EXT_GPIO_5  // External UART TX
#define UART1_RX              EXT_GPIO_6  // External UART RX

// Software Serial (any available GPIO)
#include <SoftwareSerial.h>
SoftwareSerial extSerial(EXT_GPIO_1, EXT_GPIO_2);  // RX, TX
```

## ADC Inputs

### Available ADC Channels

```cpp
// ADC1 channels (available if GPIO is free)
#define ADC_BATTERY           1         // Used for battery monitoring
#define ADC_EXT_1             EXT_GPIO_3  // External analog input 1
#define ADC_EXT_2             EXT_GPIO_4  // External analog input 2

// ADC configuration
void setupExternalADC() {
    analogReadResolution(12);  // 12-bit resolution (0-4095)
    analogSetAttenuation(ADC_11db);  // Full range (0-3.3V)
}

float readExternalVoltage(uint8_t pin) {
    int adcValue = analogRead(pin);
    return (adcValue * 3.3) / 4095.0;
}
```

## PWM Outputs

### PWM Configuration

```cpp
// PWM channels (using LEDC)
#define PWM_FREQUENCY         5000      // 5kHz PWM frequency
#define PWM_RESOLUTION        8         // 8-bit resolution (0-255)

#define PWM_CHANNEL_1         0         // PWM channel 1
#define PWM_CHANNEL_2         1         // PWM channel 2
#define PWM_CHANNEL_3         2         // PWM channel 3

void setupExternalPWM() {
    // Configure PWM channels
    ledcSetup(PWM_CHANNEL_1, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_2, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_3, PWM_FREQUENCY, PWM_RESOLUTION);
    
    // Attach pins
    ledcAttachPin(EXT_GPIO_1, PWM_CHANNEL_1);
    ledcAttachPin(EXT_GPIO_2, PWM_CHANNEL_2);
    ledcAttachPin(EXT_GPIO_3, PWM_CHANNEL_3);
}

void setExternalPWM(uint8_t channel, uint8_t duty) {
    ledcWrite(channel, duty);
}
```

## Common External Connections

### LED Control

```cpp
// External LED control
class ExternalLED {
private:
    uint8_t pin;
    uint8_t pwmChannel;
    bool isDigital;
    
public:
    ExternalLED(uint8_t p, bool digital = true) {
        pin = p;
        isDigital = digital;
        
        if (digital) {
            pinMode(pin, OUTPUT);
        } else {
            pwmChannel = 0;  // Assign PWM channel
            ledcSetup(pwmChannel, 5000, 8);
            ledcAttachPin(pin, pwmChannel);
        }
    }
    
    void on() {
        if (isDigital) {
            digitalWrite(pin, HIGH);
        } else {
            ledcWrite(pwmChannel, 255);
        }
    }
    
    void off() {
        if (isDigital) {
            digitalWrite(pin, LOW);
        } else {
            ledcWrite(pwmChannel, 0);
        }
    }
    
    void brightness(uint8_t level) {  // 0-255
        if (!isDigital) {
            ledcWrite(pwmChannel, level);
        }
    }
    
    void blink(int delayMs = 500) {
        on();
        delay(delayMs);
        off();
        delay(delayMs);
    }
};

// Usage
ExternalLED statusLED(EXT_GPIO_1);              // Digital LED
ExternalLED brightLED(EXT_GPIO_2, false);       // PWM LED
```

### Button Input

```cpp
// External button control with debouncing
class ExternalButton {
private:
    uint8_t pin;
    bool lastState;
    bool currentState;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;
    bool pullup;
    
public:
    ExternalButton(uint8_t p, bool enablePullup = true) {
        pin = p;
        pullup = enablePullup;
        debounceDelay = 50;  // 50ms debounce
        lastDebounceTime = 0;
        
        if (pullup) {
            pinMode(pin, INPUT_PULLUP);
            lastState = HIGH;
        } else {
            pinMode(pin, INPUT);
            lastState = LOW;
        }
        currentState = lastState;
    }
    
    bool read() {
        bool reading = digitalRead(pin);
        
        if (reading != lastState) {
            lastDebounceTime = millis();
        }
        
        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading != currentState) {
                currentState = reading;
            }
        }
        
        lastState = reading;
        return currentState;
    }
    
    bool pressed() {
        if (pullup) {
            return !read();  // LOW when pressed (pullup)
        } else {
            return read();   // HIGH when pressed (pulldown)
        }
    }
    
    bool released() {
        return !pressed();
    }
};

// Usage
ExternalButton userButton(EXT_GPIO_3);
```

### Servo Control

```cpp
// External servo control
class ExternalServo {
private:
    uint8_t pin;
    uint8_t pwmChannel;
    int minPulse;
    int maxPulse;
    int currentAngle;
    
public:
    ExternalServo(uint8_t p, uint8_t channel = 0) {
        pin = p;
        pwmChannel = channel;
        minPulse = 544;   // 0.544ms pulse width
        maxPulse = 2400;  // 2.4ms pulse width
        currentAngle = 90;
        
        // 50Hz for servo control
        ledcSetup(pwmChannel, 50, 16);  // 16-bit resolution for precision
        ledcAttachPin(pin, pwmChannel);
        
        write(90);  // Center position
    }
    
    void write(int angle) {
        angle = constrain(angle, 0, 180);
        currentAngle = angle;
        
        int pulseWidth = map(angle, 0, 180, minPulse, maxPulse);
        int dutyCycle = map(pulseWidth, 0, 20000, 0, 65535);  // 20ms period
        
        ledcWrite(pwmChannel, dutyCycle);
    }
    
    int read() {
        return currentAngle;
    }
    
    void sweep(int startAngle = 0, int endAngle = 180, int stepDelay = 15) {
        for (int angle = startAngle; angle <= endAngle; angle++) {
            write(angle);
            delay(stepDelay);
        }
        for (int angle = endAngle; angle >= startAngle; angle--) {
            write(angle);
            delay(stepDelay);
        }
    }
};

// Usage
ExternalServo myServo(EXT_GPIO_4, PWM_CHANNEL_1);
```

### Sensor Interfaces

```cpp
// DHT22 Temperature/Humidity Sensor
#include "DHTesp.h"

class ExternalDHT {
private:
    DHTesp dht;
    uint8_t pin;
    
public:
    ExternalDHT(uint8_t p) {
        pin = p;
        dht.setup(pin, DHTesp::DHT22);
    }
    
    float readTemperature() {
        return dht.getTemperature();
    }
    
    float readHumidity() {
        return dht.getHumidity();
    }
    
    bool isValidReading() {
        return (dht.getStatus() == DHTesp::ERROR_NONE);
    }
};

// Ultrasonic Distance Sensor (HC-SR04)
class ExternalUltrasonic {
private:
    uint8_t trigPin;
    uint8_t echoPin;
    
public:
    ExternalUltrasonic(uint8_t trig, uint8_t echo) {
        trigPin = trig;
        echoPin = echo;
        
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }
    
    float readDistance() {  // Returns distance in cm
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        long duration = pulseIn(echoPin, HIGH);
        float distance = (duration * 0.034) / 2;
        
        return distance;
    }
};

// Usage
ExternalDHT tempSensor(EXT_GPIO_1);
ExternalUltrasonic distSensor(EXT_GPIO_2, EXT_GPIO_3);
```

## Extended I/O via TCA9554PWR

### I/O Expander Usage

```cpp
// Use I/O expander for additional pins
void setupExpandedIO() {
    // Initialize TCA9554PWR
    TCA9554PWR_Init(0x1C);  // EXIO3-EXIO7 as inputs, others as outputs
    
    // Configure available pins
    Mode_EXIO(EXIO_PIN3, 0);  // Output
    Mode_EXIO(EXIO_PIN4, 1);  // Input
    Mode_EXIO(EXIO_PIN5, 0);  // Output
    Mode_EXIO(EXIO_PIN6, 1);  // Input
    Mode_EXIO(EXIO_PIN7, 0);  // Output
}

// Extended GPIO class using I/O expander
class ExpandedIO {
public:
    static void digitalWrite(uint8_t pin, bool state) {
        if (pin >= 3 && pin <= 7) {
            Set_EXIO(pin, state ? High : Low);
        }
    }
    
    static bool digitalRead(uint8_t pin) {
        if (pin >= 3 && pin <= 7) {
            return (Read_EXIO(pin) == High);
        }
        return false;
    }
    
    static void pinMode(uint8_t pin, uint8_t mode) {
        if (pin >= 3 && pin <= 7) {
            Mode_EXIO(pin, (mode == INPUT) ? 1 : 0);
        }
    }
};
```

## Connector Pinouts

### Theoretical External Connector (if added)

```
Pin | Function      | Description
----|---------------|---------------------------
1   | GND          | Ground
2   | 3.3V         | Power supply (limited current)
3   | GPIO 3       | Digital I/O
4   | GPIO 8       | Digital I/O
5   | GPIO 9       | Digital I/O / ADC
6   | GPIO 16      | Digital I/O
7   | GPIO 17      | Digital I/O
8   | GPIO 20      | Digital I/O
9   | I2C SDA      | Shared I2C data (GPIO 11)
10  | I2C SCL      | Shared I2C clock (GPIO 10)
11  | EXT_IO3      | Extended I/O via TCA9554PWR
12  | EXT_IO4      | Extended I/O via TCA9554PWR
13  | EXT_IO5      | Extended I/O via TCA9554PWR
14  | EXT_IO6      | Extended I/O via TCA9554PWR
15  | EXT_IO7      | Extended I/O via TCA9554PWR
16  | 5V           | External power input
```

## Power Considerations

### Current Limitations

```cpp
// Maximum current ratings
#define MAX_GPIO_CURRENT      40        // mA per GPIO pin
#define MAX_TOTAL_CURRENT     200       // mA total for all GPIO
#define MAX_3V3_SUPPLY        500       // mA from 3.3V regulator
#define MAX_5V_INPUT          2000      // mA 5V input capability

// Power monitoring
void checkPowerConsumption() {
    float batteryVoltage = BAT_Get_Volts();
    
    if (batteryVoltage < 3.2) {
        Serial.println("WARNING: Low battery - reducing external I/O");
        // Disable non-essential external devices
    }
}
```

### External Power Supply

```cpp
// External power detection
#define EXT_POWER_DETECT      EXT_GPIO_6  // External power detection pin

bool isExternalPowerPresent() {
    return digitalRead(EXT_POWER_DETECT);
}

void managePowerMode() {
    if (isExternalPowerPresent()) {
        // Full power mode
        Serial.println("External power detected - full performance mode");
    } else {
        // Battery power mode - reduce consumption
        Serial.println("Battery power - reducing consumption");
        enableWiFiPowerSaving();
        setWiFiTxPower(WIFI_POWER_5dBm);
    }
}
```

## Troubleshooting External I/O

### Common Issues

1. **GPIO Conflicts**
   - Check pin usage in system overview
   - Verify no internal peripheral conflicts
   - Use I/O expander for additional pins

2. **Power Issues**
   - Check current consumption
   - Verify 3.3V supply capacity
   - Monitor battery voltage

3. **I2C Bus Issues**
   - Check for address conflicts
   - Verify pull-up resistors (usually built-in)
   - Test bus with I2C scanner

4. **Timing Issues**
   - Add delays for slow devices
   - Check interrupt handling
   - Verify clock speeds

### Debug Functions

```cpp
void scanI2CBus() {
    Serial.println("Scanning I2C bus...");
    
    int deviceCount = 0;
    for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.printf("I2C device found at address 0x%02X\n", address);
            deviceCount++;
        }
    }
    
    Serial.printf("Found %d I2C devices\n", deviceCount);
}

void testExternalGPIO() {
    Serial.println("Testing external GPIO pins...");
    
    uint8_t testPins[] = {EXT_GPIO_1, EXT_GPIO_2, EXT_GPIO_3, EXT_GPIO_4};
    int numPins = sizeof(testPins) / sizeof(testPins[0]);
    
    for (int i = 0; i < numPins; i++) {
        Serial.printf("Testing GPIO %d...\n", testPins[i]);
        
        pinMode(testPins[i], OUTPUT);
        digitalWrite(testPins[i], HIGH);
        delay(100);
        
        pinMode(testPins[i], INPUT_PULLDOWN);
        bool state = digitalRead(testPins[i]);
        
        Serial.printf("  Result: %s\n", state ? "OK" : "FAILED");
    }
}
```

## Example Implementation

```cpp
#include "ExternalIO.h"

// External devices
ExternalLED statusLED(EXT_GPIO_1);
ExternalButton userButton(EXT_GPIO_2);
ExternalServo panServo(EXT_GPIO_3, PWM_CHANNEL_1);
ExternalDHT envSensor(EXT_GPIO_4);

void setup() {
    Serial.begin(115200);
    
    // Initialize I2C for I/O expander
    I2C_Init();
    setupExpandedIO();
    
    // Initialize external devices
    statusLED.on();
    
    Serial.println("External I/O system ready");
}

void loop() {
    // Check button
    if (userButton.pressed()) {
        statusLED.blink();
        panServo.sweep();
    }
    
    // Read environmental data
    static unsigned long lastReading = 0;
    if (millis() - lastReading > 5000) {
        lastReading = millis();
        
        float temp = envSensor.readTemperature();
        float humidity = envSensor.readHumidity();
        
        if (envSensor.isValidReading()) {
            Serial.printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temp, humidity);
        }
    }
    
    delay(100);
}
```

*Last Updated: October 2, 2024*