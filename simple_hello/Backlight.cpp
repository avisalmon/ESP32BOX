#include "Backlight.h"

void Backlight_Init() {
    // Use new ESP32 Arduino core API
    ledcAttach(LCD_Backlight_PIN, PWM_Frequency, PWM_Resolution);
    ledcWrite(LCD_Backlight_PIN, 512);  // 50% brightness
}

void Set_Backlight(uint8_t brightness_percent) {
    if (brightness_percent > 100) brightness_percent = 100;
    uint16_t duty = map(brightness_percent, 0, 100, 0, 1023);
    ledcWrite(LCD_Backlight_PIN, duty);
}