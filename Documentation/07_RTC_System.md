# Real-Time Clock - PCF85063

## Hardware Specifications

### RTC Controller: PCF85063

- **Interface**: I2C
- **I2C Address**: 0x51 (7-bit)
- **Frequency**: 32.768kHz crystal
- **Accuracy**: ±20ppm at 25°C
- **Operating Voltage**: 1.8V to 5.5V
- **Current Consumption**: <1µA (typical)
- **Battery Backup**: Supported

### Time/Date Features

- **Time Resolution**: Seconds
- **Date Range**: 2000-2099 (century bit)
- **Calendar**: Automatic leap year compensation
- **Alarms**: 1 alarm with interrupt capability
- **Clock Output**: 32.768kHz, 1024Hz, 32Hz, 1Hz
- **Timer**: Countdown timer with interrupt

## Pin Configuration

### I2C Connection

```cpp
// Uses main I2C bus shared with other devices
#define I2C_SCL_PIN           10        // I2C Clock
#define I2C_SDA_PIN           11        // I2C Data
#define PCF85063_ADDR         0x51      // RTC I2C Address
```

### Interrupt Pin

```cpp
// Interrupt pin for alarms and timer (if used)
#define RTC_INT_PIN           -1        // Not connected in current design
```

## Register Map

### Time/Date Registers

```cpp
#define PCF85063_REG_SECONDS    0x04
#define PCF85063_REG_MINUTES    0x05
#define PCF85063_REG_HOURS      0x06
#define PCF85063_REG_DAYS       0x07
#define PCF85063_REG_WEEKDAYS   0x08
#define PCF85063_REG_MONTHS     0x09
#define PCF85063_REG_YEARS      0x0A
```

### Control Registers

```cpp
#define PCF85063_REG_CTRL1      0x00
#define PCF85063_REG_CTRL2      0x01
#define PCF85063_REG_OFFSET     0x02
#define PCF85063_REG_RAM_BYTE   0x03
```

### Alarm Registers

```cpp
#define PCF85063_REG_ALARM_SECONDS  0x0B
#define PCF85063_REG_ALARM_MINUTES  0x0C
#define PCF85063_REG_ALARM_HOURS    0x0D
#define PCF85063_REG_ALARM_DAYS     0x0E
#define PCF85063_REG_ALARM_WEEKDAYS 0x0F
```

## Data Structures

### DateTime Structure

```cpp
struct DateTime {
    uint8_t second;     // 0-59
    uint8_t minute;     // 0-59
    uint8_t hour;       // 0-23 (24-hour format)
    uint8_t day;        // 1-31
    uint8_t weekday;    // 0-6 (0=Sunday)
    uint8_t month;      // 1-12
    uint8_t year;       // 0-99 (20xx)
};

DateTime currentTime;
```

### Alarm Structure

```cpp
struct AlarmTime {
    uint8_t second;     // 0-59 or 0x80 (disabled)
    uint8_t minute;     // 0-59 or 0x80 (disabled)
    uint8_t hour;       // 0-23 or 0x80 (disabled)
    uint8_t day;        // 1-31 or 0x80 (disabled)
    uint8_t weekday;    // 0-6 or 0x80 (disabled)
};
```

## Initialization

### RTC Setup

```cpp
void PCF85063_Init() {
    Serial.println("Initializing RTC PCF85063...");
    
    // Test I2C communication
    Wire.beginTransmission(PCF85063_ADDR);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        Serial.printf("❌ RTC not found at address 0x%02X (error: %d)\n", 
                     PCF85063_ADDR, error);
        return;
    }
    
    // Read control registers
    uint8_t ctrl1 = read_rtc_register(PCF85063_REG_CTRL1);
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    
    Serial.printf("✓ RTC found - CTRL1: 0x%02X, CTRL2: 0x%02X\n", ctrl1, ctrl2);
    
    // Configure RTC
    // Set 24-hour mode, disable test mode
    write_rtc_register(PCF85063_REG_CTRL1, 0x00);
    
    // Clear interrupt flags, enable alarm interrupt (if needed)
    write_rtc_register(PCF85063_REG_CTRL2, 0x00);
    
    // Check if oscillator is running
    if (ctrl1 & 0x20) {  // OS bit set = oscillator stopped
        Serial.println("⚠️ RTC oscillator was stopped - time may be invalid");
        
        // Set a default time if oscillator was stopped
        DateTime defaultTime = {0, 0, 12, 1, 1, 1, 24};  // 12:00:00 Jan 1, 2024, Monday
        PCF85063_SetDateTime(&defaultTime);
    }
    
    // Read current time
    PCF85063_GetDateTime(&currentTime);
    
    Serial.printf("✓ Current time: %02d:%02d:%02d %02d/%02d/20%02d\n",
                 currentTime.hour, currentTime.minute, currentTime.second,
                 currentTime.month, currentTime.day, currentTime.year);
}
```

## Time/Date Functions

### Read RTC Registers

```cpp
uint8_t read_rtc_register(uint8_t reg) {
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(PCF85063_ADDR, 1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

void write_rtc_register(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}
```

### BCD Conversion

```cpp
uint8_t bcd_to_decimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}
```

### Get Date/Time

```cpp
void PCF85063_GetDateTime(DateTime* dt) {
    // Read all time registers at once
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(PCF85063_REG_SECONDS);
    Wire.endTransmission();
    
    Wire.requestFrom(PCF85063_ADDR, 7);
    
    if (Wire.available() >= 7) {
        dt->second = bcd_to_decimal(Wire.read() & 0x7F);  // Mask VL bit
        dt->minute = bcd_to_decimal(Wire.read() & 0x7F);
        dt->hour = bcd_to_decimal(Wire.read() & 0x3F);    // 24-hour format
        dt->day = bcd_to_decimal(Wire.read() & 0x3F);
        dt->weekday = bcd_to_decimal(Wire.read() & 0x07);
        dt->month = bcd_to_decimal(Wire.read() & 0x1F);
        dt->year = bcd_to_decimal(Wire.read());
    }
}
```

### Set Date/Time

```cpp
void PCF85063_SetDateTime(DateTime* dt) {
    // Stop the oscillator while setting time
    uint8_t ctrl1 = read_rtc_register(PCF85063_REG_CTRL1);
    write_rtc_register(PCF85063_REG_CTRL1, ctrl1 | 0x20);  // Set STOP bit
    
    // Write all time registers
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(PCF85063_REG_SECONDS);
    Wire.write(decimal_to_bcd(dt->second));
    Wire.write(decimal_to_bcd(dt->minute));
    Wire.write(decimal_to_bcd(dt->hour));
    Wire.write(decimal_to_bcd(dt->day));
    Wire.write(decimal_to_bcd(dt->weekday));
    Wire.write(decimal_to_bcd(dt->month));
    Wire.write(decimal_to_bcd(dt->year));
    Wire.endTransmission();
    
    // Restart the oscillator
    write_rtc_register(PCF85063_REG_CTRL1, ctrl1 & ~0x20);  // Clear STOP bit
    
    Serial.printf("✓ Time set: %02d:%02d:%02d %02d/%02d/20%02d\n",
                 dt->hour, dt->minute, dt->second,
                 dt->month, dt->day, dt->year);
}
```

### Time Formatting

```cpp
String format_time(DateTime* dt, bool include_seconds = true) {
    if (include_seconds) {
        return String(dt->hour, DEC) + ":" + 
               (dt->minute < 10 ? "0" : "") + String(dt->minute, DEC) + ":" +
               (dt->second < 10 ? "0" : "") + String(dt->second, DEC);
    } else {
        return String(dt->hour, DEC) + ":" + 
               (dt->minute < 10 ? "0" : "") + String(dt->minute, DEC);
    }
}

String format_date(DateTime* dt) {
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    
    return String(weekdays[dt->weekday]) + ", " +
           String(months[dt->month - 1]) + " " +
           String(dt->day, DEC) + ", 20" + 
           (dt->year < 10 ? "0" : "") + String(dt->year, DEC);
}

String format_datetime(DateTime* dt) {
    return format_time(dt) + " " + format_date(dt);
}
```

## Alarm Functions

### Set Alarm

```cpp
void PCF85063_SetAlarm(AlarmTime* alarm) {
    // Disable alarm first
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    write_rtc_register(PCF85063_REG_CTRL2, ctrl2 & ~0x80);  // Clear AIE bit
    
    // Write alarm registers
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(PCF85063_REG_ALARM_SECONDS);
    Wire.write(alarm->second);    // 0x80 = disabled
    Wire.write(alarm->minute);    // 0x80 = disabled
    Wire.write(alarm->hour);      // 0x80 = disabled
    Wire.write(alarm->day);       // 0x80 = disabled
    Wire.write(alarm->weekday);   // 0x80 = disabled
    Wire.endTransmission();
    
    // Enable alarm interrupt
    write_rtc_register(PCF85063_REG_CTRL2, ctrl2 | 0x80);  // Set AIE bit
    
    Serial.println("✓ Alarm set");
}

void PCF85063_DisableAlarm() {
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    write_rtc_register(PCF85063_REG_CTRL2, ctrl2 & ~0x80);  // Clear AIE bit
    
    Serial.println("✓ Alarm disabled");
}

bool PCF85063_CheckAlarm() {
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    
    if (ctrl2 & 0x40) {  // AF bit set
        // Clear alarm flag
        write_rtc_register(PCF85063_REG_CTRL2, ctrl2 & ~0x40);
        return true;
    }
    return false;
}
```

## System Integration

### RTC Loop Function

```cpp
void PCF85063_Loop() {
    static unsigned long lastUpdate = 0;
    
    // Update time every second
    if (millis() - lastUpdate >= 1000) {
        lastUpdate = millis();
        
        // Read current time
        PCF85063_GetDateTime(&currentTime);
        
        // Check for alarm
        if (PCF85063_CheckAlarm()) {
            Serial.println("🔔 RTC Alarm triggered!");
            // Handle alarm event
        }
    }
}
```

### Time Synchronization

```cpp
void sync_time_from_ntp() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected - cannot sync time");
        return;
    }
    
    Serial.println("Synchronizing time from NTP...");
    
    // Configure NTP
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    
    // Wait for time synchronization
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    
    while (timeinfo.tm_year < (2024 - 1900) && ++retry < retry_count) {
        Serial.printf("Waiting for system time to be set... (%d/%d)\n", 
                     retry, retry_count);
        delay(2000);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    
    if (timeinfo.tm_year >= (2024 - 1900)) {
        // Convert to RTC format
        DateTime ntpTime;
        ntpTime.second = timeinfo.tm_sec;
        ntpTime.minute = timeinfo.tm_min;
        ntpTime.hour = timeinfo.tm_hour;
        ntpTime.day = timeinfo.tm_mday;
        ntpTime.weekday = timeinfo.tm_wday;
        ntpTime.month = timeinfo.tm_mon + 1;
        ntpTime.year = timeinfo.tm_year - 100;  // Years since 2000
        
        // Set RTC time
        PCF85063_SetDateTime(&ntpTime);
        
        Serial.printf("✓ Time synchronized: %s\n", 
                     format_datetime(&ntpTime).c_str());
    } else {
        Serial.println("❌ Failed to synchronize time");
    }
}
```

### LVGL Clock Display

```cpp
lv_obj_t* time_label;
lv_obj_t* date_label;

void create_clock_ui() {
    // Create time display
    time_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_32, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -20);
    
    // Create date display
    date_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_16, 0);
    lv_obj_align_to(date_label, time_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    
    // Update initially
    update_clock_ui();
}

void update_clock_ui() {
    // Update time display
    lv_label_set_text(time_label, format_time(&currentTime, true).c_str());
    
    // Update date display
    lv_label_set_text(date_label, format_date(&currentTime).c_str());
}
```

## Advanced Features

### Timer Functions

```cpp
void PCF85063_SetTimer(uint8_t seconds) {
    // Configure timer for countdown
    write_rtc_register(PCF85063_REG_TIMER_VALUE, seconds);
    
    // Enable timer with 1Hz frequency
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    write_rtc_register(PCF85063_REG_CTRL2, (ctrl2 & 0xFC) | 0x02);  // TI_TP = 1, TIE = 1
    
    Serial.printf("✓ Timer set for %d seconds\n", seconds);
}

bool PCF85063_CheckTimer() {
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    
    if (ctrl2 & 0x10) {  // TF bit set
        // Clear timer flag
        write_rtc_register(PCF85063_REG_CTRL2, ctrl2 & ~0x10);
        return true;
    }
    return false;
}
```

### Clock Output

```cpp
void PCF85063_SetClockOutput(uint8_t frequency) {
    // frequency: 0=32768Hz, 1=1024Hz, 2=32Hz, 3=1Hz
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    
    // Clear COF bits and set new frequency
    ctrl2 = (ctrl2 & 0xE7) | ((frequency & 0x03) << 3);
    
    write_rtc_register(PCF85063_REG_CTRL2, ctrl2);
    
    const char* freqs[] = {"32768Hz", "1024Hz", "32Hz", "1Hz"};
    Serial.printf("✓ Clock output set to %s\n", freqs[frequency]);
}
```

## Troubleshooting

### Common Issues

1. **RTC Not Responding**
   - Check I2C connections (pins 10, 11)
   - Verify I2C address (0x51)
   - Check power supply to RTC
   - Test I2C bus with other devices

2. **Time Not Keeping**
   - Check crystal oscillator (32.768kHz)
   - Verify backup battery connection
   - Check for electromagnetic interference
   - Test crystal loading capacitors

3. **Incorrect Time**
   - Verify BCD conversion functions
   - Check register read/write operations
   - Validate date/time range limits
   - Test with known good time source

4. **Alarms Not Working**
   - Check alarm register settings
   - Verify interrupt enable bits
   - Test alarm flag clearing
   - Check interrupt pin connection (if used)

### Debug Functions

```cpp
void rtc_debug_info() {
    Serial.println("=== RTC Debug Information ===");
    
    // Read all control registers
    uint8_t ctrl1 = read_rtc_register(PCF85063_REG_CTRL1);
    uint8_t ctrl2 = read_rtc_register(PCF85063_REG_CTRL2);
    uint8_t offset = read_rtc_register(PCF85063_REG_OFFSET);
    uint8_t ram = read_rtc_register(PCF85063_REG_RAM_BYTE);
    
    Serial.printf("Control Registers:\n");
    Serial.printf("  CTRL1: 0x%02X\n", ctrl1);
    Serial.printf("  CTRL2: 0x%02X\n", ctrl2);
    Serial.printf("  OFFSET: 0x%02X\n", offset);
    Serial.printf("  RAM: 0x%02X\n", ram);
    
    // Decode control bits
    Serial.printf("Status:\n");
    Serial.printf("  Oscillator: %s\n", (ctrl1 & 0x20) ? "STOPPED" : "Running");
    Serial.printf("  24H Mode: %s\n", (ctrl1 & 0x02) ? "Disabled" : "Enabled");
    Serial.printf("  Alarm: %s\n", (ctrl2 & 0x80) ? "Enabled" : "Disabled");
    Serial.printf("  Alarm Flag: %s\n", (ctrl2 & 0x40) ? "SET" : "Clear");
    
    // Read current time
    DateTime dt;
    PCF85063_GetDateTime(&dt);
    Serial.printf("Current Time: %s\n", format_datetime(&dt).c_str());
    
    // Read alarm settings
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(PCF85063_REG_ALARM_SECONDS);
    Wire.endTransmission();
    
    Wire.requestFrom(PCF85063_ADDR, 5);
    if (Wire.available() >= 5) {
        Serial.printf("Alarm Settings:\n");
        uint8_t alarm_regs[5];
        for (int i = 0; i < 5; i++) {
            alarm_regs[i] = Wire.read();
        }
        
        Serial.printf("  Seconds: %s\n", 
                     (alarm_regs[0] & 0x80) ? "Disabled" : String(bcd_to_decimal(alarm_regs[0] & 0x7F)).c_str());
        Serial.printf("  Minutes: %s\n", 
                     (alarm_regs[1] & 0x80) ? "Disabled" : String(bcd_to_decimal(alarm_regs[1] & 0x7F)).c_str());
        Serial.printf("  Hours: %s\n", 
                     (alarm_regs[2] & 0x80) ? "Disabled" : String(bcd_to_decimal(alarm_regs[2] & 0x3F)).c_str());
        Serial.printf("  Days: %s\n", 
                     (alarm_regs[3] & 0x80) ? "Disabled" : String(bcd_to_decimal(alarm_regs[3] & 0x3F)).c_str());
        Serial.printf("  Weekdays: %s\n", 
                     (alarm_regs[4] & 0x80) ? "Disabled" : String(bcd_to_decimal(alarm_regs[4] & 0x07)).c_str());
    }
}

void rtc_accuracy_test() {
    Serial.println("=== RTC Accuracy Test ===");
    Serial.println("This test will run for 60 seconds...");
    
    DateTime startTime, endTime;
    PCF85063_GetDateTime(&startTime);
    
    unsigned long startMillis = millis();
    unsigned long endMillis = startMillis + 60000;  // 60 seconds
    
    Serial.printf("Start: %s (millis: %lu)\n", 
                 format_datetime(&startTime).c_str(), startMillis);
    
    // Wait for 60 seconds
    while (millis() < endMillis) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();
    
    PCF85063_GetDateTime(&endTime);
    unsigned long actualMillis = millis();
    
    Serial.printf("End: %s (millis: %lu)\n", 
                 format_datetime(&endTime).c_str(), actualMillis);
    
    // Calculate differences
    long rtc_diff = (endTime.second - startTime.second) + 
                   (endTime.minute - startTime.minute) * 60 +
                   (endTime.hour - startTime.hour) * 3600;
    
    long millis_diff = (actualMillis - startMillis) / 1000;
    long error = rtc_diff - millis_diff;
    
    Serial.printf("RTC elapsed: %ld seconds\n", rtc_diff);
    Serial.printf("System elapsed: %ld seconds\n", millis_diff);
    Serial.printf("Error: %ld seconds\n", error);
    
    if (abs(error) <= 1) {
        Serial.println("✓ RTC accuracy is good");
    } else {
        Serial.println("⚠️ RTC may need calibration");
    }
}
```

## Example Implementation

### Digital Clock

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize I2C and RTC
    I2C_Init();
    PCF85063_Init();
    
    // Try to sync with NTP (if WiFi available)
    if (WiFi.status() == WL_CONNECTED) {
        sync_time_from_ntp();
    }
    
    Serial.println("Digital Clock Ready");
}

void loop() {
    // Update RTC
    PCF85063_Loop();
    
    // Display time every second
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay >= 1000) {
        lastDisplay = millis();
        
        Serial.printf("\r%s", format_datetime(&currentTime).c_str());
    }
    
    delay(100);
}
```

*Last Updated: October 2, 2025*