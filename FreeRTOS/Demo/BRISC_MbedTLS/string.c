#include <stddef.h>
#include <string.h>

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

__attribute__((naked)) void *memcpy(void *dest, const void *src, size_t num)
{
	__asm volatile(
		/* Extract src and dest byte alignment */
		"ldao r3, r0					\n"
		"extractbi r3, r3, 256			\n"
		"ldao r4, r1					\n"
		"extractbi r4, r4, 256			\n"

		/* Perform copy depending on whether the src and dest alignment matches */
		"eq r5, r3, r4					\n"
		"brf r5, memcpyUnaligned		\n"

		/* Copy assuming that src and dest byte alignment is equal */
		"memcpyAligned:					\n"
			/* Is the beginning word aligned? */
			"eqi r4, r3, 0				\n"
			"brt r4, wordAlignedMiddle1	\n"

			/* Copy bytes until the pointers are word aligned */
			"ldc r4, 4					\n"
			"subb r3, r4, r3			\n"
			"ldc r4, 0					\n"
			"wordUnalignedBegin:		\n"
				/* Finished? */
				"eq r5, r4, r2				\n"
				"brt r5, memcpyEnd			\n"
				/* Already aligned? */
				"eq r5, r4, r3				\n"
				"brt r5, wordAlignedMiddle0	\n"
				/* Copy the byte */
				"ld8u r5, r1, r4			\n"
				"st8 r5, r0, r4				\n"
				"addbi r4, r4, 1			\n"
				"bru wordUnalignedBegin		\n"

			/* Copy words until the pointers are word unaligned */
			"wordAlignedMiddle0:			\n"
			/* Align the pointers */
			"lda8 r0, r0, r4				\n"
			"lda8 r1, r1, r4				\n"
			"subb r2, r2, r4				\n"
			"wordAlignedMiddle1:			\n"
			"extractbi r3, r2, 2		\n"
			"memmove r3, r1, r0			\n"
            /* Increment the pointers by the copied amount */
            "extractbi r3, r2, 2        \n"
            "ldaw r0, r0, r3            \n"
            "ldaw r1, r1, r3            \n"
            /* Reduce the remaining num bytes to copy */
			"extractbi r2, r2, 256		\n"

			/* Copy bytes until the end */
			"wordUnalignedEnd:			\n"

		/* Copy assuming that src and dest byte alignment is different */
		"memcpyUnaligned:				\n"
		"eqi r3, r2, 0					\n"
		"brt r3, memcpyEnd				\n"

		"ldc r3, 0						\n"
		"memcpyUnalignedLoop:			\n"
		"ld8u r4, r1, r3				\n"
		"st8 r4, r0, r3					\n"
		"addbi r3, r3, 1				\n"
		"eq r4, r3, r2					\n"
		"brf r4, memcpyUnalignedLoop	\n"

		/* Exit the function */
		"memcpyEnd:						\n"
		"ret							\n"
	);
}

size_t strlen(const char *str)
{
    size_t len = 0;

    while (*str != '\0')
    {
        len++;
        str++;
    }

    return len;
}

int strncmp (const char *s1, const char *s2, size_t n)
{
	if (n == 0) return 0;

	while (n-- != 0 && *s1 == *s2)
    {
      if (n == 0 || *s1 == '\0') break;
      s1++;
      s2++;
    }

	return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

int memcmp(const void *m1, const void *m2, size_t n)
{
    unsigned char *s1 = (unsigned char *)m1;
    unsigned char *s2 = (unsigned char *)m2;

    while (n--)
    {
        if (*s1 != *s2)
        {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }

    return 0;
}

int strcmp (const char *s1, const char *s2)
{
    while (*s1 != '\0' && *s1 == *s2)
    {
      s1++;
      s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

char *strstr (const char *hs, const char *ne)
{
    size_t i;
    int c = ne[0];

    if (c == 0)
        return (char*)hs;

    for ( ; hs[0] != '\0'; hs++)
    {
        if (hs[0] != c) continue;

        for (i = 1; ne[i] != 0; i++)
	        if (hs[i] != ne[i])
	          break;

        if (ne[i] == '\0') return (char*)hs;
    }

    return NULL;
}

void *memmove(void *dst_void, const void *src_void, size_t length)
{
    char *dst = dst_void;
    const char *src = src_void;

    if (src < dst && dst < src + length)
    {
        /* Have to copy backwards */
        src += length;
        dst += length;
        while (length--)
        {
            *--dst = *--src;
        }
    }
    else
    {
        while (length--)
        {
            *dst++ = *src++;
        }
    }

    return dst_void;
}
