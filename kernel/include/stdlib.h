#ifndef _STDLIB_H
#define _STDLIB_H
#include "os_mem.h"
#define malloc os_malloc
#define free os_free
#define calloc os_calloc
#define realloc os_realloc
#endif
