#include <stdint.h>

#include "utils.h"

void **io;

__attribute__((naked)) void setCompatibilityFlag(uint32_t mode)
{
	__asm volatile(
		"ldc        r1, 13      \n"
		"getmreg    r2, r1, r0  \n"
		"ldc        r3, 1       \n"
		"insertbi   r2, r3, 134 \n"
		"setmreg    r2, r1, r0  \n"
		"ret                    \n"
	);
}

__attribute__((naked)) void fail()
{
	__asm volatile(
		"ecallui	99 			\n"
	);
}

__attribute__((naked)) void _wrap_main(void **devices)
{
	// This function is a simple wrapper around the program's main function
	// that does three things:
	//		1. Writes the device array into the io global variable
	//		2. Branches to main
	//		3. Sets the interrupt handler address once main terminates
	__asm volatile(
		"stwep  r0, io					\n"
		"ldc    r0, 0					\n"
		"bru    main					\n"
	);
}

__attribute__((naked)) uint32_t getProcessorMode(void)
{
	__asm volatile(
		"getmode r0						\n"
		"ret							\n"
	);
}

__attribute__((naked)) void *alignPointer(void *p, uint32_t mask)
{
	__asm volatile(
		/* Get the pointer offset */
		"ldao r2, r0		\n"

		/* Align the pointer offset as required */
		"not r1, r1			\n"
		"and r2, r2, r1		\n"

		/* Replace the offset in the pointer */
		"ldab r0, r0		\n"
		"lda8 r0, r0, r2	\n"

		/* Finish! */
		"ret				\n"
	);
}

__attribute__((naked)) int isPointerAligned(void *p, uint32_t mask)
{
	__asm volatile(
		/* Get the pointer offset */
		"ldao r0, r0		\n"

		/* Mask out the byte offset */
		"and r0, r0, r1		\n"

		/* Check whether it is aligned */
		"eqi r0, r0, 0		\n"

		/* Return */
		"ret				\n"
	);
}

//////////////////////////////////////////////////////////////////////////////
//// PANIC HANDLERS
//////////////////////////////////////////////////////////////////////////////

__attribute__((naked)) void panicHandler(uint32_t code, uint32_t inst)
{
    __asm volatile(
        "bru fail				\n"
    );
}

//////////////////////////////////////////////////////////////////////////////
//// EXCEPTION HANDLERS
//////////////////////////////////////////////////////////////////////////////

__attribute__((naked)) void exceptionHandler(uint32_t code, uint32_t inst)
{
	__asm volatile(
		"getopc r2, r1							\n"
		"getim r3, r1							\n"
		"brut r0								\n"
		".jump_table unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 unhandledExceptionHandler,	"
		"			 ecallExceptionHandler,	   	"
		"			 unhandledExceptionHandler	\n"
	);
}

void ecallExceptionHandler(uint32_t code, uint32_t inst,
						   uint32_t opcode, uint32_t im)
{
	static uint32_t i = 0;

	switch (opcode)
	{
		case INSTRUCTION_ECALLFI:
			fail();
			break;

		case INSTRUCTION_ECALLF:
			fail();
			break;

		case INSTRUCTION_ECALLTI:
			fail();
			break;

		case INSTRUCTION_ECALLT:
			fail();
			break;

		case INSTRUCTION_ECALLUI:
			if (im == SYSCALL_TASK_YIELD)
			{
				vPortEnterCritical();
				prvTaskSwitchContext();
				vPortExitCritical();
			}
			else
			{
				fail();
			}
			break;

		case INSTRUCTION_ECALLU:
			fail();
			break;

		default:
			fail();
			break;
	}
}

__attribute__((naked)) void exitExceptionHandler(void)
{
	__asm volatile(
		"lda16pc r0, exceptionHandler	\n"
		"eret r0						\n"
	);
}

__attribute__((naked)) void unhandledExceptionHandler(uint32_t code, uint32_t inst)
{
	__asm volatile(
		"bru fail				\n"
	);
}

//////////////////////////////////////////////////////////////////////////////
//// INTERRUPT HANDLERS
//////////////////////////////////////////////////////////////////////////////

__attribute__((naked)) void interruptHandler(uint32_t reason)
{
	__asm volatile(
		"brut r0								\n"
		".jump_table unhandledInterruptHandler,	"
		"			 unhandledInterruptHandler,	"
		"			 TMR0InterruptHandler		\n"
	);
}

__attribute__((naked)) void exitInterruptHandler(void)
{
	__asm volatile(
		"lda16pc r0, interruptHandler	\n"
		"eret r0						\n"
	);
}

void TMR0InterruptHandler(uint32_t reason)
{
    volatile InterruptConfig_t *icfg =
		(InterruptConfig_t *)io[INTERRUPT_REASON_ICFG];
	uint32_t mode;

	// This handler is expected to change the current context:
	//	1. Abort if incrementing the RTOS tick gives TRUE
	//	2. Abort if an exception is being handled
	//	3. Switch context

	if( xTaskIncrementTick() != pdFALSE )
	{
		mode = getProcessorMode();

		if( !IS_EXCEPTION_MODE_BIT_SET( mode ) )
		{
			/*
			 * No need to disable interrupts here because nothing can interrupt
			 * this mode!
			 */
			prvTaskSwitchContext();
		}
	}

    // Clear interrupt flag
    icfg->pendev = icfg->pendev & ~(1 << INTERRUPT_REASON_TMR0);
}

__attribute__((naked)) void unhandledInterruptHandler(uint32_t reason)
{
	__asm volatile(
		"bru fail				\n"
	);
}

//////////////////////////////////////////////////////////////////////////////
//// PRINTING
//////////////////////////////////////////////////////////////////////////////

__attribute__((naked)) void putchar(char c)
{
    __asm volatile(
        "putchr  r0				\n"
        "ret					\n"
    );
}

void printString(const char *s)
{
	while (*s != '\0')
	{
		putchar(*s++);
	}
}

void printHex(uint32_t v)
{
#define GET_BYTE(v, o)						\
	do {									\
		uint8_t lo = (v >> (o + 0)) & 0xf;  \
		uint8_t hi = (v >> (o + 4)) & 0xf;  \
											\
		if (hi < 10)						\
			putchar(hi + '0');				\
		else								\
			putchar(hi + 'a' - 10);			\
											\
		if (lo < 10)						\
			putchar(lo + '0');				\
		else								\
			putchar(lo + 'a' - 10);			\
	} while (0)

	GET_BYTE(v, 24);
	GET_BYTE(v, 16);
	GET_BYTE(v, 8);
	GET_BYTE(v, 0);
}
