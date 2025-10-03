#include "TCA9554PWR.h"

void TCA9554PWR_Init(uint8_t config) {
    Wire.beginTransmission(TCA9554_ADDRESS);
    Wire.write(TCA9554_CONFIG_REG);
    Wire.write(config);  // 0x00 = all outputs
    Wire.endTransmission();
}

void Set_EXIO(uint8_t Pin, uint8_t State) {
    Wire.beginTransmission(TCA9554_ADDRESS);
    Wire.write(TCA9554_OUTPUT_REG);
    
    if (Pin == EXIO_PIN2) {  // Display reset pin
        Wire.write(State == High ? 0x04 : 0x00);  // Bit 2
    }
    
    Wire.endTransmission();
}