#ifndef TYPE_H
#define TYPE_H

#define  U8VALUE_INVALID   0xFF
#define U16VALUE_INVALID 0xFFFF

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define	SIZE_OF_INT (4)
#define	SCNld   "d"
#define	SCNlX   "X"

#define     ON              1
#define     OFF             0

#ifndef __cplusplus
	#ifndef     true
	#define     true        1
	#endif

	#ifndef     false
	#define     false       0
	#endif

	#ifndef NULL
	#define NULL            ((void *)0)
	#endif
#else
    // #error "__cplusplus"
	#ifndef NULL
	#define NULL            0
	#endif
#endif
#ifndef FALSE
    #define FALSE           (0)
#endif

#ifndef TRUE
    #define TRUE            (1)
#endif

typedef char                ascii;
typedef unsigned char       byte;

typedef signed char         s8;
typedef int16_t             s16;
typedef int32_t             s32;
typedef int64_t             s64;

typedef unsigned char       u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;

#define      U8_MAX        0xFF
#define      U16_MAX       0xFFFF
#define      U32_MAX       0xFFFFFFFF

// periphery physical adress
typedef uint64_t            addr_t;
#define	ADDR_PRN            "%llx"
// #define	ADDR_PRN            "0x%" PRIx64

#ifndef KB
	#define KB 1024
#endif

#endif // ! TYPE_H
