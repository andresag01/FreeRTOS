        .text
        .globl _start
        .p2align 1
        .type _start,@function
_start:
		; Make a copy of the poiner to the device array
		rcopy r9, r0

        ; Initialize the stack
        newmwi r0, 4
        setwp r0

		; Set the compatibility flag for PANIC mode
		ldc r0, 3
		blr setCompatibilityFlag

		; Set the compatibility flag for INTERRUPT mode
		ldc r0, 2
		blr setCompatibilityFlag

		; Set the compatibility flag for EXCEPTION mode
		ldc r0, 1
		blr setCompatibilityFlag

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;;;;;; Set up the EXCEPTION context
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ldc r0, 1
        ;   1. Load the address of the exception handling function
        lda16pc r1, exceptionHandler
        ldc r2, 10
        setmreg r1, r2, r0
        ;   2. Allocate a stack and set wp
        newmwi r1, 8
        lda16pc r2, exitExceptionHandler
        stwfi r2, r1, 0
        ldawfi r1, r1, 4
        ldc r2, 12
        setmreg r1, r2, r0
        ;   3. Set the ep to current ep
        ldawep r1, 0
        ldc r2, 11
        setmreg r1, r2, r0

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;;;;;; Change current mode to INTERRUPT
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ldc r0, 2
        ;   1. Load the address of the main function
        lda16pc r1, _wrap_main
        ldc r2, 10
        setmreg r1, r2, r0
        ;   2. Allocate a stack and set wp.
        ;      NOTE: We havent implemented the compiler properly so it is hard
        ;      to write a function in C that returns from an interrupt without
        ;      using the __attribute__((naked)). So when an interrupt is raised
        ;      the code directly jumps to the assembly function
        ;      interruptHandler() that jumps to the appropriate C handler to
        ;      deal with the interrupt. The C function returns as normal using
        ;      the 'ret' instruction. For this to work, we manually set the
        ;      return address in the top-most stack to a trampolin bit of code
        ;      at the label 'exitInterruptHandler'
        newmwi r1, 8
        lda16pc r2, exitInterruptHandler
        stwfi r2, r1, 0
        ldawfi r1, r1, 4
        ldc r2, 12
        setmreg r1, r2, r0
        ;   3. Set the ep to current ep
        ldawep r1, 0
        ldc r2, 11
        setmreg r1, r2, r0
        ;   4. Load the pointer to device array in r0
        ldc r2, 0
        setmreg r9, r2, r0
        ;   5. Set the INTERRUPT mode bit
        ldc r1, 1
        setmode r1, r0
        ;   6. Unset the PANIC mode bit
        lda16pc r0, panicHandler
        eret r0

        ; Terminate the simulation (should never be reached!)
        ldc r0, 99
        halt r0
.Lfunc_end0:
        .size _start, .Lfunc_end0-_start
