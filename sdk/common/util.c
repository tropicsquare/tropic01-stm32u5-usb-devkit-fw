#include "util.h"

char strnicmp(const char *str1, const char *str2, unsigned char len)
{
    char a, b;

    for (; len; len--)
    {
        a = *(str1++);
        b = *(str2++);
        if (a == b)
        {
            if (a=='\0')
                break;
            continue;
        }
        if (((a >= 'A') && (a <= 'Z')) && (a + ('a' - 'A') == b))
            continue;
        if (((b >= 'A') && (b <= 'Z')) && (b + ('a' - 'A') == a))
            continue;
        return (a - b);

    }
    return (0);
}

bool is_char(char ch)
{
    if ((ch >= 'a') && (ch <= 'z'))
        return (true);
    if ((ch >= 'A') && (ch <= 'Z'))
        return (true);
    if ((ch >= '0') && (ch <= '9'))
        return (true);
    if ((ch == '*') || (ch == '#'))
        return (true);

    return (false);
}

int is_number(const char *str, int limit)
{
    int i;

    for (i = 0; (i < limit) && (*str != '\0'); i++, str++)
    {
        if ((*str < '0') || (*str > '9'))
        {
            return (0);
        }
    }
    return (i);
}

static int _hex_to_bin(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return (ch -'0');
    if ((ch >= 'a') && (ch <= 'f'))
        return (10 + ch - 'a');
    if ((ch >= 'A') && (ch <= 'F'))
        return (10 + ch - 'A');

    return (-1);
}

bool is_hex(char ch)
{
	return ((_hex_to_bin(ch) >= 0) ? true : false);
}

static bool _get_hex(u8 *dest, const char *src)
{   // read one byte of HEX value
    int tmp;
    u8 result = 0;
    
    if ((tmp = _hex_to_bin(*src++)) < 0)
        return (false);

    result = tmp<<4;

    if ((tmp = _hex_to_bin(*src++)) < 0)
        return (false);

    result |= tmp;
    *dest = result;
    return (true);
}

int hex_to_bin(u8 *dest, const char *src, int limit)
{
	int i;

	for (i=0; i<limit; i++)
	{
        if (! _get_hex(dest, src))
            break;

		dest++;
		src += 2;
	}
	return (i);
}

