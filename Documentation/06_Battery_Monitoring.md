# Battery System - ADC Monitoring

## Hardware Specifications

### Battery Management

- **Battery Type**: Li-Po/Li-ion single cell
- **Voltage Range**: 3.0V - 4.2V
- **Capacity**: Depends on connected battery
- **Charging**: Built-in USB charging circuit
- **Protection**: Over-discharge and over-charge protection

### ADC Monitoring

- **ADC Pin**: GPIO 8
- **Resolution**: 12-bit (0-4095)
- **Reference Voltage**: 3.3V
- **Voltage Divider**: 2:1 ratio
- **Measurement Range**: 0V - 6.6V (theoretical)
- **Practical Range**: 3.0V - 4.2V (battery voltage)

## Pin Configuration

### Battery ADC

```cpp
#define BAT_ADC_PIN           8         // ADC pin for battery monitoring
#define Measurement_offset    0.990476  // Calibration offset
```

### Voltage Divider Circuit

The battery voltage is divided by 2 before reaching the ADC:
- **High Side Resistor**: ~100kΩ
- **Low Side Resistor**: ~100kΩ
- **Actual Battery Voltage**: ADC_Reading × 2 × (3.3V / 4095)

## Global Variables

```cpp
extern float BAT_analogVolts;   // Current battery voltage
```

## Initialization

### Battery Monitoring Setup

```cpp
void BAT_Init(void) {
    // Set ADC resolution to 12-bit
    analogReadResolution(12);
    
    // Configure ADC pin
    pinMode(BAT_ADC_PIN, INPUT);
    
    // Perform initial reading
    BAT_analogVolts = BAT_Get_Volts();
    
    Serial.printf("✓ Battery monitoring initialized: %.2fV\n", BAT_analogVolts);
}
```

## Battery Reading Functions

### Voltage Measurement

```cpp
float BAT_Get_Volts(void) {
    // Take multiple readings for stability
    const int numReadings = 10;
    long total = 0;
    
    for (int i = 0; i < numReadings; i++) {
        total += analogRead(BAT_ADC_PIN);
        delay(1);  // Small delay between readings
    }
    
    // Calculate average ADC value
    int adcValue = total / numReadings;
    
    // Convert to voltage with calibration
    float voltage = (adcValue / 4095.0) * 3.3 * 2.0 * Measurement_offset;
    
    // Filter out invalid readings
    if (voltage < 2.5 || voltage > 5.0) {
        voltage = 0.0;  // Invalid reading
    }
    
    // Update global variable
    BAT_analogVolts = voltage;
    
    return voltage;
}
```

### Battery Percentage Calculation

```cpp
int BAT_Get_Percentage(void) {
    float voltage = BAT_Get_Volts();
    
    // Li-Po discharge curve (approximate)
    if (voltage >= 4.2) return 100;
    if (voltage >= 4.1) return 90;
    if (voltage >= 4.0) return 80;
    if (voltage >= 3.9) return 70;
    if (voltage >= 3.8) return 60;
    if (voltage >= 3.7) return 50;
    if (voltage >= 3.6) return 40;
    if (voltage >= 3.5) return 30;
    if (voltage >= 3.4) return 20;
    if (voltage >= 3.3) return 10;
    if (voltage >= 3.2) return 5;
    if (voltage >= 3.0) return 1;
    
    return 0;  // Battery critically low
}
```

### Advanced Battery Analysis

```cpp
struct BatteryStatus {
    float voltage;
    int percentage;
    bool charging;
    bool lowBattery;
    bool criticalBattery;
    unsigned long lastUpdate;
};

BatteryStatus batteryStatus = {0, 0, false, false, false, 0};

void update_battery_status() {
    batteryStatus.voltage = BAT_Get_Volts();
    batteryStatus.percentage = BAT_Get_Percentage();
    batteryStatus.lowBattery = (batteryStatus.percentage <= 20);
    batteryStatus.criticalBattery = (batteryStatus.percentage <= 5);
    batteryStatus.lastUpdate = millis();
    
    // Detect charging (voltage rising)
    static float lastVoltage = 0;
    static unsigned long lastCheck = 0;
    
    if (millis() - lastCheck > 5000) {  // Check every 5 seconds
        if (batteryStatus.voltage > lastVoltage + 0.05) {
            batteryStatus.charging = true;
        } else if (batteryStatus.voltage < lastVoltage - 0.02) {
            batteryStatus.charging = false;
        }
        
        lastVoltage = batteryStatus.voltage;
        lastCheck = millis();
    }
}

String get_battery_status_string() {
    update_battery_status();
    
    String status = String(batteryStatus.voltage, 2) + "V (" + 
                   String(batteryStatus.percentage) + "%)";
    
    if (batteryStatus.charging) {
        status += " 🔌";
    } else if (batteryStatus.criticalBattery) {
        status += " ⚠️";
    } else if (batteryStatus.lowBattery) {
        status += " 🔋";
    } else {
        status += " 🔋";
    }
    
    return status;
}
```

## Power Management

### Low Battery Detection

```cpp
bool check_low_battery() {
    float voltage = BAT_Get_Volts();
    
    if (voltage < 3.3 && voltage > 0.1) {  // Avoid false readings
        return true;
    }
    return false;
}

void handle_low_battery() {
    if (check_low_battery()) {
        Serial.println("⚠️ Low battery warning!");
        
        // Reduce system performance
        // - Lower display brightness
        // - Reduce CPU frequency
        // - Disable non-essential features
        
        // Display warning on screen
        // show_low_battery_warning();
        
        // Consider entering deep sleep
        if (BAT_Get_Volts() < 3.1) {
            Serial.println("🚨 Critical battery - entering deep sleep");
            // enter_deep_sleep();
        }
    }
}
```

### Battery Calibration

```cpp
void calibrate_battery_reading() {
    Serial.println("Battery Calibration Mode");
    Serial.println("Connect a multimeter to measure actual battery voltage");
    Serial.println("Press any key when ready...");
    
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read();  // Clear buffer
    
    // Take reading
    float measuredVoltage = BAT_Get_Volts();
    
    Serial.printf("Current reading: %.3fV\n", measuredVoltage);
    Serial.println("Enter actual voltage (e.g., 4.150):");
    
    // Wait for user input
    String input = "";
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                break;
            }
            input += c;
        }
    }
    
    float actualVoltage = input.toFloat();
    if (actualVoltage > 2.0 && actualVoltage < 5.0) {
        float newOffset = (actualVoltage / measuredVoltage) * Measurement_offset;
        
        Serial.printf("Old offset: %.6f\n", Measurement_offset);
        Serial.printf("New offset: %.6f\n", newOffset);
        Serial.println("Update BAT_Driver.h with new offset value");
    } else {
        Serial.println("Invalid voltage entered");
    }
}
```

## Battery Logging

### Historical Data

```cpp
struct BatteryLog {
    unsigned long timestamp;
    float voltage;
    int percentage;
    bool charging;
};

#define MAX_BATTERY_LOGS 100
BatteryLog batteryLogs[MAX_BATTERY_LOGS];
int batteryLogIndex = 0;

void log_battery_data() {
    BatteryLog* log = &batteryLogs[batteryLogIndex];
    
    log->timestamp = millis();
    log->voltage = BAT_Get_Volts();
    log->percentage = BAT_Get_Percentage();
    log->charging = batteryStatus.charging;
    
    batteryLogIndex = (batteryLogIndex + 1) % MAX_BATTERY_LOGS;
}

void print_battery_history() {
    Serial.println("Battery History (last 100 readings):");
    Serial.println("Time(s)\tVoltage\t%\tCharging");
    
    for (int i = 0; i < MAX_BATTERY_LOGS; i++) {
        int index = (batteryLogIndex + i) % MAX_BATTERY_LOGS;
        BatteryLog* log = &batteryLogs[index];
        
        if (log->timestamp > 0) {
            Serial.printf("%lu\t%.2f\t%d%%\t%s\n", 
                         log->timestamp / 1000,
                         log->voltage,
                         log->percentage,
                         log->charging ? "Yes" : "No");
        }
    }
}

void save_battery_log_to_sd() {
    File logFile = SD_MMC.open("/battery_log.csv", FILE_WRITE);
    if (!logFile) {
        Serial.println("❌ Failed to open battery log file");
        return;
    }
    
    // Write header if file is new
    if (logFile.size() == 0) {
        logFile.println("Timestamp,Voltage,Percentage,Charging");
    }
    
    // Write current data
    update_battery_status();
    logFile.printf("%lu,%.3f,%d,%d\n",
                   millis(),
                   batteryStatus.voltage,
                   batteryStatus.percentage,
                   batteryStatus.charging ? 1 : 0);
    
    logFile.close();
}
```

## Integration with System

### LVGL Battery Indicator

```cpp
lv_obj_t* battery_bar;
lv_obj_t* battery_label;

void create_battery_ui() {
    // Create battery percentage bar
    battery_bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(battery_bar, 60, 8);
    lv_obj_align(battery_bar, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_bar_set_range(battery_bar, 0, 100);
    
    // Create battery voltage label
    battery_label = lv_label_create(lv_scr_act());
    lv_obj_align_to(battery_label, battery_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
    
    // Update initially
    update_battery_ui();
}

void update_battery_ui() {
    update_battery_status();
    
    // Update bar value
    lv_bar_set_value(battery_bar, batteryStatus.percentage, LV_ANIM_ON);
    
    // Set color based on battery level
    if (batteryStatus.percentage > 50) {
        lv_obj_set_style_bg_color(battery_bar, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
    } else if (batteryStatus.percentage > 20) {
        lv_obj_set_style_bg_color(battery_bar, lv_color_hex(0xFFAA00), LV_PART_INDICATOR);
    } else {
        lv_obj_set_style_bg_color(battery_bar, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    }
    
    // Update label
    lv_label_set_text_fmt(battery_label, "%.2fV", batteryStatus.voltage);
}
```

### System Task Integration

```cpp
void battery_monitoring_task(void *parameter) {
    while (true) {
        // Update battery status
        update_battery_status();
        
        // Log data every 5 minutes
        static unsigned long lastLog = 0;
        if (millis() - lastLog > 300000) {
            lastLog = millis();
            log_battery_data();
            save_battery_log_to_sd();
        }
        
        // Check for low battery
        handle_low_battery();
        
        // Update UI
        update_battery_ui();
        
        // Sleep for 30 seconds
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

void start_battery_monitoring() {
    xTaskCreatePinnedToCore(
        battery_monitoring_task,
        "BatteryTask",
        2048,
        NULL,
        2,  // Priority
        NULL,
        1   // Core 1
    );
}
```

## Troubleshooting

### Common Issues

1. **Inaccurate Readings**
   - Check voltage divider resistors
   - Calibrate measurement offset
   - Verify ADC reference voltage
   - Check for loose connections

2. **Noisy Measurements**
   - Add filtering capacitor
   - Take multiple readings and average
   - Check for ground loops
   - Shield ADC lines from interference

3. **False Low Battery Warnings**
   - Calibrate voltage thresholds
   - Account for voltage drop under load
   - Implement hysteresis in thresholds
   - Check battery health

4. **No Battery Detection**
   - Verify battery connection
   - Check charging circuit
   - Test with known good battery
   - Measure voltages with multimeter

### Debug Functions

```cpp
void battery_system_test() {
    Serial.println("=== Battery System Test ===");
    
    // Raw ADC readings
    Serial.printf("Raw ADC: %d\n", analogRead(BAT_ADC_PIN));
    
    // Voltage calculation steps
    int adcValue = analogRead(BAT_ADC_PIN);
    float adcVoltage = (adcValue / 4095.0) * 3.3;
    float batteryVoltage = adcVoltage * 2.0 * Measurement_offset;
    
    Serial.printf("ADC Value: %d\n", adcValue);
    Serial.printf("ADC Voltage: %.3fV\n", adcVoltage);
    Serial.printf("Battery Voltage: %.3fV\n", batteryVoltage);
    Serial.printf("Battery Percentage: %d%%\n", BAT_Get_Percentage());
    
    // Continuous monitoring
    Serial.println("\nContinuous monitoring (30 readings):");
    for (int i = 0; i < 30; i++) {
        float voltage = BAT_Get_Volts();
        int percentage = BAT_Get_Percentage();
        
        Serial.printf("%2d: %.3fV (%d%%)\n", i + 1, voltage, percentage);
        delay(1000);
    }
    
    Serial.println("Battery test complete");
}

void battery_calibration_wizard() {
    Serial.println("=== Battery Calibration Wizard ===");
    Serial.println("You will need a multimeter for this process");
    Serial.println();
    
    // Test at different voltage levels
    float testVoltages[] = {3.0, 3.3, 3.7, 4.0, 4.2};
    int numTests = sizeof(testVoltages) / sizeof(testVoltages[0]);
    
    for (int i = 0; i < numTests; i++) {
        Serial.printf("Test %d: Set battery to %.1fV and press Enter\n", 
                     i + 1, testVoltages[i]);
        
        while (Serial.read() != '\n') {
            delay(10);
        }
        
        float measured = BAT_Get_Volts();
        float error = measured - testVoltages[i];
        float errorPercent = (error / testVoltages[i]) * 100;
        
        Serial.printf("  Measured: %.3fV\n", measured);
        Serial.printf("  Error: %.3fV (%.1f%%)\n", error, errorPercent);
        Serial.println();
    }
    
    Serial.println("Calibration complete");
}
```

## Example Implementation

### Simple Battery Monitor

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize battery monitoring
    BAT_Init();
    
    Serial.println("Battery Monitor Ready");
    Serial.println("Commands:");
    Serial.println("  'v' - Show voltage");
    Serial.println("  'p' - Show percentage");
    Serial.println("  's' - Show status");
    Serial.println("  'c' - Calibrate");
}

void loop() {
    // Handle serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        
        switch (cmd) {
            case 'v':
                Serial.printf("Battery Voltage: %.3fV\n", BAT_Get_Volts());
                break;
            case 'p':
                Serial.printf("Battery Percentage: %d%%\n", BAT_Get_Percentage());
                break;
            case 's':
                Serial.printf("Battery Status: %s\n", get_battery_status_string().c_str());
                break;
            case 'c':
                calibrate_battery_reading();
                break;
        }
    }
    
    // Update status every 10 seconds
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000) {
        lastUpdate = millis();
        
        Serial.printf("Battery: %s\n", get_battery_status_string().c_str());
        
        // Check for warnings
        handle_low_battery();
    }
    
    delay(100);
}
```

*Last Updated: October 2, 2025*