#pragma once
#include <stdio.h>
#include "I2C_Driver.h"

#define TCA9554_ADDRESS         0x20
#define TCA9554_INPUT_REG       0x00
#define TCA9554_OUTPUT_REG      0x01
#define TCA9554_CONFIG_REG      0x03

#define Low   0
#define High  1
#define EXIO_PIN1   1
#define EXIO_PIN2   2

void TCA9554PWR_Init(uint8_t config);
void Set_EXIO(uint8_t Pin, uint8_t State);