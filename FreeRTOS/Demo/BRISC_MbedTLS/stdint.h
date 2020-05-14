#ifndef __STDINT_H__
#define __STDINT_H__

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

typedef unsigned long long int uint64_t;
typedef signed long long int int64_t;

#define SIZE_MAX 0xffffffff

#endif /* __STDINT_H__ */
