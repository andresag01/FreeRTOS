# Compiler and toolchain
LLVM_BASE_DIR = /Users/andresag/Documents/repos/llvm-beyondrisc/build/bin
CLANG         = $(LLVM_BASE_DIR)/clang
LLC           = $(LLVM_BASE_DIR)/llc
DISASSEMBLER  = $(LLVM_BASE_DIR)/llvm-dis

BRISC_ASM_DIR = /Users/andresag/Documents/repos/brisc-assembler
ASM           = $(BRISC_ASM_DIR)/assembler.py

INSTR_DB = /Users/andresag/Documents/repos/sure-specification/isa/instructions

# Compiler and toolchain flags
ARCH = cpu0
OLEVEL = 2

CFLAGS = -O$(OLEVEL) -emit-llvm -nostdlib -target $(ARCH) -ffreestanding \
		-fno-stack-check -fno-stack-protector -I. \
		-I../../Source/include -I../Common/include
LFLAGS = -relocation-model=pic -march=$(ARCH) -O$(OLEVEL) -filetype=asm
AFLAGS = --prog-handle 1       \
	--handle-bits 16           \
	--instructions $(INSTR_DB) \
	--entry $(ENTRY_FUNC)

# Source files
RTOS_SOURCE_DIR = ../../Source
DEMO_SOURCE_DIR = ../Common/Minimal

CSRCS = main.c $(DEMO_SOURCE_DIR)/integer.c
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
	@$(RM) *.bc *.ll *.disasm *.bin $(addsuffix .asm,$(basename $(CSRCS)))

.PHONY: clean all
.PRECIOUS: %.bc %.asm %.ll %.disasm %.bin
