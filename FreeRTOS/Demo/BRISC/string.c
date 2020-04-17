#include <stddef.h>
#include <string.h>

void *memcpy(void *dest, const void *src, size_t num)
{
    char *cdest = (char *)dest;
    const char *csrc = (const char *)src;

    while (num-- > 0)
    {
        *cdest++ = *csrc++;
    }

    return dest;
}

void *memset(void *ptr, int value, size_t num)
{
    unsigned char *ucptr = (unsigned char *)ptr;
    const unsigned char ucvalue = (unsigned char)value;

    while (num-- > 0)
    {
        *ucptr++ = ucvalue;
    }

    return ptr;
}
