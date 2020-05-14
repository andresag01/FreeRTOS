#include <stdint.h>

#include "utils.h"
#include "queue.h"

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

__attribute__((naked)) uint32_t getModePc(uint32_t mode)
{
	__asm volatile (
		"ldc		r1, 10		\n"
		"getmreg	r0, r1, r0	\n"
		"ldao		r0, r0		\n"
		"ret					\n"
	);
}

//////////////////////////////////////////////////////////////////////////////
//// PANIC HANDLERS
//////////////////////////////////////////////////////////////////////////////

void panicHandler(uint32_t code, uint32_t inst)
{
	uint32_t mode = getProcessorMode();

	printString("PANIC: ");
	if (IS_INTERRUPT_MODE_BIT_SET(mode))
	{
		printString("mode:INTERRUPT code:");
		printHex(code);
		printString(" inst:");
		printHex(inst);
		printString(" npc");
		printHex(getModePc(2));
	}
	else if (IS_EXCEPTION_MODE_BIT_SET(mode))
	{
		printString("mode:EXCEPTION code:");
		printHex(code);
		printString(" inst:");
		printHex(inst);
		printString(" npc");
		printHex(getModePc(1));
	}
	else
	{
		printString("Do not know what mode generated interrupt");
	}

	putchar('\n');

	fail();
    __asm volatile(
		"rcopy		r4, r1		\n"

		/* Print the reason code */
		"blr		printHex	\n"

		/* Print space */
		"ldc		r0, 32		\n"
		"blr		putchar		\n"

		/* Print instruction */
		"rcopy		r0, r4		\n"
		"blr		printHex	\n"

		/* Print space */
		"ldc		r0, 32		\n"
		"blr		putchar		\n"

		/* Print pc+1 address */
		"blr		printHex	\n"

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
			printString("ECALLFI\n");
			fail();
			break;

		case INSTRUCTION_ECALLF:
			printString("ECALLF\n");
			fail();
			break;

		case INSTRUCTION_ECALLTI:
			printString("ECALLTI\n");
			fail();
			break;

		case INSTRUCTION_ECALLT:
			printString("ECALLT\n");
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
				printString("Unknown syscall: ");
				printHex(im);
				printString("\n");
				fail();
			}
			break;

		case INSTRUCTION_ECALLU:
			printString("ECALLU\n");
			fail();
			break;

		default:
			printString("default ecall exception\n");
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
		/* Print the reason code */
		"blr		printHex	\n"

		/* Print space */
		"ldc		r0, 32		\n"
		"blr		putchar		\n"

		/* Print pc+1 address */
		"ldc		r0, 10		\n"
		"ldc		r1, 0		\n"
		"getmreg	r0, r0, r1	\n"
		"ldao		r0, r0		\n"
		"blr		printHex	\n"

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
		"			 TMR0InterruptHandler,      "
		"			 UART0InterruptHandler    \n"
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

	// This handler is expected to change the current context:
	//	1. Abort if incrementing the RTOS tick gives TRUE
	//	2. Abort if an exception is being handled
	//	3. Switch context

	if( xTaskIncrementTick() != pdFALSE )
	{
		portYIELD_FROM_ISR();
	}

    // Clear interrupt flag
    icfg->pendev = icfg->pendev & ~(1 << INTERRUPT_REASON_TMR0);
}

void UART0InterruptHandler(uint32_t reason)
{
	volatile InterruptConfig_t *icfg =
		(InterruptConfig_t *)io[INTERRUPT_REASON_ICFG];
	volatile UART_t *uart = (UART_t *)io[INTERRUPT_REASON_UART0];

	signed char cChar;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// What caused the interrupt
	uint32_t ulStatus = (uart->status >> 4) & 0x3;

	printString("UART handler\n");

	// Was a character transferred?
	if (ulStatus & 2)
	{
		printString("UART intTx\n");
		if (xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			uart->buftx = cChar;
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt */
			uart->status &= ~(1 << 3);
		}
	}

	// Was a character received?
	if (ulStatus & 1)
	{
		printString("UART intRx\n");
		cChar = uart->bufrx;

		xQueueSendFromISR(xRxedChars, &cChar, &xHigherPriorityTaskWoken);
	}

	// Acknoledge the interrupt
	icfg->pendev = icfg->pendev & ~(1 << INTERRUPT_REASON_UART0);

	/* If an event caused a task to unblock then we call "Yield from ISR" to
	ensure that the unblocked task is the task that executes when the interrupt
	completes if the unblocked task has a priority higher than the interrupted
	task. */
	if (xHigherPriorityTaskWoken)
	{
		portYIELD_FROM_ISR();
	}
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
