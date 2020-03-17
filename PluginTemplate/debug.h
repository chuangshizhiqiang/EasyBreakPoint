#pragma once

#define __CSZQDEBUG 0
#if __CSZQDEBUG > 0
#define DBGPRINT(format , ...)   printf(format , ##__VA_ARGS__);
#define PRINTDBG printf("[DEBUG]%s:%d\r\n" , __FUNCTION__, __LINE__);
#else
#define DBGPRINT(format, ...)
#define PRINTDBG
#endif