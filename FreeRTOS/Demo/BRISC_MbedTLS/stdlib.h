#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stddef.h>

#define free( x )			vPortFree( x )
#define malloc( x )			pvPortMalloc( x )
#define calloc( x, y )		pvPortMalloc( x * y )

extern void *pvPortMalloc( size_t xSize );
extern void vPortFree( void *pv );

int atoi (const char * str);

#endif /* __STDLIB_H__ */
