#include "cmd_parser.h"
#include <stdio.h>
#include <string.h>
#include "DMA_buffer_config.h"
#include "pid.h"
#include "software_config.h"
#include "target.h"

#ifdef UNIT_TEST
#include "mock_motor.h"
#include "mock_stm32h7xx_hal.h"
#include "mock_wireless.h"
UART_HandleTypeDef huart1;
pid_t pid_big;
gError_t gError;
#else
#include "motorTask.h"
#include "stm32h7xx_hal.h"
#include "wireless.h"
//extern UART_HandleTypeDef huart1;
extern pid_t pid_big;
#endif

#define CMD_BUF_SIZE 256

static char cmd_buffer[CMD_BUF_SIZE];
static uint16_t cmd_index;

void CmdParser_Init(void) {
  cmd_index = 0;
  memset(cmd_buffer, 0, CMD_BUF_SIZE);
}

static void send_response(const char* msg) {
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
}

static int is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static const char* skip_whitespace(const char* p) {
  while (p && *p && is_whitespace(*p)) {
    p++;
  }
  return p;
}

static void trim_trailing(char* s) {
  int len = strlen(s);
  while (len > 0 && is_whitespace(s[len - 1])) {
    s[len - 1] = '\0';
    len--;
  }
}

static void cmd_help(void) {
  send_response(
      "h: help\ngr: get rune\nSet-Rune <small|big>: set rune mode\ngs: get "
      "system info\nres: reset system\ngm: get motor\nsm <id> <val>: set "
      "motor\nGet-PID: get PID params\nSet-PID <kp|ki|kd|mode> <val>: set "
      "PID\n");
}

static void cmd_get_rune(void) {
  char buf[64];
  snprintf(buf, sizeof(buf), "rune_mode=%d\n", g_game.rune_mode);
  send_response(buf);
}

static void cmd_set_rune(const char* arg) {
  if (strcmp(arg, "small") == 0) {
    g_game.rune_mode = SMALL_RUNE;
    send_response("rune set to small\n");
  } else if (strcmp(arg, "big") == 0) {
    g_game.rune_mode = BIG_RUNE;
    send_response("rune set to big\n");
  } else {
    send_response("invalid rune mode\n");
  }
}

static void cmd_get_sysinfo(void) {
  send_response("EnergyRune_H7 system\n");
}

static void cmd_reset(void) {
  Motor_ParamsReset();
  send_response("system reset\n");
}

static void cmd_get_motor(void) {
  char buf[64];
  snprintf(buf, sizeof(buf), "motor angle=%.1f rate=%d\n",
           motor_encoder.ecd_angle, motor_encoder.filter_rate);
  send_response(buf);
}

static void cmd_set_motor(const char* args) {
  int id, val;
  if (sscanf(args, "%d %d", &id, &val) == 2) {
    if (id >= 0 && id <= 2) {
      send_response("motor set\n");
    } else {
      send_response("invalid motor id\n");
    }
  } else {
    send_response("usage: sm <id> <val>\n");
  }
}

static void cmd_get_pid(void) {
  char buf[128];
  snprintf(buf, sizeof(buf), "kp=%.3f ki=%.3f kd=%.3f mode=%u\n", pid_big.p,
           pid_big.i, pid_big.d, pid_big.pid_mode);
  send_response(buf);
}

static void cmd_set_pid(const char* args) {
  char param[32];
  if (sscanf(args, "%31s", param) < 1) {
    send_response("usage: Set-PID <kp|ki|kd|mode> <val>\n");
    return;
  }
  if (strcmp(param, "mode") == 0) {
    char mode_str[32];
    if (sscanf(args, "%*s %31s", mode_str) < 1) {
      send_response("usage: Set-PID mode <pos|delta>\n");
      return;
    }
    if (strcmp(mode_str, "pos") == 0) {
      pid_big.pid_mode = POSITION_PID;
      send_response("pid mode set to position\n");
    } else if (strcmp(mode_str, "delta") == 0) {
      pid_big.pid_mode = DELTA_PID;
      send_response("pid mode set to delta\n");
    } else {
      send_response("unknown pid mode\n");
    }
  } else if (strcmp(param, "kp") == 0) {
    float val;
    if (sscanf(args, "%*s %f", &val) == 1) {
      pid_big.p = val;
      send_response("pid kp set\n");
    }
  } else if (strcmp(param, "ki") == 0) {
    float val;
    if (sscanf(args, "%*s %f", &val) == 1) {
      pid_big.i = val;
      send_response("pid ki set\n");
    }
  } else if (strcmp(param, "kd") == 0) {
    float val;
    if (sscanf(args, "%*s %f", &val) == 1) {
      pid_big.d = val;
      send_response("pid kd set\n");
    }
  } else {
    send_response("unknown pid param\n");
  }
}
void CmdParser_ProcessLine(const char* cmd, uint16_t len) {
  if (cmd == NULL || len == 0) {
    return;
  }
  const char* p = skip_whitespace(cmd);
  if (*p == '\0') {
    return;
  }
  char first[32];
  char rest[CMD_BUF_SIZE];
  rest[0] = '\0';
  if (sscanf(p, "%31s %255[^\n]", first, rest) < 1) {
    return;
  }
  if (strcmp(first, "h") == 0 || strcmp(first, "help") == 0) {
    cmd_help();
  } else if (strcmp(first, "gr") == 0 || strcmp(first, "Get-Rune") == 0) {
    cmd_get_rune();
  } else if (strcmp(first, "sr") == 0 || strcmp(first, "Set-Rune") == 0) {
    cmd_set_rune(rest);
  } else if (strcmp(first, "gs") == 0 || strcmp(first, "Get-SysInfo") == 0) {
    cmd_get_sysinfo();
  } else if (strcmp(first, "res") == 0 || strcmp(first, "Reset") == 0) {
    cmd_reset();
  } else if (strcmp(first, "gm") == 0 || strcmp(first, "Get-Motor") == 0) {
    cmd_get_motor();
  } else if (strcmp(first, "sm") == 0 || strcmp(first, "Set-Motor") == 0) {
    cmd_set_motor(rest);
  } else if (strcmp(first, "gp") == 0 || strcmp(first, "Get-PID") == 0) {
    cmd_get_pid();
  } else if (strcmp(first, "sp") == 0 || strcmp(first, "Set-PID") == 0) {
    cmd_set_pid(rest);
  } else {
    send_response("unknown command\n");
  }
}
void CmdParser_HandleUartData(void) {
  uint32_t write_index = uart_ring_buffer.pPut;
  uint32_t read_index = uart_ring_buffer.pGet;
  while (read_index != write_index) {
    uint8_t c = uart_ring_buffer.Buffer[read_index];
    read_index = (read_index + 1) % RX_BUF_SIZE;
    if (c == '\n' || c == '\r') {
      if (cmd_index > 0) {
        cmd_buffer[cmd_index] = '\0';
        CmdParser_ProcessLine(cmd_buffer, cmd_index);
        cmd_index = 0;
      }
    } else {
      if (cmd_index < CMD_BUF_SIZE - 1) {
        cmd_buffer[cmd_index++] = c;
      }
    }
  }
  uart_ring_buffer.pGet = read_index;
}