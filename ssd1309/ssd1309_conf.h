/**
 * Private configuration file for the SSD1309 library.
 * This example is configured for STM32F0, I2C and including all fonts.
 */

#ifndef __SSD1309_CONF_H__
#define __SSD1309_CONF_H__

// Mirror the screen if needed
// #define SSD1309_MIRROR_VERT
// #define SSD1309_MIRROR_HORIZ

// Set inverse color if needed
// # define SSD1309_INVERSE_COLOR

// Include only needed fonts
#define SSD1309_INCLUDE_FONT_6x8
#define SSD1309_INCLUDE_FONT_7x10
#define SSD1309_INCLUDE_FONT_11x18
#define SSD1309_INCLUDE_FONT_16x26

// The width of the screen can be set using this
// define. The default value is 128.
// #define SSD1309_WIDTH           64

// If your screen horizontal axis does not start
// in column 0 you can use this define to
// adjust the horizontal offset
// #define SSD1309_OFFSET_X

// If your screen vertical axis does not start
// in column 0 you can use this define to
// adjust the vertical offset
// #define SSD1309_OFFSET_Y

// The height can be changed as well if necessary.
// It can be 32, 64 or 128. The default value is 64.
// #define SSD1309_HEIGHT          64

#endif /* __SSD1309_CONF_H__ */
