#include "control_task.h"
#include "water_flow.h"

int game_stage = 0;

int time_tick = 0;
int led_time1 = 1;
int all_finish_time[5] = {0, 0, 0, 0, 0};
int last_led_time1 = 24;

int time1 = 0;
int turn_flag = 0;
int energe_mode = 0;
int energe_leaf = 0;

double turn_a = 1.045, turn_w = 2, turn_b = 1.045;
double angle = 0;
long long time_Big_energe = 0;

int energe_leaf1_rand_flag = 0;
int energe_leaf2_rand_flag = 0;
int energe_leaf3_rand_flag = 0;
int energe_leaf4_rand_flag = 0;
int energe_leaf5_rand_flag = 0;
int energe_leaf_all_rand_flag = 0;
leaf LED[5] = {0, 0, 0, 0, 0};
int current_leaf = 0;
uint8_t n;
uint8_t m;
uint8_t flag_start = 0;
uint8_t count_leaf = 0;
uint8_t flag_all_finish = 0;
uint8_t finish_flag = 0;
uint8_t rand_finish = 0;

uint8_t target_mode[6] = {0, 0, 0, 0, 0, 0};
uint8_t led_mode_last[5] = {0};
uint8_t water_flow_flag = 0;
uint8_t water_flow_stepped = 0;

void LED_task(void) {
  if (time_tick % 50 == 0) {
    water_flow_stepped = 0;
    m = 1;
    led_display_set(m);
  }

  if (time_tick % 50 == 1) {
    m = 2;
    led_display_set(m);
  }
}

void led_display_set(uint8_t m) {
  led_mode_last[m] = LED[m].mode;

  switch (LED[m].mode) {
    case Waiting_hit:
      my_ws2812_set_all_off(m);
      water_flow_flag = 1;
      target_mode[m] = 1;
      break;
    case hit_finish:
      my_ws2812_set_all_off(m);
      LED_Strip(m);
      target_mode[m] = 0;
      break;
    case LED_none:
      my_ws2812_set_all_off(m);
      LED_Strip(m);
      target_mode[m] = 0;
      break;
    case all_finish:
      my_ws2812_set_all_off(m);
      LED_current_all_on(m);
      target_mode[m] = 2;
      break;
  }

  if (led_mode_last[m] == Waiting_hit && LED[m].mode != Waiting_hit) {
    WaterFlow_Stop(m);
  }

  if (water_flow_flag == 1 && water_flow_stepped == 0) {
    water_flow_stepped = 1;
    WaterFlow_Step();
    WaterFlow_Display(m);
    water_flow_flag = 0;
  }
}

void control_task(void) {
  time_tick++;
  LED_task();
  RC_CtrlData.rc.s2 = 0;

  switch (RC_CtrlData.rc.s2) {
    case 0:
      flag_start = 1;
      break;
    case 1:
      flag_start = 1;
      break;
    case 2:
      flag_start = 1;
      break;
    case 3:
      flag_start = 1;
      break;
  }

  CAN1_Send(14000, 0, 0, 0, 0);

  if (time_tick % 35 == 0) {
    led_time1++;
    if (led_time1 > 8)
      led_time1 = 1;
  }

  if (time_tick % 10000000 == 0)
    time_tick = 0;
}

int rc_input = 0;
int last_rc_input = 0;

void clean_time(void) {
  led_time1 = 1;
  last_led_time1 = 24;
}

void control_task_Init(void) {
  WaterFlow_Init();
}