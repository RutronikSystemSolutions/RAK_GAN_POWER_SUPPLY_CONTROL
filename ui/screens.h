#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_PROPERTIES = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *properties;
    lv_obj_t *u_out;
    lv_obj_t *i_out;
    lv_obj_t *power_button;
    lv_obj_t *power_button_label;
    lv_obj_t *current_inc_button;
    lv_obj_t *current_inc_button_label;
    lv_obj_t *current_dec_button;
    lv_obj_t *current_dec_button_label;
    lv_obj_t *voltage_inc_button;
    lv_obj_t *voltage_inc_button_label;
    lv_obj_t *voltage_dec_button;
    lv_obj_t *voltage_dec_button_label;
    lv_obj_t *info_button;
    lv_obj_t *info_button_label;
    lv_obj_t *curr_info_label;
    lv_obj_t *volt_info_label;
    lv_obj_t *mode_label;
    lv_obj_t *info_return_button;
    lv_obj_t *info_return_button_label;
    lv_obj_t *info_temp_label;
    lv_obj_t *info_uin_label;
    lv_obj_t *info_i_in_label;
    lv_obj_t *info_pwr_label;
    lv_obj_t *info_eff_label;
    lv_obj_t *info_energy_label;
    lv_obj_t *info_rst_button;
    lv_obj_t *info_rst_button_label;
    lv_obj_t *info_temp_label_const;
    lv_obj_t *info_uin_label_const;
    lv_obj_t *info_i_in_label_const;
    lv_obj_t *info_pwrin_label_const;
    lv_obj_t *info_eff_label_const;
    lv_obj_t *info_energy_label_const;
    lv_obj_t *info_pwrout_label;
    lv_obj_t *info_pwrout_label_const;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_properties();
void tick_screen_properties();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/