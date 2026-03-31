/********************************************************************************
 * \copyright
 * Copyright 2018-2022 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#include "ssd1309_conf.h"
#include "ssd1309_fonts.h"
#include "spi_master.h"

#if defined(__cplusplus)
extern "C"
{
#endif


/* SSD1309 OLED height in pixels  */
#ifndef SSD1309_HEIGHT
#define SSD1309_HEIGHT          64
#endif

/* SSD1309 width in pixels	  */
#ifndef SSD1309_WIDTH
#define SSD1309_WIDTH           128
#endif

/* SSD1309 offset of x in pixels  */
#ifndef SSD1309_OFFSET_X
#define SSD1309_OFFSET_X        0
#define SSD1309_X_OFFSET_LOWER (SSD1309_OFFSET_X & 0x0F)
#define SSD1309_X_OFFSET_UPPER ((SSD1309_OFFSET_X >> 4) & 0x07)
#else
#define SSD1309_X_OFFSET_LOWER 0
#define SSD1309_X_OFFSET_UPPER 0
#endif

/* SSD1309 offset of y in pixels  */
#ifndef SSD1309_OFFSET_Y
#define SSD1309_OFFSET_Y        0
#endif

#define SSD1309_BUFFER_SIZE     (SSD1309_WIDTH * SSD1309_HEIGHT / 8)

/* Enumeration for screen colors			  */
typedef enum
{
    Black = 0x00, /* Black color, no pixel		  */
    White = 0x01  /* Pixel is set. Color depends on OLED  */
} SSD1309_COLOR;

typedef enum
{
    ROTATION_0    = 0,
    ROTATION_90   = 1,
    ROTATION_180  = 2,
    ROTATION_270  = 3
} SSD1309_ROTATION;

/* Struct to store transformations			  */
typedef struct
{
    uint16_t CurrentX;
    uint16_t CurrentY;
    SSD1309_ROTATION Rotation;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SSD1309_t;

typedef struct
{
    uint8_t x;
    uint8_t y;
} SSD1309_VERTEX;

cy_rslt_t ssd1309_init_spi(void);
void ssd1309_write_command_byte(uint8_t c);
void ssd1309_write_data_byte(uint8_t c);
void ssd1309_write_data_stream(uint8_t* pData, int numBytes);
void ssd1309_free(void);
void ssd1309_Fill(SSD1309_COLOR color);
void ssd1309_UpdateScreen(void);
void ssd1309_SetCursor(uint8_t x, uint8_t y);
void ssd1309_DrawPixel(uint8_t x, uint8_t y, SSD1309_COLOR color);
char ssd1309_WriteChar(char ch, FontDef Font, SSD1309_COLOR color);
void ssd1309_WriteSymbol(SymbolID_t Symbol, uint8_t x, uint8_t y);
char ssd1309_WriteString(char* str, FontDef Font, SSD1309_COLOR color);
void ssd1309_DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1309_COLOR color);
void ssd1309_set_contrast(uint8_t contrast);

#if defined(__cplusplus)
}
#endif

/** \} group_board_libs */
