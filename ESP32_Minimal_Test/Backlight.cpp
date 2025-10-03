#include "Backlight.h"

void Backlight_Init() {
    // Use new ESP32 Arduino core API
    ledcAttach(LCD_Backlight_PIN, Frequency, Resolution);
    ledcWrite(LCD_Backlight_PIN, Dutyfactor);  // 50% brightness
}

void Set_Backlight(uint8_t Light) {
    if (Light > 100) Light = 100;
    uint16_t duty = map(Light, 0, 100, 0, 1023);
    ledcWrite(LCD_Backlight_PIN, duty);
}