//
// Created by 31466 on 2026/2/24.
//

#ifndef ENERGYRUNE_H7_CMD_PARSER_H
#define ENERGYRUNE_H7_CMD_PARSER_H
#include <stdbool.h>
#include <stdint.h>

#define CMD_BUF_SIZE 256
#define MAX_ARGS 16

typedef enum {
  CMD_OK = 0,
  CMD_UNKNOWN,
  CMD_INVALID_PARAM,
  CMD_EXEC_ERROR
} eCmdResult;

typedef struct {
  char* verb;
  char* noun;
  char* args[MAX_ARGS];
  uint8_t arg_count;
} CmdParseResult_t;

typedef eCmdResult (*CmdHandler_t)(CmdParseResult_t* cmd);

typedef struct {
  const char* verb;
  const char* noun;
  CmdHandler_t handler;
  const char* description;
} CmdEntry_t;

void CmdParser_Init(void);
void CmdParser_ProcessLine(const char* line, uint16_t length);
void CmdParser_HandleUartData(void);
void CmdParser_SendResponse(const char* format, ...);
void CmdParser_SendOk(const char* msg);
void CmdParser_SendError(const char* msg);

#endif  // ENERGYRUNE_H7_CMD_PARSER_H
