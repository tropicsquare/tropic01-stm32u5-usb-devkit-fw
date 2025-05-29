/* 
 * File:   buf.h
 * Author: pep
 *
 * Created on February 12, 2010, 6:28 AM
 */

#ifndef BUF_H
#define	BUF_H

#include "os.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define	os_vsnprintf vsnprintf

#ifndef OS_MEM_ALLOC
  #define OS_MEM_ALLOC malloc
  #define OS_MEM_FREE  free
#endif // ~OS_MEM_ALLOC 
// #ifndef ASSERT
//   #define ASSERT(x) if(!(x)){while(1);}
// #endif // ~ASSERT

#define BUF_EOL INT_MAX

#define buf_def(TYPE, NAME, SIZE) \
	TYPE NAME##_mmeemm[SIZE] = { 0 }; \
	buf_t NAME = { NAME##_mmeemm, 0, SIZE } 

typedef struct {
	char* data;
	int length;
	int size;
} buf_t;

/*
 * BUF GLOBAL API
 */

#define buf_empty(BUF) ((BUF)->length == 0)
#define buf_full(BUF) ((BUF)->length == (BUF)->size - 1)

#define buf_length(BUF) ((BUF)->length)
#define buf_space(BUF) ((BUF)->size - (BUF)->length - 1)
#define buf_capacity(BUF) ((BUF)->size - 1)
#define buf_data(BUF) ((BUF)->data)

extern bool buf_init(buf_t* buf, char* data, int size);
extern bool buf_init_full(buf_t* buf, char* data, int size);
extern void buf_free(buf_t* buf);
extern void buf_adjust(buf_t* buf, int len);
extern void buf_fill(buf_t* buf, char c);
#define buf_clear(BUF) buf_adjust((BUF), 0)

extern bool buf_equals_buf(const buf_t* buf1, const buf_t* buf2);
extern bool buf_equals_str(const buf_t* buf, const char* str);

/*
 * BUF RANGE API
 *
 * delete:
 *   void   buf_delete(B,P,L)
 *   void   buf_trim(B,P,L)
 *
 * get/replace:
 *   void*  buf_mem(B,P,L)
 *   char   buf_char(B,P)
 *   intN   buf_intN(B,P)
 *   uintN  buf_uintN(B,P)
 *   int    buf_replace_[mem|str|buf|fmt|vfmt](B,P,...)
 *
 * insert/append/assign:
 *   - store up to buf size
 *   - return bytes stored (except *_fmt, which is compatible with *nprintf)
 *   int    buf_insert_[mem|char|str|buf|fmt|vfmt|intN|uintN](B,P,...)
 *   int    buf_append_[mem|char|str|buf|fmt|vfmt|intN|uintN](B,...)
 *   int    buf_assign_[mem|char|str|buf|fmt|vfmt|intN|uintN](B,...)
 */

/* delete: */
extern void buf_delete(buf_t* buf, int pos, int len);
extern void buf_trim(buf_t* buf, int pos, int len);

/* get/replace: */
extern void* buf_mem(const buf_t* buf, int pos, int len);
#define buf_char(BUF, POS) *(char*)buf_mem((BUF), (POS), 1)
#define buf_s8(BUF, POS) *(s8*)buf_mem((BUF), (POS), 1)
#define buf_s16(BUF, POS) *(s16*)buf_mem((BUF), (POS), 2)
#define buf_s32(BUF, POS) *(s32*)buf_mem((BUF), (POS), 4)
#define buf_s64(BUF, POS) *(s64*)buf_mem((BUF), (POS), 8)
#define buf_u8(BUF, POS) *(u8*)buf_mem((BUF), (POS), 1)
#define buf_u16(BUF, POS) *(u16*)buf_mem((BUF), (POS), 2)
#define buf_u32(BUF, POS) *(u32*)buf_mem((BUF), (POS), 4)
#define buf_u64(BUF, POS) *(u64*)buf_mem((BUF), (POS), 8)

extern int buf_replace_mem(buf_t* buf, int pos, int len, const void* mem);
extern int buf_replace_str(buf_t* buf, int pos, const void* str);
extern int buf_replace_buf(buf_t* buf, int pos, const buf_t* src_buf);
extern int buf_replace_fmt(buf_t* buf, int pos, int len, const char* fmt, ...);
extern int buf_replace_vfmt(buf_t* buf, int pos, int len, const char* fmt, va_list args);
extern int buf_replace_s16(buf_t* buf, int pos, s16 num);
#define buf_replace_u16(BUF, POS, NUM) buf_replace_s16((BUF), (POS), (s16)(NUM))

/* insert: */
extern int buf_insert_mem(buf_t* buf, int pos, int len, const void* mem);
extern int buf_insert_char(buf_t* buf, int pos, char c);
extern int buf_insert_str(buf_t* buf, int pos, const void* str);
extern int buf_insert_buf(buf_t* buf, int pos, const buf_t* src_buf);
extern int buf_insert_fmt(buf_t* buf, int pos, const char* fmt, ...);
extern int buf_insert_vfmt(buf_t* buf, int pos, const char* fmt, va_list args);
extern int buf_insert_s8(buf_t* buf, int pos, s8 num);
extern int buf_insert_s16(buf_t* buf, int pos, s16 num);
extern int buf_insert_s32(buf_t* buf, int pos, s32 num);
extern int buf_insert_s64(buf_t* buf, int pos, s64 num);
#define buf_insert_u8(BUF, POS, NUM) buf_insert_s8((BUF), (POS), (s8)(NUM))
#define buf_insert_u16(BUF, POS, NUM) buf_insert_s16((BUF), (POS), (s16)(NUM))
#define buf_insert_u32(BUF, POS, NUM) buf_insert_s32((BUF), (POS), (s32)(NUM))
#define buf_insert_u64(BUF, POS, NUM) buf_insert_s64((BUF), (POS), (s64)(NUM))
extern int buf_insert_hex(buf_t* buf, int pos, u8 *data, int len);

/* append: */
#define buf_append_mem(BUF, LEN, MEM) buf_insert_mem((BUF), BUF_EOL, (LEN), (MEM))
#define buf_append_char(BUF, C) buf_insert_char((BUF), BUF_EOL, (C))
#define buf_append_str(BUF, STR) buf_insert_str((BUF), BUF_EOL, (STR))
#define buf_append_buf(BUF, SRC_BUF) buf_insert_buf((BUF), BUF_EOL, (SRC_BUF))
extern int buf_append_fmt(buf_t* buf, const char* fmt, ...);
#define buf_append_vfmt(BUF, FMT, ARGS) buf_insert_vfmt((BUF), BUF_EOL, (FMT), (ARGS))
#define buf_append_s8(BUF, NUM) buf_insert_s8((BUF), BUF_EOL, (NUM))
#define buf_append_s16(BUF, NUM) buf_insert_s16((BUF), BUF_EOL, (NUM))
#define buf_append_s32(BUF, NUM) buf_insert_s32((BUF), BUF_EOL, (NUM))
#define buf_append_s64(BUF, NUM) buf_insert_s64((BUF), BUF_EOL, (NUM))
#define buf_append_u8(BUF, NUM) buf_insert_u8((BUF), BUF_EOL, (NUM))
#define buf_append_u16(BUF, NUM) buf_insert_u16((BUF), BUF_EOL, (NUM))
#define buf_append_u32(BUF, NUM) buf_insert_u32((BUF), BUF_EOL, (NUM))
#define buf_append_u64(BUF, NUM) buf_insert_u64((BUF), BUF_EOL, (NUM))
#define buf_append_hex(BUF, SRC, NUM) buf_insert_hex((BUF), BUF_EOL, SRC, (NUM))

/* assign: */
extern int buf_assign_mem(buf_t* buf, int len, const void* mem);
extern int buf_assign_char(buf_t* buf, char c);
extern int buf_assign_str(buf_t* buf, const void* str);
extern int buf_assign_buf(buf_t* buf, const buf_t* src_buf);
extern int buf_assign_fmt(buf_t* buf, const char* fmt, ...);
extern int buf_assign_vfmt(buf_t* buf, const char* fmt, va_list args);
extern int buf_assign_s8(buf_t* buf, s8 num);
extern int buf_assign_s16(buf_t* buf, s16 num);
extern int buf_assign_s32(buf_t* buf, s32 num);
extern int buf_assign_s64(buf_t* buf, s64 num);
#define buf_assign_u8(BUF, NUM) buf_assign_s8((BUF), (u8)(NUM))
#define buf_assign_u16(BUF, NUM) buf_assign_s16((BUF), (u16)(NUM))
#define buf_assign_u32(BUF, NUM) buf_assign_s32((BUF), (u32)(NUM))
#define buf_assign_u64(BUF, NUM) buf_assign_s64((BUF), (u64)(NUM))

/*
 * STRING/MEMORY HELPERS
 */
extern void buf_mem_copy(void* dest, const void* src, int len);
extern void buf_mem_shift(void* mem, int len, int shift);

#ifdef __cplusplus
}
#endif

#endif	/* BUF_H */
