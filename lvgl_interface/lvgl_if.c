/*
 * lvgl_if.c
 *
 *  Created on: 2024-08-12
 *      Author: GDR
 */

#include "lvgl.h"
#include "lvgl_if.h"
#include "ssd1309.h"
#include "GT911.h"

#define BIT_SET(a,b) ((a) |= (1U<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1U<<(b)))
#define DISP_BUF_SIZE (128*64/8)

static lv_disp_drv_t disp_drv;
static lv_color_t disp_buf1[DISP_BUF_SIZE];
static lv_color_t disp_buf2[DISP_BUF_SIZE];
lv_font_t oled;
lv_disp_t disp;

lv_indev_drv_t indev_drv;
lv_indev_t * indev_touchpad;
TouchCordinate_t cordinate[5];
uint8_t number;

void set_px_cb(struct _lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
void rounder_cb(struct _lv_disp_drv_t * disp_drv, lv_area_t * area);
static void flush_cb(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);
void lv_port_disp_init();
void lv_port_indev_init();


/*******************************************************************************
* Function Name: void lv_port_disp_init(void)
********************************************************************************
*
* Summary: This function is responsible for initializing the display driver.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void lv_port_disp_init()
{

    static lv_disp_draw_buf_t buf;
    memset(disp_buf1, 0, sizeof(disp_buf1));
    memset(disp_buf2, 0, sizeof(disp_buf2));

    lv_disp_draw_buf_init(&buf, disp_buf1, disp_buf2, DISP_BUF_SIZE);

    lv_disp_drv_init(&disp_drv);

    disp_drv.draw_buf = &buf;
    disp_drv.flush_cb = flush_cb;
    disp_drv.set_px_cb = set_px_cb;
    disp_drv.rounder_cb = rounder_cb;

    disp_drv.hor_res = SSD1309_WIDTH;
    disp_drv.ver_res = SSD1309_HEIGHT;
    disp_drv.antialiasing = 0;

    lv_disp_drv_register(&disp_drv);

	lv_theme_t* theme = lv_theme_mono_init(&disp, false, &oled);
	lv_disp_set_theme(&disp, theme);
}

/*******************************************************************************
* Function Name: flush_cb
********************************************************************************
*
* Summary: This function is responsible for flushing the color buffer in LVGL.
*
* Parameters:
*    x1      left coordinate of the rectangle
*    x2      right coordinate of the rectangle
*    y1      top coordinate of the rectangle
*    y2      bottom coordinate of the rectangle
*    x2      right coordinate of the rectangle
*    color_p pointer to an array of colors
*
* Return:
*  None
*
*
*******************************************************************************/

static void flush_cb(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint8_t row1 = area->y1>>3;
    uint8_t row2 = area->y2>>3;
    uint8_t *buf = (uint8_t*) color_p;

    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if(area->x1 > SSD1309_WIDTH - 1) return;
    if(area->y1 > SSD1309_HEIGHT - 1) return;

    for(uint8_t row = row1; row <= row2; row++)
    {
      ssd1309_write_command_byte(0xB0 | row);
      ssd1309_write_command_byte(0x00 | (area->x1 & 0xF));
      ssd1309_write_command_byte(0x10 | ((area->x1>>4) & 0xF) );

      for(uint16_t x = area->x1; x <= area->x2; x++)
      {
        ssd1309_write_data_byte(*buf);
        buf++;
      }
    }

    lv_disp_flush_ready(drv);
}

void set_px_cb(struct _lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
{
  uint16_t byte_index = x + (( y>>3 ) * buf_w);
  uint8_t  bit_index  = y & 0x7;
  // == 0 inverts, so we get white on black
  if ( color.full == 0 )
  {
    BIT_SET( buf[ byte_index ] , bit_index );
  }
  else
  {
    BIT_CLEAR( buf[ byte_index ] , bit_index );
  }
}

void rounder_cb(struct _lv_disp_drv_t * disp_drv, lv_area_t * area)
{
  area->y1 = (area->y1 & (~0x7));
  area->y2 = (area->y2 & (~0x7)) + 7;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(int16_t * x, int16_t * y)
{
    /*Your code comes here*/

    (*x) = cordinate[0].x;
    (*y) = cordinate[0].y;
}

/*
static _Bool touchpad_is_pressed(void)
{
	static uint16_t last_x = 0;
	static uint16_t last_y = 0;

	if((last_x != cordinate[0].x) || (last_y != cordinate[0].y))
	{
		last_x = cordinate[0].x;
		last_y = cordinate[0].y;
		return true;
	}

	return false;
}
*/

#define TOUCHPANEL_X_SCALE 		(1.1963f)
#define TOUCHPANEL_Y_SCALE 		(1.3617f)
#define TOUCHPANEL_X_OFFSET 	(2U)
#define TOUCHPANEL_Y_OFFSET 	(4U)
void my_touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	float x_scale;
	float y_scale;
	/*Read the touch pad*/
	GT911_ReadTouch(cordinate, &number);

    if(number > 0)
    {
		/*Fix the coordinates from touch panel*/
		if(cordinate[0].x <32)
		{cordinate[0].x = cordinate[0].x * 2;}
		cordinate[0].x = cordinate[0].x-TOUCHPANEL_X_OFFSET;
		if(cordinate[0].x < 0){cordinate[0].x = 0;}
		cordinate[0].y = cordinate[0].y-TOUCHPANEL_Y_OFFSET;
		if(cordinate[0].y < 0){cordinate[0].y = 0;}
		x_scale = 	(float)cordinate[0].x * TOUCHPANEL_X_SCALE;
		y_scale = 	(float)cordinate[0].y * TOUCHPANEL_Y_SCALE;
	  	cordinate[0].x = (int16_t)x_scale;
	  	if(cordinate[0].x > 128){cordinate[0].x = 128;}
	  	cordinate[0].y = (int16_t)y_scale;
	  	if(cordinate[0].y > 64){cordinate[0].y = 64;}
	  	
      	data->state = LV_INDEV_STATE_PRESSED;
      	touchpad_get_xy(&data->point.x, &data->point.y);
    }
    else
    {
      data->state = LV_INDEV_STATE_RELEASED;
    }
}

void lv_port_indev_init()
{
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_touchpad_read;
	lv_indev_drv_register(&indev_drv);
}
