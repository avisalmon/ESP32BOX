// User_Setup.h for ESP32-S3-Touch-LCD-1.85C
// ST77916 Controller Configuration for TFT_eSPI

// Define the driver
#define ST77XX_DRIVER      // Generic ST77XX driver for ST77916

// Define display size
#define TFT_WIDTH  360
#define TFT_HEIGHT 360

// ESP32-S3 Pin definitions for ST77916 QSPI interface
#define TFT_MOSI 46  // SPI Data 0 (MOSI) 
#define TFT_SCLK 40  // SPI Clock
#define TFT_CS   21  // Chip Select
#define TFT_DC   -1  // Data/Command (not used in QSPI mode)
#define TFT_RST  -1  // Reset (controlled by TCA9554PWR EXIO2)

// Additional QSPI data lines (TFT_eSPI may not fully utilize these)
#define TFT_D1 45    // SPI Data 1
#define TFT_D2 42    // SPI Data 2
#define TFT_D3 41    // SPI Data 3

// Font loading - enable commonly used fonts
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font
#define LOAD_FONT2  // Font 2. Small 16 pixel high font
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font
#define LOAD_FONT6  // Font 6. Large 48 pixel high font
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel high font
#define LOAD_FONT8  // Font 8. Large 75 pixel high font
#define LOAD_GFXFF  // FreeFonts

#define SMOOTH_FONT

// SPI frequency settings
#define SPI_FREQUENCY  40000000   // 40MHz for display
#define SPI_READ_FREQUENCY  20000000

// Color definitions (RGB565 format)
#define TFT_BLACK       0x0000
#define TFT_NAVY        0x000F
#define TFT_DARKGREEN   0x03E0
#define TFT_DARKCYAN    0x03EF
#define TFT_MAROON      0x7800
#define TFT_PURPLE      0x780F
#define TFT_OLIVE       0x7BE0
#define TFT_LIGHTGREY   0xC618
#define TFT_DARKGREY    0x7BEF
#define TFT_BLUE        0x001F
#define TFT_GREEN       0x07E0
#define TFT_CYAN        0x07FF
#define TFT_RED         0xF800
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_WHITE       0xFFFF
#define TFT_ORANGE      0xFDA0
#define TFT_GREENYELLOW 0xB7E0
#define TFT_PINK        0xFC9F