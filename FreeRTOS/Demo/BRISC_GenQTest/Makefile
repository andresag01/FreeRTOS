# Compiler and toolchain
LLVM_BASE_DIR = /home/andresag/Repos/llvm-project-beyondrisc/build/bin
CLANG         = $(LLVM_BASE_DIR)/clang
LLC           = $(LLVM_BASE_DIR)/llc
DISASSEMBLER  = $(LLVM_BASE_DIR)/llvm-dis

BRISC_ASM_DIR = /home/andresag/Repos/brisc-assembler
ASM           = $(BRISC_ASM_DIR)/assembler.py

INSTR_DB = /home/andresag/Repos/IHGC-Specifications/isa/instructions

# Compiler and toolchain flags
ARCH = cpu0
OLEVEL = 2

SYSROOT_DIR ?= /dir/that/does/not/exist
INCLUDES = -I. -I../../Source/include -I../Common/include -I../../Source/portable/BRISC

CFLAGS = -O$(OLEVEL) -emit-llvm -nostdlib -target $(ARCH) -ffreestanding \
		-fno-stack-check -fno-stack-protector $(INCLUDES) \
		-isystem $(SYSROOT_DIR) --sysroot $(SYSROOT_DIR)
LFLAGS = -relocation-model=pic -march=$(ARCH) -O$(OLEVEL) -filetype=asm
AFLAGS = --prog-handle 1       \
	--handle-bits 15           \
	--instructions $(INSTR_DB) \
	--entry $(ENTRY_FUNC)

# Source files
RTOS_SOURCE_DIR = ../../Source
DEMO_SOURCE_DIR = ../Common/Minimal

CSRCS = main.c 									\
	utils.c										\
	string.h									\
	$(DEMO_SOURCE_DIR)/GenQTest.c				\
	$(RTOS_SOURCE_DIR)/portable/BRISC/port.c	\
	$(RTOS_SOURCE_DIR)/portable/BRISC/heap.c	\
	$(RTOS_SOURCE_DIR)/tasks.c					\
	$(RTOS_SOURCE_DIR)/queue.c					\
	$(RTOS_SOURCE_DIR)/list.c
ASRCS = $(addsuffix .asm,$(basename $(CSRCS))) startup.asm
OUTPUT_BIN = main.bin
ENTRY_FUNC = _start

all: $(OUTPUT_BIN)

%.bin: $(ASRCS)
	@echo "  ASM    $^"
	@$(ASM) --output $@ --disassemble $*.disasm --input $^ $(AFLAGS)

%.asm: %.bc
	@echo "  LLC    $<"
	@$(LLC) $(LFLAGS) $< -o $@

%.bc: %.c
	@echo "  CLANG  $<"
	@$(CLANG) $(CFLAGS) -c $< -o $@
	@echo "  DASM   $*.bc"
	@$(DISASSEMBLER) $@

clean:
	@$(RM) $(foreach ext,bc ll disasm bin asm,$(addsuffix .$(ext),$(basename $(CSRCS))))

.PHONY: clean all
.PRECIOUS: %.bc %.asm %.ll %.disasm %.bin
