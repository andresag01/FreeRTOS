#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Mode bit indexes from the mode register */

#define MODE_NORMAL_REGSET_INDEX		0
#define MODE_EXCEPTION_REGSET_INDEX		1
#define MODE_INTERRUPT_REGSET_INDEX		2
#define MODE_PANIC_REGSET_INDEX			3

/* Utilities and other necessary junk */

// Useful for stringifying macros
#define xstr(s) str(s)
#define str(s) #s

void setCompatibilityFlag(uint32_t mode);
__attribute__((noreturn)) void fail(void);
void panicHandler(uint32_t code, uint32_t inst);
void _wrap_main(void **devices);
uint32_t getProcessorMode(void);
void prvTaskSwitchContext(void);

#define IS_EXCEPTION_MODE_BIT_SET( x )	( ( x >> MODE_EXCEPTION_REGSET_INDEX ) & 1 )
#define IS_INTERRUPT_MODE_BIT_SET( x )  ( ( x >> MODE_INTERRUPT_REGSET_INDEX ) & 1 )

/* Exceptions */

void exceptionHandler(uint32_t code, uint32_t inst);

void unhandledExceptionHandler(uint32_t code, uint32_t inst);
void ecallExceptionHandler(uint32_t code, uint32_t inst,
						   uint32_t opcode, uint32_t im);

void exitExceptionHandler(void);

/* Interrupts */

void interruptHandler(uint32_t reason);

void unhandledInterruptHandler(uint32_t reason);
void TMR0InterruptHandler(uint32_t reason);
void UART0InterruptHandler(uint32_t reason);

void exitInterruptHandler(void);

/* Syscall identifiers */

#define SYSCALL_TASK_YIELD	0

/* Interrupt and I/O facilities */

#define DEVICE_COUNT 4

#define INTERRUPT_REASON_ICFG 0
#define INTERRUPT_REASON_ACTR 1
#define INTERRUPT_REASON_TMR0 2
#define INTERRUPT_REASON_UART0 3

#define ALLOCATION_COUNTERS_COUNT 64

typedef struct InterruptConfig
{
	uint32_t type;
	uint32_t global;
	uint32_t enbdev;
	uint32_t pendev;
} InterruptConfig_t;

typedef struct AllocationCounter
{
	uint32_t status;
	uint32_t maxmem;
	uint32_t curmem;
	uint32_t maxhan;
	uint32_t curhan;
} AllocationCounter_t;

typedef struct AllocationCounters
{
	uint32_t type;
	uint32_t global;
	AllocationCounter_t ctrs[ALLOCATION_COUNTERS_COUNT];
} AllocationCounters_t;

typedef struct Timer
{
	uint32_t type;
	uint32_t status;
	uint32_t reload;
	uint32_t curcnt;
} Timer_t;

typedef struct UART
{
	volatile uint32_t type;
	volatile uint32_t status;
	volatile uint32_t bufrx;
	volatile uint32_t buftx;
} UART_t;

extern void **io;
extern QueueHandle_t xRxedChars;
extern QueueHandle_t xCharsForTx;

/* Printing and debugging */

void printString(const char *s);
void printHex(uint32_t v);
void putchar(char c);

/* Pointer handling */

int isPointerAligned(void *p, uint32_t mask);
void *alignPointer(void *p, uint32_t mask);

/* Instruction identifiers for decoding during exception handling */

#define INSTRUCTION_ADDB 0
#define INSTRUCTION_ADDBI 1
#define INSTRUCTION_ADDS 2
#define INSTRUCTION_ADDSI 3
#define INSTRUCTION_AND 4
#define INSTRUCTION_ASHR 5
#define INSTRUCTION_BAU 6
#define INSTRUCTION_BAUT 7
#define INSTRUCTION_BCNT 8
#define INSTRUCTION_BITREV 9
#define INSTRUCTION_BLAU 10
#define INSTRUCTION_BLRB 11
#define INSTRUCTION_BLRF 12
#define INSTRUCTION_BRBF 13
#define INSTRUCTION_BRBT 14
#define INSTRUCTION_BRBU 15
#define INSTRUCTION_BRFF 16
#define INSTRUCTION_BRFT 17
#define INSTRUCTION_BRFU 18
#define INSTRUCTION_BRU 19
#define INSTRUCTION_BRUT 20
#define INSTRUCTION_BYTEREV 21
#define INSTRUCTION_CHCTR 22
#define INSTRUCTION_CLZ 23
#define INSTRUCTION_CSXD 24
#define INSTRUCTION_CSXW 25
#define INSTRUCTION_CSXWI 26
#define INSTRUCTION_CZXW 27
#define INSTRUCTION_CZXWI 28
#define INSTRUCTION_DIVS 29
#define INSTRUCTION_DIVU 30
#define INSTRUCTION_ECALLF 31
#define INSTRUCTION_ECALLFI 32
#define INSTRUCTION_ECALLT 33
#define INSTRUCTION_ECALLTI 34
#define INSTRUCTION_ECALLU 35
#define INSTRUCTION_ECALLUI 36
#define INSTRUCTION_ENTEP 37
#define INSTRUCTION_ENTWP 38
#define INSTRUCTION_EQ 40
#define INSTRUCTION_EQH 41
#define INSTRUCTION_EQI 42
#define INSTRUCTION_ERET 43
#define INSTRUCTION_EXTEP 44
#define INSTRUCTION_EXTRACTB 45
#define INSTRUCTION_EXTRACTBI 46
#define INSTRUCTION_EXTWP 47
#define INSTRUCTION_GETIM 48
#define INSTRUCTION_GETMODE 49
#define INSTRUCTION_GETMREG 50
#define INSTRUCTION_GETNIL 51
#define INSTRUCTION_GETOP 52
#define INSTRUCTION_GETOPC 53
#define INSTRUCTION_GETPRM 54
#define INSTRUCTION_GETSR 55
#define INSTRUCTION_HALT 56
#define INSTRUCTION_INSERTB 57
#define INSTRUCTION_INSERTBI 58
#define INSTRUCTION_ISEMPTY 59
#define INSTRUCTION_ISPTR 60
#define INSTRUCTION_LADDB 61
#define INSTRUCTION_LADDS 62
#define INSTRUCTION_LD16S 63
#define INSTRUCTION_LD8U 64
#define INSTRUCTION_LD8UI 65
#define INSTRUCTION_LDA16 66
#define INSTRUCTION_LDA16FI 67
#define INSTRUCTION_LDA16PCB 68
#define INSTRUCTION_LDA16PCF 69
#define INSTRUCTION_LDA8 70
#define INSTRUCTION_LDA8FI 71
#define INSTRUCTION_LDAB 72
#define INSTRUCTION_LDAO 73
#define INSTRUCTION_LDAW 74
#define INSTRUCTION_LDAWEP 75
#define INSTRUCTION_LDAWFI 76
#define INSTRUCTION_LDAWWP 77
#define INSTRUCTION_LDC 78
#define INSTRUCTION_LDIV 79
#define INSTRUCTION_LDOP 80
#define INSTRUCTION_LDSZ 81
#define INSTRUCTION_LDTAG 82
#define INSTRUCTION_LDW 83
#define INSTRUCTION_LDWEP 84
#define INSTRUCTION_LDWFI 85
#define INSTRUCTION_LDWP 86
#define INSTRUCTION_LDWWP 87
#define INSTRUCTION_LMUL 88
#define INSTRUCTION_LSHL 89
#define INSTRUCTION_LSHR 90
#define INSTRUCTION_LSP 91
#define INSTRUCTION_LSS 92
#define INSTRUCTION_LSU 93
#define INSTRUCTION_LSUBB 94
#define INSTRUCTION_LSUBS 95
#define INSTRUCTION_MACCS 96
#define INSTRUCTION_MACCU 97
#define INSTRUCTION_MEMMOVE 98
#define INSTRUCTION_MEMSET 99
#define INSTRUCTION_MULB 100
#define INSTRUCTION_MULS 101
#define INSTRUCTION_MULSI 102
#define INSTRUCTION_NEG 103
#define INSTRUCTION_NEWH 104
#define INSTRUCTION_NEWM8 105
#define INSTRUCTION_NEWMW 106
#define INSTRUCTION_NEWMWI 107
#define INSTRUCTION_NOT 108
#define INSTRUCTION_OR 109
#define INSTRUCTION_PUTCHR 111
#define INSTRUCTION_RCOPY 112
#define INSTRUCTION_REMS 113
#define INSTRUCTION_RESTOREREGS 114
#define INSTRUCTION_RET 115
#define INSTRUCTION_SAVEREGS 116
#define INSTRUCTION_SETEP 117
#define INSTRUCTION_SETMODE 118
#define INSTRUCTION_SETMREG 119
#define INSTRUCTION_SETOP 120
#define INSTRUCTION_SETPRM 121
#define INSTRUCTION_SETSR 122
#define INSTRUCTION_SETWP 123
#define INSTRUCTION_SHL 124
#define INSTRUCTION_SHLI 125
#define INSTRUCTION_ST16 126
#define INSTRUCTION_ST8 127
#define INSTRUCTION_ST8I 128
#define INSTRUCTION_STOP 129
#define INSTRUCTION_STTAG 130
#define INSTRUCTION_STTAGI 131
#define INSTRUCTION_STW 132
#define INSTRUCTION_STWEP 133
#define INSTRUCTION_STWFI 134
#define INSTRUCTION_STWWP 135
#define INSTRUCTION_SUBB 136
#define INSTRUCTION_SUBS 137
#define INSTRUCTION_SXD 138
#define INSTRUCTION_SXW 139
#define INSTRUCTION_SXWI 140
#define INSTRUCTION_WCNT 141
#define INSTRUCTION_XOR 142
#define INSTRUCTION_ZXW 143
#define INSTRUCTION_ZXWI 144
#define INSTRUCTION_NONE 145

#endif /* __UTILS_H__ */
