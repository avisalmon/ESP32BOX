/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

// PHASE 1: Core hardware only - progressively enable features
#include "Display_ST77916.h"
#include "I2C_Driver.h"         
#include "TCA9554PWR.h"         

// PHASE 2: LVGL (enable after Phase 1 works)
// #include "LVGL_Driver.h"
// #include "LVGL_Example.h"

// PHASE 3: Additional peripherals (enable one by one)
// #include "Audio_PCM5101.h"
// #include "RTC_PCF85063.h"
// #include "MIC_MSM.h"
// #include "SD_Card.h"
// #include "BAT_Driver.h"
// #include "Wireless.h"

// PHASE 1: Absolute minimum - just compile and boot
void setup()
{
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("ESP32-S3 DEMO - Phase 1: Minimal Boot Test");
  Serial.println("==========================================");
  
  Serial.println("✅ Phase 1: Arduino boots and Serial works!");
  Serial.println("� Next: Add I2C, then display, then LVGL...");
}

void loop() {
  // Phase 1: Just blink the built-in LED to prove it's alive
  static unsigned long lastPrint = 0;
  static int counter = 0;
  
  if (millis() - lastPrint >= 3000) {
    counter++;
    Serial.printf("💓 Heartbeat #%d - %lu seconds uptime\n", 
                  counter, millis() / 1000);
    lastPrint = millis();
  }
  
  delay(500);
}

// PHASE 2 & 3: Uncomment these when Phase 1 works
/*
void Driver_Loop(void *parameter)
{
  // Wireless_Test2();  // Phase 3
  while(1)
  {
    // PCF85063_Loop();  // Phase 3
    // BAT_Get_Volts();  // Phase 3
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void Advanced_Setup() {
  // Phase 2: LVGL
  // Lvgl_Init();
  // Lvgl_Example1();
  
  // Phase 3: Peripherals  
  // SD_Init();
  // Audio_Init();
  // MIC_Init();
  // BAT_Init();
  // PCF85063_Init();
  
  // Phase 3: Background task
  // xTaskCreatePinnedToCore(Driver_Loop, "DriverTask", 4096, NULL, 3, NULL, 0);
}
*/
