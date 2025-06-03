#include "os.h"
#include "log.h"

#if LOG_ENABLE

#include <stdarg.h>

static int _log_level = LOG_LEVEL_OFF;

static void _log_timestamp(void)
{
    u64 t = OS_TIMER()/OS_TIMER_MS;
    printf("\r# %ld ",(u32)t);
}

void log_msg(int level, const ascii *id, const ascii *fmt, ... )
{
    va_list args;

    const ascii *type;

    if (level > _log_level)
        return;

    switch (level)
    {
    case LOG_LEVEL_ERROR:   type = "E"; break;
    case LOG_LEVEL_WARNING: type = "W"; break;
    case LOG_LEVEL_INFO:    type = "I"; break;
    case LOG_LEVEL_DEBUG:   type = "D"; break;
        break;
    default:
        return;
    }

    va_start(args, fmt);

    _log_timestamp();
    printf("%s(%s): ", type, id);
    vprintf(fmt, args);
    va_end(args);
    printf(NL);

    OS_FLUSH();
}

void log_dump(const ascii *id, u8 *data, int len)
{
#define _MAX_DUMP_SIZE (256)

    int i;
    char buf[2*_MAX_DUMP_SIZE + 1];

    if (len <= 1)
        return;

    if (len > _MAX_DUMP_SIZE)
        len = _MAX_DUMP_SIZE;

    for (i=0; i<len; i++)
    {
        sprintf(buf+2*i, "%02x", data[i]);
    }

    log_msg(LOG_LEVEL_DEBUG, id, buf);
}

void log_level_set(int level)
{
    _log_level = level;
}

int log_level_get(void)
{
    return (_log_level);
}

#endif // LOG_ENABLE

