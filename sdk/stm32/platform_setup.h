#ifndef  PLATFORM_SETUP_H
#define  PLATFORM_SETUP_H

#include "type.h"
#include "util.h"
#include "hardware.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACK __attribute__((packed))
#define PACK_BEGIN
#define PACK_END

#define PLATFORM_NAME        "STM32xx"

#define NL "\r\n"

#define STRINGYFY(s) #s
#define FILELINE(line) __FILE__ ":" STRINGYFY(line)
#define ASSERT(p,msg) if(!(p)){ printf("ASSERT: " FILELINE(__LINE__) " " msg NL); while(1);}

#endif //  PLATFORM_SETUP_H
