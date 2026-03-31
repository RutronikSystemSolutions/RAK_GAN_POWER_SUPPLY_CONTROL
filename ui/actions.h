#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_inc_volt_by_step(lv_event_t * e);
extern void action_dec_volt_by_step(lv_event_t * e);
extern void action_inc_curr_by_step(lv_event_t * e);
extern void action_dec_curr_by_step(lv_event_t * e);
extern void action_inv_pwr_ctrl(lv_event_t * e);
extern void action_switch_screen_to_prop(lv_event_t * e);
extern void action_switch_volt_label(lv_event_t * e);
extern void action_switch_curr_label(lv_event_t * e);
extern void action_switch_screen_to_main(lv_event_t * e);
extern void action_reset_energy_cnt(lv_event_t * e);
extern void action_inc_volt_cont(lv_event_t * e);
extern void action_dec_volt_cont(lv_event_t * e);
extern void action_inc_curr_cont(lv_event_t * e);
extern void action_dec_curr_cont(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/