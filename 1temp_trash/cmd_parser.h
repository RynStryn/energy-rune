#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <stdint.h>

void CmdParser_Init(void);
void CmdParser_ProcessLine(const char* cmd, uint16_t len);
void CmdParser_HandleUartData(void);

#endif