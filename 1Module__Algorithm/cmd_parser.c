#include "cmd_parser.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffTask.h"
#include "motorTask.h"
#include "pid.h"
#include "software_config.h"



#include "stm32h7xx_hal.h"
#include "wireless.h"
extern UART_HandleTypeDef huart1;
extern RingBuffer_t       uart_ring_buffer;
extern int                rune_mode;
extern gError_t           gError;
extern pid_t              pid;
#endif

static char     cmd_buffer[CMD_BUF_SIZE];
static uint16_t cmd_buffer_index = 0;

typedef struct {
    const char *alias;
    const char *verb;
    const char *noun;
} AliasEntry_t;

static const AliasEntry_t alias_table[] = {
    {"gs", "Get", "SystemInfo"},
    {"sm", "Set", "Motor"},
    {"gm", "Get", "Motor"},
    {"sr", "Set", "Rune"},
    {"gr", "Get", "Rune"},
    {"res", "Reset", "System"},
    {"h", "Get", "Help"},
};

static const uint8_t alias_table_size =
        sizeof(alias_table) / sizeof(alias_table[0]);

static eCmdResult HandleGetSystemInfo(CmdParseResult_t *cmd);

static eCmdResult HandleSetMotor(CmdParseResult_t *cmd);

static eCmdResult HandleGetMotor(CmdParseResult_t *cmd);

static eCmdResult HandleSetRune(CmdParseResult_t *cmd);

static eCmdResult HandleGetRune(CmdParseResult_t *cmd);

static eCmdResult HandleReset(CmdParseResult_t *cmd);

static eCmdResult HandleHelp(CmdParseResult_t *cmd);

static eCmdResult HandleGetPID(CmdParseResult_t *cmd);

static eCmdResult HandleSetPID(CmdParseResult_t *cmd);

static const CmdEntry_t cmd_table[] = {
    {"Get", "SystemInfo", HandleGetSystemInfo, "Get system information"},
    {"Set", "Motor", HandleSetMotor, "Set motor parameters: sm <id> <value>"},
    {"Get", "Motor", HandleGetMotor, "Get motor parameters: gm [id]"},
    {"Set", "Rune", HandleSetRune, "Set rune mode: sr <small|big>"},
    {"Get", "Rune", HandleGetRune, "Get rune mode: gr"},
    {"Reset", "System", HandleReset, "Reset system: res"},
    {"Get", "Help", HandleHelp, "Get help information: h"},
    {
        "Get",
        "PID",
        HandleGetPID,
        "Get PID params: gm pid <kp|ki|kd|mode|i_max>"
    },
    {
        "Set",
        "PID",
        HandleSetPID,
        "Set PID params: sm pid <kp|ki|kd|mode|i_max> <value>"
    },
};

static const uint8_t cmd_table_size = sizeof(cmd_table) / sizeof(cmd_table[0]);

void CmdParser_Init(void)
{
    cmd_buffer_index = 0;
    memset(cmd_buffer, 0, CMD_BUF_SIZE);
}

void CmdParser_SendResponse(const char *format, ...)
{
    char    tx_buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(tx_buf, sizeof(tx_buf), format, args);
    va_end(args);

    uint16_t len = strlen(tx_buf);
    HAL_UART_Transmit(&huart1, (uint8_t *) tx_buf, len, HAL_MAX_DELAY);
}

void CmdParser_SendOk(const char *msg)
{
    CmdParser_SendResponse("[OK] %s\r\n", msg);
}

void CmdParser_SendError(const char *msg)
{
    CmdParser_SendResponse("[ERROR] %s\r\n", msg);
}

static void ResolveAlias(CmdParseResult_t *cmd)
{
    for (uint8_t i = 0; i < alias_table_size; i++)
    {
        if (strcmp(cmd->verb, alias_table[i].alias) == 0)
        {
            cmd->verb = (char *) alias_table[i].verb;
            cmd->noun = (char *) alias_table[i].noun;
            return;
        }
    }
}

static bool ParseCommandLine(char *line, CmdParseResult_t *result)
{
    char *saveptr;
    memset(result, 0, sizeof(CmdParseResult_t));

    const char *cmd_delim = " \t\r\n-";
    char *      token     = strtok_r(line, cmd_delim, &saveptr);
    if (token == NULL)
    {
        return false;
    }
    result->verb = token;

    ResolveAlias(result);

    if (result->noun == NULL)
    {
        token = strtok_r(NULL, cmd_delim, &saveptr);
        if (token == NULL)
        {
            return false;
        }
        result->noun = token;
    }

    const char *arg_delim = " \t\r\n";
    uint8_t     arg_idx   = 0;

    while ((token = strtok_r(NULL, arg_delim, &saveptr)) != NULL)
    {
        if (arg_idx >= MAX_ARGS)
        {
            break;
        }
        result->args[arg_idx++] = token;
    }
    result->arg_count = arg_idx;

    if (result->arg_count > 0 && strcmp(result->args[0], "pid") == 0)
    {
        if (strcmp(result->verb, "Set") == 0 || strcmp(result->verb, "Get") ==
            0)
        {
            result->noun = (char *) "PID";
            for (uint8_t i = 0; i < result->arg_count - 1; i++)
            {
                result->args[i] = result->args[i + 1];
            }
            result->arg_count--;
        }
    }

    return true;
}

static eCmdResult FindAndExecuteCommand(CmdParseResult_t *cmd)
{
    for (uint8_t i = 0; i < cmd_table_size; i++)
    {
        if (strcmp(cmd->verb, cmd_table[i].verb) == 0 &&
            strcmp(cmd->noun, cmd_table[i].noun) == 0)
        {
            return cmd_table[i].handler(cmd);
        }
    }
    return CMD_UNKNOWN;
}

void CmdParser_ProcessLine(const char *line, uint16_t length)
{
    CmdParseResult_t cmd_result;
    eCmdResult       result;

    if (length >= CMD_BUF_SIZE)
    {
        CmdParser_SendError("Command too long");
        return;
    }

    memcpy(cmd_buffer, line, length);
    cmd_buffer[length] = '\0';

    if (!ParseCommandLine(cmd_buffer, &cmd_result))
    {
        CmdParser_SendError(
            "Invalid command format. Use: Verb-Noun [args] or alias");
        return;
    }

    result = FindAndExecuteCommand(&cmd_result);

    switch (result)
    {
        case CMD_OK:
            break;
        case CMD_UNKNOWN:
            CmdParser_SendError("Unknown command. Use h for help");
            break;
        case CMD_INVALID_PARAM:
            CmdParser_SendError("Invalid parameters");
            break;
        case CMD_EXEC_ERROR:
            CmdParser_SendError("Command execution error");
            break;
    }
}

void CmdParser_HandleUartData(void)
{
    uint32_t pPut = uart_ring_buffer.pPut;
    uint32_t pGet = uart_ring_buffer.pGet;
    char     ch;

    while (pGet != pPut)
    {
        ch   = uart_ring_buffer.Buffer[pGet];
        pGet = (pGet + 1) % RX_BUF_SIZE;

        if (ch == '\r' || ch == '\n')
        {
            if (cmd_buffer_index > 0)
            {
                CmdParser_ProcessLine(cmd_buffer, cmd_buffer_index);
                cmd_buffer_index = 0;
                memset(cmd_buffer, 0, CMD_BUF_SIZE);
            }
        } else
        {
            if (cmd_buffer_index < CMD_BUF_SIZE - 1)
            {
                cmd_buffer[cmd_buffer_index++] = ch;
            } else
            {
                cmd_buffer_index = 0;
                CmdParser_SendError("Command buffer overflow");
                break;
            }
        }
    }

    uart_ring_buffer.pGet = pGet;
}

static eCmdResult HandleGetSystemInfo(CmdParseResult_t *cmd)
{
    CmdParser_SendResponse("System Information:\r\n");
    CmdParser_SendResponse("  Rune Mode: %s\r\n",
                           (rune_mode == BIG_RUNE)
                               ? "BIG"
                               : "SMALL");
    CmdParser_SendResponse(
        "  Error Counts: GenerationLoop=%d, updateTarget=%d, RNGwrong=%d\r\n",
        gError.GenerationLoop,
        gError.updateTarget,
        gError.RNGwrong);
    CmdParser_SendOk("System info retrieved");
    return CMD_OK;
}

static eCmdResult HandleSetMotor(CmdParseResult_t *cmd)
{
    if (cmd->arg_count >= 1 && strcmp(cmd->args[0], "pid") == 0)
    {
        return HandleSetPID(cmd);
    }

    if (cmd->arg_count < 2)
    {
        CmdParser_SendError("Usage: sm <id> <value>");
        return CMD_INVALID_PARAM;
    }

    char *endptr;
    long  motor_id = strtol(cmd->args[0], &endptr, 10);
    if (*endptr != '\0' || motor_id < 0 || motor_id > 3)
    {
        CmdParser_SendError("Motor ID must be 0-3");
        return CMD_INVALID_PARAM;
    }

    long value = strtol(cmd->args[1], &endptr, 10);
    if (*endptr != '\0')
    {
        CmdParser_SendError("Invalid motor value");
        return CMD_INVALID_PARAM;
    }

    CmdParser_SendResponse("Set Motor[%ld] = %ld\r\n", motor_id, value);
    CmdParser_SendOk("Motor parameter set");
    return CMD_OK;
}

static eCmdResult HandleGetMotor(CmdParseResult_t *cmd)
{
    if (cmd->arg_count >= 1 && strcmp(cmd->args[0], "pid") == 0)
    {
        return HandleGetPID(cmd);
    }

    if (cmd->arg_count < 1)
    {
        CmdParser_SendResponse("Motor Angle: %.2f\r\n",
                               motor_encoder.ecd_angle);
        CmdParser_SendResponse("Motor Rate: %d\r\n", motor_encoder.filter_rate);
        CmdParser_SendOk("All motor info retrieved");
        return CMD_OK;
    }

    char *endptr;
    long  motor_id = strtol(cmd->args[0], &endptr, 10);
    if (*endptr != '\0' || motor_id < 0 || motor_id > 3)
    {
        CmdParser_SendError("Motor ID must be 0-3");
        return CMD_INVALID_PARAM;
    }

    CmdParser_SendResponse("Motor[%ld] Angle: %.2f\r\n",
                           motor_id,
                           motor_encoder.ecd_angle);
    CmdParser_SendOk("Motor info retrieved");
    return CMD_OK;
}

static eCmdResult HandleSetRune(CmdParseResult_t *cmd)
{
    if (cmd->arg_count < 1)
    {
        CmdParser_SendError("Usage: sr <small|big>");
        return CMD_INVALID_PARAM;
    }

    if (strcmp(cmd->args[0], "small") == 0)
    {
        rune_mode = SMALL_RUNE;
        CmdParser_SendOk("Rune mode set to SMALL");
    } else if (strcmp(cmd->args[0], "big") == 0)
    {
        rune_mode = BIG_RUNE;
        CmdParser_SendOk("Rune mode set to BIG");
    } else
    {
        CmdParser_SendError("Invalid rune mode. Use: small or big");
        return CMD_INVALID_PARAM;
    }

    return CMD_OK;
}

static eCmdResult HandleGetRune(CmdParseResult_t *cmd)
{
    CmdParser_SendResponse("Current Rune Mode: %s\r\n",
                           (rune_mode == BIG_RUNE)
                               ? "BIG"
                               : "SMALL");
    CmdParser_SendOk("Rune mode retrieved");
    return CMD_OK;
}

static eCmdResult HandleReset(CmdParseResult_t *cmd)
{
    Motor_ParamsReset();
    CmdParser_SendOk("System reset complete");
    return CMD_OK;
}

static eCmdResult HandleHelp(CmdParseResult_t *cmd)
{
    CmdParser_SendResponse("Available Commands:\r\n");
    CmdParser_SendResponse("-------------------\r\n");

    CmdParser_SendResponse("Short Aliases:\r\n");
    CmdParser_SendResponse("  gs = Get-SystemInfo\r\n");
    CmdParser_SendResponse("  sm = Set-Motor\r\n");
    CmdParser_SendResponse("  gm = Get-Motor\r\n");
    CmdParser_SendResponse("  sr = Set-Rune\r\n");
    CmdParser_SendResponse("  gr = Get-Rune\r\n");
    CmdParser_SendResponse("  res = Reset-System\r\n");
    CmdParser_SendResponse("  h = Get-Help\r\n");
    CmdParser_SendResponse("\r\n");

    CmdParser_SendResponse("Full Commands:\r\n");
    for (uint8_t i = 0; i < cmd_table_size; i++)
    {
        CmdParser_SendResponse("  %s-%s\r\n",
                               cmd_table[i].verb,
                               cmd_table[i].noun);
        CmdParser_SendResponse("    %s\r\n", cmd_table[i].description);
    }

    CmdParser_SendOk("Help displayed");
    return CMD_OK;
}

static eCmdResult HandleGetPID(CmdParseResult_t *cmd)
{
    if (cmd->arg_count < 1)
    {
        CmdParser_SendResponse("PID Parameters:\r\n");
        CmdParser_SendResponse("  kp: %.4f\r\n", pid.p);
        CmdParser_SendResponse("  ki: %.4f\r\n", pid.i);
        CmdParser_SendResponse("  kd: %.4f\r\n", pid.d);
        CmdParser_SendResponse("  mode: %s\r\n",
                               (pid.pid_mode == POSITION_PID)
                                   ? "POSITION"
                                   : "DELTA");
        CmdParser_SendResponse("  i_max: %u\r\n", pid.integral_limit);
        CmdParser_SendResponse("  err[0]: %.4f\r\n", pid.err[0]);
        CmdParser_SendResponse("  err[1]: %.4f\r\n", pid.err[1]);
        CmdParser_SendResponse("  err[2]: %.4f\r\n", pid.err[2]);
        CmdParser_SendResponse("  i_out: %.4f\r\n", pid.iout);
        CmdParser_SendResponse("  out: %.4f\r\n", pid.out);
        CmdParser_SendOk("PID info retrieved");
        return CMD_OK;
    }

    const char *param = cmd->args[0];
    if (strcmp(param, "kp") == 0)
    {
        CmdParser_SendResponse("kp = %.4f\r\n", pid.p);
    } else if (strcmp(param, "ki") == 0)
    {
        CmdParser_SendResponse("ki = %.4f\r\n", pid.i);
    } else if (strcmp(param, "kd") == 0)
    {
        CmdParser_SendResponse("kd = %.4f\r\n", pid.d);
    } else if (strcmp(param, "mode") == 0)
    {
        CmdParser_SendResponse("mode = %s\r\n",
                               (pid.pid_mode == POSITION_PID)
                                   ? "POSITION"
                                   : "DELTA");
    } else if (strcmp(param, "i_max") == 0)
    {
        CmdParser_SendResponse("i_max = %u\r\n", pid.integral_limit);
    } else if (strcmp(param, "err") == 0)
    {
        CmdParser_SendResponse("err[0] = %.4f\r\n", pid.err[0]);
        CmdParser_SendResponse("err[1] = %.4f\r\n", pid.err[1]);
        CmdParser_SendResponse("err[2] = %.4f\r\n", pid.err[2]);
    } else if (strcmp(param, "i_out") == 0)
    {
        CmdParser_SendResponse("i_out = %.4f\r\n", pid.iout);
    } else if (strcmp(param, "out") == 0)
    {
        CmdParser_SendResponse("out = %.4f\r\n", pid.out);
    } else
    {
        CmdParser_SendError(
            "Unknown param. Use: kp, ki, kd, mode, i_max, err, i_out, out");
        return CMD_INVALID_PARAM;
    }

    CmdParser_SendOk("PID param retrieved");
    return CMD_OK;
}

static eCmdResult HandleSetPID(CmdParseResult_t *cmd)
{
    if (cmd->arg_count < 2)
    {
        CmdParser_SendError("Usage: sm pid <kp|ki|kd|mode|i_max> <value>");
        return CMD_INVALID_PARAM;
    }

    const char *param = cmd->args[0];

    if (strcmp(param, "mode") == 0)
    {
        if (strcmp(cmd->args[1], "pos") == 0 ||
            strcmp(cmd->args[1], "position") == 0)
        {
            pid.pid_mode = POSITION_PID;
            CmdParser_SendResponse("mode set to POSITION\r\n");
        } else if (strcmp(cmd->args[1], "delta") == 0)
        {
            pid.pid_mode = DELTA_PID;
            CmdParser_SendResponse("mode set to DELTA\r\n");
        } else
        {
            CmdParser_SendError("Invalid mode. Use: pos or delta");
            return CMD_INVALID_PARAM;
        }
    } else
    {
        char *endptr;
        float value = strtof(cmd->args[1], &endptr);
        if (*endptr != '\0')
        {
            CmdParser_SendError("Invalid numeric format");
            return CMD_INVALID_PARAM;
        }

        if (strcmp(param, "kp") == 0)
        {
            pid.p = value;
            CmdParser_SendResponse("kp set to %.4f\r\n", value);
        } else if (strcmp(param, "ki") == 0)
        {
            pid.i = value;
            CmdParser_SendResponse("ki set to %.4f\r\n", value);
        } else if (strcmp(param, "kd") == 0)
        {
            pid.d = value;
            CmdParser_SendResponse("kd set to %.4f\r\n", value);
        } else if (strcmp(param, "i_max") == 0)
        {
            pid.integral_limit = (uint32_t) value;
            CmdParser_SendResponse("i_max set to %u\r\n",
                                   pid.integral_limit);
        } else
        {
            CmdParser_SendError("Unknown param. Use: kp, ki, kd, mode, i_max");
            return CMD_INVALID_PARAM;
        }
    }

    CmdParser_SendOk("PID param set");
    return CMD_OK;
}
