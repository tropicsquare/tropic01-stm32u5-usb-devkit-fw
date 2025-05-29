/*
 * File:   buf.c
 * Author: pep
 *
 * Created on February 12, 2010, 6:29 AM
 */

#include "buf.h"

#include <string.h>

#define	THIS_SOURCE_ID	3

#define	_ASSERT(x,msg) OS_ASSERT(x,msg)

static void buf_pos_normalize(const buf_t* buf, int* pos);
static void buf_range_normalize(const buf_t* buf, int* pos, int* len);
static void buf_range_check(const buf_t* buf, int pos, int len);
static void buf_resize(buf_t* buf, int pos, int* len);

/*--------------------------------------------------------------------------*/

bool buf_init(buf_t* buf, char* data, int size) {
	_ASSERT (size > 0, "BUF_RANGE_FAILURE buf_init()");
	
	if (data == NULL)
	{
		if ((buf->data = (char *)OS_MEM_ALLOC (size)) == NULL)
		{
			buf->size = 0;
			return (false);
		}
	}
	else
	{
		buf->data = data;
	}
	buf->size = size;
	buf_adjust(buf, 0);
	return (true);
}

bool buf_init_full(buf_t* buf, char* data, int size) {
	if (data == NULL)
	{
		buf->size = 0;
		return (false);
	}
	else
	{
		buf->data = data;
	}
	buf->size = size+1;
	buf->length = size;
	return (true);
}

void buf_free(buf_t* buf) {
	if (buf->data != NULL)
		OS_MEM_FREE (buf->data);
	buf->size = 0;
}

void buf_adjust(buf_t* buf, int len) {
	_ASSERT (len >= 0 && len < buf->size, "BUF_RANGE_FAILURE buf_adjust()");
	
	buf->length = len;
	buf->data[len] = '\0';
}

void buf_fill(buf_t* buf, char c) {
	int i;

	for (i = 0; i < buf->size - 1; i++) {
		buf->data[i] = c;
	}

	buf_adjust(buf, buf->size - 1);
}

bool buf_equals_buf(const buf_t* buf1, const buf_t* buf2) {
	if (buf_length(buf1) == buf_length(buf2) && memcmp(buf_data(buf1), buf_data(buf2), buf_length(buf1)) == 0) {
		return true;
	}

	return false;
}

bool buf_equals_str(const buf_t* buf, const char* str) {
	if ((u16)buf_length(buf) == strlen(str) && strcmp(buf_data(buf), str) == 0) {
		return true;
	}

	return false;
}

/*--------------------------------------------------------------------------*/

void buf_delete(buf_t* buf, int pos, int len) {
	buf_range_normalize(buf, &pos, &len);
	buf_range_check(buf, pos, len);
	buf_mem_shift(buf->data + pos + len, buf->length - pos - len, -len);
	buf_adjust(buf, buf->length - len);
}

void buf_trim(buf_t* buf, int pos, int len) {
	buf_range_normalize(buf, &pos, &len);

	buf_delete(buf, pos + len, BUF_EOL);
	buf_delete(buf, 0, pos);
}

/*--------------------------------------------------------------------------*/

void* buf_mem(const buf_t* buf, int pos, int len) {
	buf_range_normalize(buf, &pos, &len);
	buf_range_check(buf, pos, len);

	return buf->data + pos;
}

/*--------------------------------------------------------------------------*/

int buf_replace_mem(buf_t* buf, int pos, int len, const void* mem) {
	buf_range_normalize(buf, &pos, &len);
	buf_range_check(buf, pos, len);

	buf_mem_copy(buf->data + pos, mem, len);

	return len;
}

int buf_replace_str(buf_t* buf, int pos, const void* str) {
	int str_len;

	str_len = strlen((const char *)str);
	buf_replace_mem(buf, pos, str_len, str);

	return str_len;
}

int buf_replace_buf(buf_t* buf, int pos, const buf_t* src_buf) {
	buf_replace_mem(buf, pos, src_buf->length, src_buf->data);

	return src_buf->length;
}

int buf_replace_fmt(buf_t* buf, int pos, int len, const char* fmt, ...) {
	va_list args;
	int n;

	va_start(args, fmt);
	n = buf_replace_vfmt(buf, pos, len, fmt, args);
	va_end(args);

	return n;
}

int buf_replace_vfmt(buf_t* buf, int pos, int len, const char* fmt, va_list args) {
	char last;
	int n;

	buf_range_normalize(buf, &pos, &len);
	buf_range_check(buf, pos, len);

	last = buf->data[pos + len];
	n = os_vsnprintf(buf->data + pos, len + 1, fmt, args);
	_ASSERT (n == len, "BUF_RANGE_FAILURE buf_replace_vfmt()");
	buf->data[pos + len] = last;

	return n;
}

int buf_replace_s16(buf_t* buf, int pos, s16 num) {
	return buf_replace_mem(buf, pos, 2, &num);
}

/*--------------------------------------------------------------------------*/

int buf_insert_mem(buf_t* buf, int pos, int len, const void* mem) {
	buf_range_normalize(buf, &pos, &len);
	buf_resize(buf, pos, &len);

	buf_mem_copy(buf->data + pos, mem, len);

	return len;
}

int buf_insert_char(buf_t* buf, int pos, char c) {
	return buf_insert_mem(buf, pos, 1, &c);
}

int buf_insert_str(buf_t* buf, int pos, const void* str) {
	return buf_insert_mem(buf, pos, strlen((const char*)str), str);
}

int buf_insert_buf(buf_t* buf, int pos, const buf_t* src_buf) {
	return buf_insert_mem(buf, pos, src_buf->length, src_buf->data);
}

int buf_insert_fmt(buf_t* buf, int pos, const char* fmt, ...) {
	va_list args;
	int n;

	va_start(args, fmt);
	n = buf_insert_vfmt(buf, pos, fmt, args);
	va_end(args);

	return n;
}

int buf_insert_vfmt(buf_t* buf, int pos, const char* fmt, va_list args) {
	int space;
	int n_ret;
	int n;

	buf_pos_normalize(buf, &pos);
	_ASSERT (pos >= 0 && pos <= buf->length, "BUF_RANGE_FAILURE buf_insert_vfmt()");

	space = buf_space(buf) + 1;
	buf_mem_shift(buf->data + pos, buf->length - pos, space);

	n_ret = os_vsnprintf(buf->data + pos, space, fmt, args);
	/* log_dump(0, buf->data, buf->size); */
	if (n_ret < 0) {
		n = 0;
	} else if (n_ret >= space) {
		n = space - 1;
	} else {
		n = n_ret;
	}
	buf_mem_shift(buf->data + pos + space, buf->length - pos, -space + n);
	buf_adjust(buf, buf->length + n);
	/* log_msg(0, "space:%d n:%d", space, n); */

	return n_ret;
}

int buf_insert_s8(buf_t* buf, int pos, s8 num) {
	return buf_insert_mem(buf, pos, 1, &num);
}

int buf_insert_s16(buf_t* buf, int pos, s16 num) {
	return buf_insert_mem(buf, pos, 2, &num);
}

int buf_insert_s32(buf_t* buf, int pos, s32 num) {
	return buf_insert_mem(buf, pos, 4, &num);
}

int buf_insert_s64(buf_t* buf, int pos, s64 num) {
	return buf_insert_mem(buf, pos, 8, &num);
}

static char _hex_to_a(char ch)
{
	ch &= 0xF;
	return (ch >= 10 ? (ch - 10 + 'A') : (ch + '0'));
}

int buf_insert_hex(buf_t* buf, int pos, u8 *data, int len) 
{
    int n=0;

    while (len--)
    {
        if (! buf_insert_char(buf, pos, _hex_to_a(*data >> 4)))
        {
            break;
        }

        if (pos != BUF_EOL)
        {
            pos++;
        }

        if (! buf_insert_char(buf, pos, _hex_to_a(*data & 0x0F)))
        {
            break;
        }

        if (pos != BUF_EOL)
        {
            pos++;
        }
        n+=2;
        data++;
    }
	return (n);
}

/*--------------------------------------------------------------------------*/

int buf_append_fmt(buf_t* buf, const char* fmt, ...) {
	va_list args;
	int n;

	va_start(args, fmt);
	n = buf_insert_vfmt(buf, buf->length, fmt, args);
	va_end(args);

	return n;
}

/*--------------------------------------------------------------------------*/

int buf_assign_mem(buf_t* buf, int len, const void* mem) {
	buf_clear(buf);
	return buf_append_mem(buf, len, mem);
}

int buf_assign_char(buf_t* buf, char c) {
	buf_clear(buf);
	return buf_append_char(buf, c);
}

int buf_assign_str(buf_t* buf, const void* str) {
	buf_clear(buf);
	return buf_append_str(buf, str);
}

int buf_assign_buf(buf_t* buf, const buf_t* src_buf) {
	buf_clear(buf);
	return buf_append_buf(buf, src_buf);
}

int buf_assign_fmt(buf_t* buf, const char* fmt, ...) {
	va_list args;
	int n;

	va_start(args, fmt);
	n = buf_assign_vfmt(buf, fmt, args);
	va_end(args);

	return n;
}

int buf_assign_vfmt(buf_t* buf, const char* fmt, va_list args) {
	int n;

	buf_clear(buf);
	n = buf_insert_vfmt(buf, 0, fmt, args);

	return n;
}

int buf_assign_s8(buf_t* buf, s8 num) {
	buf_clear(buf);
	return buf_append_s8(buf, num);
}

int buf_assign_s16(buf_t* buf, s16 num) {
	buf_clear(buf);
	return buf_append_s16(buf, num);
}

int buf_assign_s32(buf_t* buf, s32 num) {
	buf_clear(buf);
	return buf_append_s32(buf, num);
}

int buf_assign_s64(buf_t* buf, s64 num) {
	buf_clear(buf);
	return buf_append_s64(buf, num);
}

/*--------------------------------------------------------------------------*/

void buf_mem_copy(void* dest, const void* src, int len) {
	char* d;
	const char* s;
	int i;

	d = (char *)dest;
	s = (const char *)src;
	for (i = 0; i < len; i++) {
		*d++ = *s++;
	}
}

void buf_mem_shift(void* mem, int len, int shift) {
	int i;
	char* p;

	p = (char *)mem;
	if (shift < 0) {
		/* LEFT */
		for (i = 0; i < len; i++) {
			p[i + shift] = p[i];
		}

	} else {
		/* RIGHT */
		for (i = len - 1; i >= 0; i--) {
			p[i + shift] = p[i];
		}
	}
}

/*--------------------------------------------------------------------------*/

void buf_pos_normalize(const buf_t* buf, int* pos) {
	if (*pos == BUF_EOL) {
		*pos = buf->length;

	} else if (*pos < 0) {
		*pos += buf->length;
	}
}

void buf_range_normalize(const buf_t* buf, int* pos, int* len) {
	buf_pos_normalize(buf, pos);
	if (*len == BUF_EOL) {
		*len = buf->length - *pos;
	}
}

void buf_range_check(const buf_t* buf, int pos, int len) {
	_ASSERT (pos >= 0 && len >= 0 && (pos + len) <= buf->length, "BUF_RANGE_FAILURE buf_range_check()");
}

void buf_resize(buf_t* buf, int pos, int* len) {
	/* CHECK SPACE */
	_ASSERT (pos >=0 && *len >= 0 && pos <= buf->length, "BUF_RANGE_FAILURE buf_resize()");

	if (*len > buf_space(buf)) {
		*len = buf_space(buf);
	}

	buf_mem_shift(buf->data + pos, buf->length - pos, *len);
	buf_adjust(buf, buf->length + *len);
}
