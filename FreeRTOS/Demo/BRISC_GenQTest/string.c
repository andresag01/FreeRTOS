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
