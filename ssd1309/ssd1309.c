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

#include "ssd1309.h"

static uint8_t SSD1309_Buffer[SSD1309_BUFFER_SIZE];
static SSD1309_t SSD1309;

/* Rutronik System Solutions logo */
const unsigned char logo[]  =
{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xc0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xc0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x30, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x1f, 0xc0, 0x30, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x38, 0x60, 0x11, 0xe0, 0x00, 0x00,
		0x00, 0x00, 0x60, 0x30, 0x7f, 0x30, 0xc0, 0x00, 0x00, 0x00, 0xc7, 0x98, 0xf3, 0x31, 0xe0, 0x00,
		0x00, 0x00, 0xcf, 0xc8, 0x11, 0xe3, 0x30, 0x00, 0x00, 0x00, 0xd8, 0x4c, 0x10, 0xc3, 0x30, 0x00,
		0x00, 0x07, 0xd8, 0x08, 0x10, 0x01, 0xe0, 0x00, 0x00, 0x1f, 0x98, 0x00, 0x10, 0x00, 0xc0, 0x00,
		0x00, 0x70, 0x08, 0x00, 0x10, 0x07, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x38, 0x1c, 0x00, 0x00,
		0x00, 0xc1, 0x80, 0x0f, 0xff, 0xf8, 0x06, 0x00, 0x00, 0xc1, 0xc0, 0x00, 0x18, 0x00, 0x0f, 0x00,
		0x00, 0x80, 0xc0, 0x00, 0x10, 0x00, 0x19, 0x80, 0x00, 0x80, 0x60, 0x00, 0x10, 0x00, 0x19, 0x80,
		0x00, 0xc8, 0x60, 0x00, 0x18, 0x00, 0x0f, 0x00, 0x00, 0xf8, 0x20, 0x40, 0x1f, 0xfe, 0x06, 0x00,
		0x01, 0xe0, 0x60, 0xc7, 0x18, 0x03, 0x00, 0x00, 0x03, 0x80, 0x60, 0xc1, 0x90, 0x01, 0x80, 0x00,
		0x03, 0x00, 0x61, 0x81, 0x90, 0x00, 0xc0, 0x00, 0x06, 0x00, 0xc3, 0x81, 0x90, 0xc0, 0x60, 0x00,
		0x0c, 0x01, 0x8f, 0x01, 0x91, 0xe0, 0x00, 0x00, 0x0c, 0x00, 0x1c, 0x07, 0x1f, 0x30, 0x03, 0x00,
		0x08, 0x60, 0x00, 0x06, 0x1f, 0x30, 0x07, 0x80, 0x08, 0x60, 0x00, 0x00, 0x11, 0xe0, 0x0c, 0xc0,
		0x08, 0x30, 0x00, 0x00, 0x10, 0xc0, 0x0c, 0xc0, 0x08, 0x1c, 0x00, 0x00, 0x30, 0x00, 0x07, 0x80,
		0x0c, 0x0f, 0x87, 0xff, 0xf0, 0x01, 0x83, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x10, 0x03, 0xc0, 0x00,
		0x06, 0x00, 0x00, 0x00, 0x18, 0x06, 0x60, 0x00, 0x07, 0x30, 0x00, 0x00, 0x1f, 0xfe, 0x60, 0x00,
		0x03, 0xf0, 0x00, 0x00, 0x18, 0x03, 0xc0, 0x00, 0x01, 0xc0, 0x7e, 0x0e, 0x10, 0x01, 0x80, 0xc0,
		0x00, 0xc0, 0x3f, 0x1f, 0x10, 0x00, 0x01, 0xe0, 0x00, 0x80, 0x00, 0x00, 0x10, 0x00, 0x03, 0x30,
		0x01, 0x80, 0x00, 0x00, 0x10, 0x06, 0x03, 0x30, 0x01, 0x80, 0x04, 0x00, 0x10, 0x0f, 0x01, 0xe0,
		0x00, 0x80, 0x07, 0x00, 0x10, 0x19, 0x80, 0xc0, 0x00, 0xc0, 0x01, 0x80, 0x1f, 0xf9, 0x80, 0x00,
		0x00, 0x40, 0x31, 0x80, 0x30, 0x0f, 0x00, 0x00, 0x00, 0x4c, 0x21, 0x80, 0xf0, 0x06, 0x00, 0x00,
		0x00, 0x2e, 0xe1, 0x80, 0x10, 0x00, 0x0c, 0x00, 0x00, 0x1f, 0xc3, 0x00, 0x10, 0x00, 0x1e, 0x00,
		0x00, 0x00, 0xc2, 0x00, 0x18, 0x00, 0x33, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x1f, 0xe0, 0x33, 0x00,
		0x00, 0x00, 0x60, 0x00, 0x18, 0x38, 0x1e, 0x00, 0x00, 0x00, 0x60, 0x00, 0x10, 0x06, 0x0c, 0x00,
		0x00, 0x00, 0x38, 0x70, 0x10, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x10, 0x19, 0x80, 0x00,
		0x00, 0x00, 0x03, 0x80, 0x30, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x60, 0x0f, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xe0, 0xe0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//--------------------------------------------------------------------------------------------------
// ssd1309_reset
//
// This function writes a command byte to the display controller
//--------------------------------------------------------------------------------------------------
void ssd1309_reset(void)
{
	Cy_GPIO_Clr(ARD_IO0_PORT, ARD_IO0_NUM);
	CyDelay(1);
	Cy_GPIO_Set(ARD_IO0_PORT, ARD_IO0_NUM);
	CyDelay(1);
}

//--------------------------------------------------------------------------------------------------
// ssd1309_write_command_byte
//
// This function writes a command byte to the display controller
//--------------------------------------------------------------------------------------------------
void ssd1309_write_command_byte(uint8_t c)
{
    Cy_GPIO_Clr(ARDU_CS_PORT, ARDU_CS_NUM);
	Cy_GPIO_Clr(ARDU_MISO_PORT, ARDU_MISO_NUM);
	CyDelay(1);

	//cyhal_spi_transfer(&ssd1309_spi, &c, 1, NULL, 0, 0xFF);
	spi_master_write(&ARD_SPI_context, (void*)&c, 1);

	Cy_GPIO_Set(ARDU_CS_PORT, ARDU_CS_NUM);
}


//--------------------------------------------------------------------------------------------------
// ssd1309_write_data_byte
//
// This function writes a data byte to the display controller
//--------------------------------------------------------------------------------------------------
void ssd1309_write_data_byte(uint8_t c)
{
    Cy_GPIO_Clr(ARDU_CS_PORT, ARDU_CS_NUM);
	Cy_GPIO_Set(ARDU_MISO_PORT, ARDU_MISO_NUM);
	//CyDelay(1);

	//cyhal_spi_transfer(&ssd1309_spi, &c, 1, NULL, 0, 0xFF);
	spi_master_write(&ARD_SPI_context, (void*)&c, 1);

	Cy_GPIO_Set(ARDU_CS_PORT, ARDU_CS_NUM);
}


//--------------------------------------------------------------------------------------------------
// ssd1309_write_data_stream
//
// This function writes multiple data bytes to the display controller
//--------------------------------------------------------------------------------------------------
void ssd1309_write_data_stream(uint8_t* pData, int numBytes)
{
    Cy_GPIO_Clr(ARDU_CS_PORT, ARDU_CS_NUM);
	Cy_GPIO_Set(ARDU_MISO_PORT, ARDU_MISO_NUM);
	//CyDelay(1);

	//while(cyhal_spi_is_busy( &ssd1309_spi))
	//{
	//
	//}
	//cyhal_spi_transfer(&ssd1309_spi, pData, numBytes, NULL, 0, 0xFF);

	spi_master_write(&ARD_SPI_context, (void*)pData, numBytes);

	Cy_GPIO_Set(ARDU_CS_PORT, ARDU_CS_NUM);
}

/* Fill the whole screen with the given color */
void ssd1309_Fill(SSD1309_COLOR color)
{
    /* Set memory */
    uint32_t i;

    for(i = 0; i < sizeof(SSD1309_Buffer); i++)
    {
        SSD1309_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

/* Write the screenbuffer with changed to the screen */
void ssd1309_UpdateScreen(void)
{
    /* Write data to each page of RAM. Number of pages
     * depends on the screen height:
     *
     *  * 32px   ==  4 pages
     *  * 64px   ==  8 pages
     *  * 128px  ==  16 pages
     */
    for (uint8_t i = 0; i < (SSD1309_HEIGHT / 8); i++)
    {
    	ssd1309_write_command_byte(0xB0 + i);
    	ssd1309_write_command_byte(0x00 + SSD1309_X_OFFSET_LOWER);
    	ssd1309_write_command_byte(0x10 + SSD1309_X_OFFSET_UPPER);
    	ssd1309_write_data_stream(&SSD1309_Buffer[SSD1309_WIDTH * i], SSD1309_WIDTH);
    }
}

/* Position the cursor */
void ssd1309_SetCursor(uint8_t x, uint8_t y)
{
    SSD1309.CurrentX = x - SSD1309_OFFSET_X;
    SSD1309.CurrentY = y - SSD1309_OFFSET_Y;
}

/*    Draw one pixel in the screenbuffer  */
/*    X => X Coordinate			  */
/*    Y => Y Coordinate			  */
/*    color => Pixel color		  */
void ssd1309_DrawPixel(uint8_t x, uint8_t y, SSD1309_COLOR color)
{
    if ((x >= SSD1309_WIDTH) || (y >= SSD1309_HEIGHT))
    {
        /* Don't write outside the buffer */
        return;
    }

    /* Draw in the right color */
    if (color == White)
    {
	    SSD1309_Buffer[x + (y / 8) * SSD1309_WIDTH] |= 1 << (y % 8);
    }
    else
    {
	    SSD1309_Buffer[x + (y / 8) * SSD1309_WIDTH] &= ~(1 << (y % 8));
    }
}

/* Draw 1 char to the screen buffer	      */
/* ch         => char om weg te schrijven     */
/* Font     => Font waarmee we gaan schrijven */
/* color     => Black or White                */
char ssd1309_WriteChar(char ch, FontDef Font, SSD1309_COLOR color)
{
    uint32_t i, b, j;

    /* Check if character is valid */
    if (ch < 32 || ch > 126)
    {
        return 0;
    }

    /* Check remaining space on current line */
    if ((SSD1309_WIDTH <= (SSD1309.CurrentX + Font.FontWidth))  ||
        (SSD1309_HEIGHT <= (SSD1309.CurrentY + Font.FontHeight))
       )
    {
        /* Not enough space on current line */
        return 0;
    }

    /* Use the font to write */
    for (i = 0; i < Font.FontHeight; i++)
    {
	b = Font.data[(ch - 32) * Font.FontHeight + i];

        for (j = 0; j < Font.FontWidth; j++)
	{
            if ((b << j) & 0x8000)
	    {
                ssd1309_DrawPixel(SSD1309.CurrentX + j, (SSD1309.CurrentY + i), (SSD1309_COLOR)color);
            }
	    else
	    {
                ssd1309_DrawPixel(SSD1309.CurrentX + j, (SSD1309.CurrentY + i), (SSD1309_COLOR)!color);
            }
        }
    }

    /* The current space is now taken */
    SSD1309.CurrentX += Font.FontWidth;

    /* Return written char for validation */
    return ch;
}


void ssd1309_WriteSymbol(SymbolID_t Symbol, uint8_t x, uint8_t y)
{
    uint32_t i, b, j;
    SSD1309_COLOR color = White;

    ssd1309_SetCursor(x, y);

    /* Check remaining space on current line */
    if ((SSD1309_WIDTH <= (SSD1309.CurrentX + SSD1309_Symbol[Symbol].SymbolWidth))  ||
        (SSD1309_HEIGHT <= (SSD1309.CurrentY + SSD1309_Symbol[Symbol].SymbolHeight))
       )
    {
        /* Not enough space on current line */
        return ;
    }

    /* Use the data to write */
    for (i = 0; i <= SSD1309_Symbol[Symbol].SymbolHeight; i++)
    {
	b = SSD1309_Symbol[Symbol].data[i];

        for (j = 0; j < SSD1309_Symbol[Symbol].SymbolWidth; j++)
	{
            if ((b << j) & 0x8000)
	    {
                ssd1309_DrawPixel(SSD1309.CurrentX + j, (SSD1309.CurrentY + i), (SSD1309_COLOR)color);
            }
	    else
	    {
                ssd1309_DrawPixel(SSD1309.CurrentX + j, (SSD1309.CurrentY + i), (SSD1309_COLOR)!color);
            }
        }
    }

    /* The current space is now taken */
    SSD1309.CurrentX += SSD1309_Symbol[Symbol].SymbolWidth;
}


/* Write full string to screenbuffer */
char ssd1309_WriteString(char* str, FontDef Font, SSD1309_COLOR color)
{
    /* Write until null-byte */
    while (*str)
    {
        if (ssd1309_WriteChar(*str, Font, color) != *str)
        {
            /* Char could not be written */
            return *str;
        }

        /* Next char */
        str++;
    }

    /* Everything ok */
    return *str;
}

/* Draw bitmap - ported from the ADAFruit GFX library */
void ssd1309_DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1309_COLOR color)
{
    uint8_t byte = 0;
    int16_t byteWidth = (w + 7) / 8; /* Bitmap scanline pad = whole byte */

    if (x >= SSD1309_WIDTH || y >= SSD1309_HEIGHT)
    {
        return;
    }

    for (uint8_t j = 0; j < h; j++, y++)
    {
        for (uint8_t i = 0; i < w; i++)
        {
            if (i & 7)
            {
                byte <<= 1;
            }
            else
            {
                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }

            if (byte & 0x80)
            {
                ssd1309_DrawPixel(x + i, y, color);
            }
        }
    }

    return;
}

//--------------------------------------------------------------------------------------------------
// ssd1309_init_spi
//
// Initialise the OLED display.
//--------------------------------------------------------------------------------------------------
cy_rslt_t ssd1309_init_spi(void)
{
	cy_rslt_t result = CY_RSLT_SUCCESS;

	/*SPI Initialisation*/

/*
	result = cyhal_spi_init(
			&ssd1309_spi,
			ARDU_MOSI,
			NC,
			ARDU_CLK,
			NC,
			NULL,
			8,
			CYHAL_SPI_MODE_00_MSB,
			false);
    if(result != CY_RSLT_SUCCESS)
    {
  	  return result;
    }

    result = cyhal_spi_set_frequency(&ssd1309_spi, 8000000);
    if(result != CY_RSLT_SUCCESS)
    {
    	return result;
    }
*/
    /*Initialise the D/C# pin*/
/*
    result = cyhal_gpio_init( ARDU_MISO, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    if (result != CY_RSLT_SUCCESS)
    {
  	  return result;
    }
*/
    /*Initialise the CS# pin*/
/*
    result = cyhal_gpio_init( ARDU_CS, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);
    if (result != CY_RSLT_SUCCESS)
    {
  	  return result;
    }
*/
    /*Initialise the RES# pin*/
/*
    result = cyhal_gpio_init( ARDU_IO1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);
    if (result != CY_RSLT_SUCCESS)
    {
  	  return result;
    }
*/
    /*Reset the display*/
    ssd1309_reset();

    /* Initialise OLED Display*/
    ssd1309_write_command_byte(0xAE); /* Display off */
    ssd1309_write_command_byte(0x20); /* Set Memory Addressing Mode */
    ssd1309_write_command_byte(0x10); /* 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode; */
    ssd1309_write_command_byte(0xB0); /*Set Page Start Address for Page Addressing Mode, 0-7 */
#ifdef SSD1309_MIRROR_VERT
    ssd1309_write_command_byte(0xC0); /* Mirror vertically */
#else
    ssd1309_write_command_byte(0xC8); /* Set COM Output Scan Direction */
#endif
    ssd1309_write_command_byte(0x00); /*---set low column address  */
    ssd1309_write_command_byte(0x10); /*---set high column address */
    ssd1309_write_command_byte(0x40); /*--set start line address - CHECK */
    ssd1309_write_command_byte(0x81); /*--set contrast control register - CHECK */
    ssd1309_write_command_byte(0xFF);
#ifdef SSD1309_MIRROR_HORIZ
    ssd1309_write_command_byte(0xA0); /* Mirror horizontally */
#else
    ssd1309_write_command_byte(0xA1); /* --set segment re-map 0 to 127 - CHECK */
#endif

#ifdef SSD1309_INVERSE_COLOR
    ssd1309_write_command_byte(0xA7); /*--set inverse color */
#else
    ssd1309_write_command_byte(0xA6); /*--set normal color */
#endif
    /* Set multiplex ratio. */
#if (SSD1309_HEIGHT == 128)
    /* Found in the Luma Python lib for SH1106. */
    ssd1309_write_command_byte(0xFF);
#else
    ssd1309_write_command_byte(0xA8); /*--set multiplex ratio(1 to 64) - CHECK */
#endif

#if (SSD1309_HEIGHT == 32)
    ssd1309_write_command_byte(0x1F);
#elif (SSD1309_HEIGHT == 64)
    ssd1309_write_command_byte(0x3F);
#elif (SSD1309_HEIGHT == 128)
    ssd1309_write_command_byte(0x3F); /* Seems to work for 128px high displays too. */
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
    ssd1309_write_command_byte(0xA4); /* 0xA4, Output follows RAM content;0xa5,Output ignores RAM content */
    ssd1309_write_command_byte(0xD3); /*-set display offset - CHECK */
    ssd1309_write_command_byte(0x00); /*-not offset */
    ssd1309_write_command_byte(0xD5); /*--set display clock divide ratio/oscillator frequency */
    ssd1309_write_command_byte(0xF0); /*--set divide ratio */
    ssd1309_write_command_byte(0xD9); /*--set pre-charge period */
    ssd1309_write_command_byte(0x22); /*			  */
    ssd1309_write_command_byte(0xDA); /*--set com pins hardware configuration - CHECK */
#if (SSD1309_HEIGHT == 32)
    ssd1309_write_command_byte(0x02);
#elif (SSD1309_HEIGHT == 64)
    ssd1309_write_command_byte(0x12);
#elif (SSD1309_HEIGHT == 128)
    ssd1309_write_command_byte(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
    ssd1309_write_command_byte(0xDB); /*--set vcomh */
    ssd1309_write_command_byte(0x20); /* 0x20, 0.77xVcc */
    ssd1309_write_command_byte(0x8D); /*--set DC-DC enable */
    ssd1309_write_command_byte(0x14); /*                   */
    ssd1309_write_command_byte(0xAF); /*--turn on SSD1309 panel */

    /* Clear screen */
    ssd1309_Fill(Black);
/*

    ssd1309_SetCursor(62, 12);
    char string[64] = {"RUTRONIK"};
    ssd1309_WriteString(string, Font_7x10, White);

    memset(string, 0x00, sizeof(string));
    ssd1309_SetCursor(62, 28);
    strcpy(string, "SYSTEM");
    ssd1309_WriteString(string, Font_7x10, White);

    memset(string, 0x00, sizeof(string));
    ssd1309_SetCursor(62, 44);
    strcpy(string, "SOLUTIONS");
    ssd1309_WriteString(string, Font_7x10, White);

    ssd1309_DrawBitmap(0, 0, logo, 64, 64, White);

    ssd1309_set_contrast(0);

    ssd1309_UpdateScreen();
    for(uint16_t cont = 0; cont <= 0xff; cont++)
    {
    	ssd1309_set_contrast(cont);
    	CyDelay(10);
    }
    CyDelay(1000);

    ssd1309_Fill(Black);
*/

    /* Flush buffer to screen */
    ssd1309_UpdateScreen();

    return result;
}

//--------------------------------------------------------------------------------------------------
// ssd1309_free
//
// Free the resources used with the OLED display.
//--------------------------------------------------------------------------------------------------

void ssd1309_set_contrast(uint8_t contrast)
{
    ssd1309_write_command_byte(0x81); /*--set contrast control register - CHECK */
    ssd1309_write_command_byte(contrast);
}

void ssd1309_free(void)
{
	return;
}
