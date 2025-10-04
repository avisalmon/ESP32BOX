/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include "Display_ST77916.h"
#include "Audio_PCM5101.h"
#include "RTC_PCF85063.h"
#include "LVGL_Driver.h"
#include "MIC_MSM.h"
#include "SD_Card.h"
#include "LVGL_Example.h"
#include "BAT_Driver.h"
#include "WiFi_Manager.h"

WiFiManager wifiManager;
bool wifiConnected = false;

void Driver_Loop(void *parameter)
{
  Serial.println("\n\n*** Driver_Loop TASK IS RUNNING ***\n");
  // Wireless_Test2();  // Disabled - interferes with WiFi Manager
  
  // Try WiFi connection after initial startup
  Serial.println("Driver_Loop task started");
  Serial.println("Waiting 5 seconds for system to stabilize...");
  vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 seconds for any old tasks to complete
  Serial.println("Starting WiFi Manager...");
  wifiConnected = wifiManager.connectToBestNetwork();
  Serial.printf("\n=== WiFi connection result: %s ===\n", wifiConnected ? "SUCCESS" : "FAILED");
  
  while(1)
  {
    PCF85063_Loop();
    BAT_Get_Volts();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
void Driver_Init()
{
  Flash_test();
  BAT_Init();
  I2C_Init();
  TCA9554PWR_Init(0x00);   
  Backlight_Init();
  PCF85063_Init();
}
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== SETUP STARTING ===");
  
  Serial.println("Calling Driver_Init...");
  Driver_Init();
  Serial.println("Driver_Init complete");

  Serial.println("Calling SD_Init...");
  SD_Init();
  Serial.println("SD_Init complete");
  Serial.println("SD_Init complete");
  
  Serial.println("Calling Audio_Init...");
  Audio_Init();
  Serial.println("Audio_Init complete");
  
  // MIC_Init();  // Disabled - causes crash without SR model files
  
  Serial.println("Calling LCD_Init...");
  LCD_Init();
  Serial.println("LCD_Init complete");
  Serial.println("LCD_Init complete");
  
  Serial.println("Calling Lvgl_Init...");
  Lvgl_Init();
  Serial.println("Lvgl_Init complete");

  Serial.println("Calling Lvgl_Example1...");
  Lvgl_Example1();
  Serial.println("Lvgl_Example1 complete");
  
  // WiFi connection happens in Driver_Loop task to avoid blocking display
  // lv_demo_widgets();
  // lv_demo_benchmark();
  // lv_demo_keypad_encoder();
  // lv_demo_music();
  // lv_demo_printer();
  // lv_demo_stress();
  
  Serial.println("Creating Driver_Loop task...");
  BaseType_t taskResult = xTaskCreatePinnedToCore(
    Driver_Loop,           
    "DriverTask",         
    8192,                 // Increased stack size
    NULL,                 
    3,                    
    NULL,                 
    0                     
  );
  
  if (taskResult == pdPASS) {
    Serial.println("Driver_Loop task created successfully");
  } else {
    Serial.println("ERROR: Failed to create Driver_Loop task!");
  }
  
  Serial.println("=== SETUP COMPLETE ===");
}
int Time_Loop=0;
void loop() {
  Lvgl_Loop();
  vTaskDelay(pdMS_TO_TICKS(5));

}
