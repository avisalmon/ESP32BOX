#pragma once

#define LCD_Backlight_PIN   5
#define PWM_Channel     1       
#define Frequency       20000   
#define Resolution      10       
#define Dutyfactor      500     

void Backlight_Init();
void Set_Backlight(uint8_t Light);