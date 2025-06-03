#include "common.h"
#include "cmd.h"
#include "hardware.h"
#include "gpreg.h"
#include "gpio.h"
#include "wd.h"
#include "main.h"
#include "spi.h"

#include "version.h"

typedef struct _cmd_t {
    const char *text;
    bool (*pfunc)(const struct _cmd_t *cmd);
    bool (*pfunc_set)(const struct _cmd_t *cmd,  const char **pptext);
    const char *help;
} cmd_t;

static const cmd_t _CMD_TABLE[];

static const char *ERR_INVALID_PARAMETER = "invalid parameter";
static const char *ERR_MISSING_PARAMETER = "missing parameter";
static const char *ERR_ILLEGAL_PARAMETER = "illegal parameter";
static const char *ERR_UNKNOWN_COMMAND   = "unknown command";

#define _PIN_STATE(pin) ((pin) ? 1 : 0)

static bool _pwr_state = true;

static void _cmd_error(const char *msg)
{
    OS_PRINTF("ERROR: %s" NL, msg);
}

static void _skip_spaces(const char **pptext)
{
    while (**pptext == ' ')
    {
        (*pptext)++;
    }
}

static bool _isdigit(char ch)
{
    return (((ch >= '0') && (ch <= '9')) ? true : false);
}

char *parse_number(s32 *number, const char *s)
{
    char *p;
    s32 tmp = 0;

    p = (char *) s;

    if (*p == '-')
    {
        p++;
    }
    if (_isdigit(*p) == 0)
    {
        return (NULL);
    }
    while (_isdigit(*p))
    {
        tmp *= 10;
        tmp += *p - '0';
        p++;
    }
    if (*s == '-')
    {
        *number = 0-tmp;
    }
    else
    {
        *number = tmp;
    }

    return p;
}

static bool _cmd_fetch_bool(bool *dest, const char **pptext)
{
    char ch = **pptext;
    if (ch == '0')
    {
        *dest = false;
    }
    else if (ch == '1')
    {
        *dest = true;
    }
    else
    {
        return (false);
    }
    (*pptext)++;
    return (true);
}

static bool _cmd_fetch_num(s32 *dest, const char **pptext)
{
    char *p;
    s32 num;

    if ((p = parse_number(&num, *pptext)) == NULL)
    {
        return (false);
    }

    *pptext = p;
    *dest = num;
    return (true);
}

static bool _cmd_fetch_hex(u8 *dest, const char **pptext)
{
    if (hex_to_bin(dest, *pptext, 1) != 1)
        return (false);

    (*pptext) += 2;
    return (true);
}

static bool _cmd_fetch_next(const char **pptext)
{
    _skip_spaces(pptext);

    if (**pptext != ',')
        return (false);

    (*pptext)++;
    return (true);
}

static bool _cmd_help(const cmd_t *cmd)
{
    const cmd_t *table = _CMD_TABLE;
    int i;

    OS_PRINTF("Tropicsquare USB/SPI interface" NL);
    OS_PRINTF("Type HEX characters to make SPI transfer (i.e. \"1122aabb\")" NL);
    OS_PRINTF("Supported commands:" NL);

    for (i = 0; ; i++)
    {
        if (table[i].text == NULL)
            break;

        if ((table[i].help == NULL)
         || (strlen(table[i].help) == 0))
            continue;

        if (strlen(table[i].text) == 0)
            break;

        OS_PRINTF("%s : %s" NL, table[i].text, table[i].help);
    }
    return (true);
}

static void _cmd_basic_reply(const cmd_t *cmd)
{
    OS_PRINTF("%s: ", cmd->text);
}

static bool _cmd_auto(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    if (main_spi_auto)
        OS_PRINTF("1, %02X, %02X" NL, main_spi_get_resp, main_spi_no_resp);
    else
       OS_PRINTF("0" NL);
    return (true);
}

static bool _cmd_auto_set(const struct _cmd_t *cmd, const char **pptext)
{
  #define MSG_HDR_GET_RESP     (0xAA)
  #define MSG_VALUE_NO_RESP    (0xFF)

    bool state;
    u8 get_resp = MSG_HDR_GET_RESP;
    u8 no_resp  = MSG_VALUE_NO_RESP;
    
    if (! _cmd_fetch_bool(&state, pptext))
    {
        goto err;
    }
    if (_cmd_fetch_next(pptext))
    {   // second parameter may be GET_RESP value
        if (! _cmd_fetch_hex(&get_resp, pptext))
            goto err;

    }
    if (_cmd_fetch_next(pptext))
    {   // thitd parameter may be NO_RESP value
        if (! _cmd_fetch_hex(&no_resp, pptext))
            goto err;
    }
    main_spi_auto     = state;
    main_spi_get_resp = get_resp;
    main_spi_no_resp  = no_resp;
    // OS_PRINTF("[%02x, %02x]", get_resp, no_resp);
    return (true);

err:
    _cmd_error(ERR_INVALID_PARAMETER);
    return (false);
}

#ifdef HW_BUTTON_PRESSED
static bool _cmd_button(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    
    if (HW_BUTTON_PRESSED)
        OS_PRINTF("1" NL);
    else
        OS_PRINTF("0" NL);

    return (true);
}
#endif // defined HW_BUTTON_PRESSED

static bool _cmd_cs(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    OS_PRINTF("%u" NL, (spi1_cs_state() == SPI_CS_ACTIVE) ? 1 : 0); // "1" == CS active (LOW)
    return (true);
}

static bool _cmd_cs_set(const struct _cmd_t *cmd, const char **pptext)
{
    bool state;
    
    if (! _cmd_fetch_bool(&state, pptext))
    {
        _cmd_error(ERR_INVALID_PARAMETER);
        return (false);
    }

    spi1_cs(state ? SPI_CS_ACTIVE : SPI_CS_IDLE);
    return (true);
}

static bool _cmd_clkdiv(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    OS_PRINTF("%lu" NL, spi1_get_prescaler());
    return (true);
}

static bool _cmd_clkdiv_set(const struct _cmd_t *cmd, const char **pptext)
{
    s32 value;

    if (! _cmd_fetch_num(&value, pptext))
    {
        goto err;
    }

    if (spi1_set_prescaler(value))
        return (true);

err:
    _cmd_error(ERR_INVALID_PARAMETER);
    return (false);
}

static bool _cmd_id(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    OS_PRINTF(VERSION_NAME NL);
    return (true);
}

char *ux_device_sn_text(void);

static bool _cmd_sn(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    OS_PRINTF("%s", ux_device_sn_text());
    OS_PRINTF(NL);
    return (true);
}

static bool _cmd_ver(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    OS_PRINTF("FW %s, HW %s, https://github.com/tropicsquare" NL, VERSION_STRING, HW_NAME);
    return (true);
}

static bool _cmd_reset(const cmd_t *cmd)
{
    OS_PRINTF("RESET" NL);
    OS_DELAY(10);
    wd_reset(GPREG_BOOT_REBOOT);
    return (true);
}

static bool _cmd_gpo(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    OS_PRINTF("%d" NL, _PIN_STATE(HW_GPO_IN));
    return (true);
}

static bool _cmd_pwr(const cmd_t *cmd)
{
    _cmd_basic_reply(cmd);
    OS_PRINTF("%d" NL, _pwr_state ? 1 : 0);
    return (true);
}

static bool _cmd_pwr_set(const struct _cmd_t *cmd,  const char **pptext)
{
    bool state;
    
    if (! _cmd_fetch_bool(&state, pptext))
    {
        _cmd_error(ERR_INVALID_PARAMETER);
        return (false);
    }

    if (state)
    {
        HW_CHIP_PWR_ON; 
        HW_SPI_OE_ENABLE;
    }
    else
    {
        HW_CHIP_PWR_OFF;
        HW_SPI_OE_DISABLE; // also disconnect SPI interface (if possible)
    }
    _pwr_state = state;
    return (true);
}

static const cmd_t *_find_cmd(const cmd_t *table, const char **text)
{
    uint16_t i, l;

    for (i = 0; ; i++)
    {
        if (table[i].text == NULL)
            return (NULL);

        if ((l = strlen(table[i].text)) == 0)
            return (NULL);

        if (strnicmp(*text, table[i].text, l) == 0)
        {   // text found
            // now next character should be OK
            if (! is_char((*text)[l]))
            {
                (*text) += l;
                return (&table[i]);
            }
        }
    }
    return (NULL);
}

static bool _process_cmd(const cmd_t *cmd, const char **pptext)
{
    _skip_spaces(pptext);

    if (**pptext == '=')
    {   // parameter included
        if (cmd->pfunc_set == NULL)
        {   // but not enabled for this command
            _cmd_error(ERR_INVALID_PARAMETER);
            return (false);
        }
        (*pptext)++;
        _skip_spaces(pptext);
        return (cmd->pfunc_set(cmd, pptext));
    }
    else if ((**pptext != '?') && (**pptext != '\0'))
    {
        _cmd_error(ERR_ILLEGAL_PARAMETER);
        return (false);
    }

    // parameter not included
    if (cmd->pfunc == NULL)
    {   // but parameter required
        _cmd_error(ERR_MISSING_PARAMETER);
        return (false);
    }

    return (cmd->pfunc(cmd));
}

void cmd_parse(const char *ptext)
{
    const cmd_t *cmd;

    if (*ptext == '#')
        return; // quietly skip remarks

    if ((cmd = _find_cmd(_CMD_TABLE, &ptext)) == NULL)
    {
        _cmd_error(ERR_UNKNOWN_COMMAND);
        return;
    }
    
    if (! _process_cmd(cmd, &ptext))
    {
        return;
    }
    OS_PRINTF("OK" NL);
}

static const cmd_t _CMD_TABLE[] = {
    {"AUTO",      _cmd_auto,    _cmd_auto_set,  "Automatic response reading get/set"},
#ifdef HW_BUTTON_PRESSED
    {"BUTTON",    _cmd_button,  NULL,           "Get button state"},
#endif // defined HW_BUTTON_PRESSED
    {"CLKDIV",    _cmd_clkdiv,  _cmd_clkdiv_set,"Clock divisor get/set"},
    {"CS",        _cmd_cs,      _cmd_cs_set,    "SPI chip select direct control"},
    {"GPO",       _cmd_gpo,     NULL,           "Show GPO state"},
    {"HELP",      _cmd_help,    NULL,           "This help text"},
    {"ID",        _cmd_id,      NULL,           "Request product id"},
    {"PWR",       _cmd_pwr,     _cmd_pwr_set,   "Get/set target power"},
    {"RESET",     _cmd_reset,   NULL,           "Instant reset"},
    {"SN",        _cmd_sn,      NULL,           "Request product serial number"},
    {"VER",       _cmd_ver,     NULL,           "Request version information"},

    {NULL, NULL, 0, NULL} // command list termination
};

