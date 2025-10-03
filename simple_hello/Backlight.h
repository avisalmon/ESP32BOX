#pragma once

#define LCD_Backlight_PIN   5
#define PWM_Frequency       20000   
#define PWM_Resolution      10       

void Backlight_Init();
void Set_Backlight(uint8_t brightness_percent);