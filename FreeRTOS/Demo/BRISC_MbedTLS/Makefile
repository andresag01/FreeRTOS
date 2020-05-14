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
INCLUDES = -I. -I../../Source/include -I../../Source/portable/BRISC -I./mbedtls/include

CFLAGS = -O$(OLEVEL) -emit-llvm -nostdlib -target $(ARCH) -ffreestanding \
		-fno-stack-check -fno-stack-protector $(INCLUDES) \
		-isystem $(SYSROOT_DIR) --sysroot $(SYSROOT_DIR) \
		-DMBEDTLS_CONFIG_FILE=\"user_config.h\"
LFLAGS = -relocation-model=pic -march=$(ARCH) -O$(OLEVEL) -filetype=asm
AFLAGS = --prog-handle 1       \
	--handle-bits 15           \
	--instructions $(INSTR_DB) \
	--entry $(ENTRY_FUNC)

# Source files
RTOS_SOURCE_DIR = ../../Source
MBEDTLS_SOURCE_DIR = ./mbedtls/library

CSRCS = main.c 									\
	utils.c										\
	string.c									\
	serial.c									\
	$(RTOS_SOURCE_DIR)/portable/BRISC/port.c	\
	$(RTOS_SOURCE_DIR)/portable/BRISC/heap.c	\
	$(RTOS_SOURCE_DIR)/tasks.c					\
	$(RTOS_SOURCE_DIR)/queue.c					\
	$(RTOS_SOURCE_DIR)/list.c					\
	$(MBEDTLS_SOURCE_DIR)/aes.c                 \
	$(MBEDTLS_SOURCE_DIR)/aesni.c               \
	$(MBEDTLS_SOURCE_DIR)/arc4.c                \
	$(MBEDTLS_SOURCE_DIR)/aria.c                \
	$(MBEDTLS_SOURCE_DIR)/asn1parse.c           \
	$(MBEDTLS_SOURCE_DIR)/asn1write.c           \
	$(MBEDTLS_SOURCE_DIR)/base64.c              \
	$(MBEDTLS_SOURCE_DIR)/bignum.c              \
	$(MBEDTLS_SOURCE_DIR)/blowfish.c            \
	$(MBEDTLS_SOURCE_DIR)/camellia.c            \
	$(MBEDTLS_SOURCE_DIR)/ccm.c                 \
	$(MBEDTLS_SOURCE_DIR)/certs.c               \
	$(MBEDTLS_SOURCE_DIR)/chacha20.c            \
	$(MBEDTLS_SOURCE_DIR)/chachapoly.c          \
	$(MBEDTLS_SOURCE_DIR)/cipher.c              \
	$(MBEDTLS_SOURCE_DIR)/cipher_wrap.c         \
	$(MBEDTLS_SOURCE_DIR)/cmac.c                \
	$(MBEDTLS_SOURCE_DIR)/ctr_drbg.c            \
	$(MBEDTLS_SOURCE_DIR)/debug.c               \
	$(MBEDTLS_SOURCE_DIR)/des.c                 \
	$(MBEDTLS_SOURCE_DIR)/dhm.c                 \
	$(MBEDTLS_SOURCE_DIR)/ecdh.c                \
	$(MBEDTLS_SOURCE_DIR)/ecdsa.c               \
	$(MBEDTLS_SOURCE_DIR)/ecjpake.c             \
	$(MBEDTLS_SOURCE_DIR)/ecp.c                 \
	$(MBEDTLS_SOURCE_DIR)/ecp_curves.c          \
	$(MBEDTLS_SOURCE_DIR)/entropy.c             \
	$(MBEDTLS_SOURCE_DIR)/entropy_poll.c        \
	$(MBEDTLS_SOURCE_DIR)/error.c               \
	$(MBEDTLS_SOURCE_DIR)/gcm.c                 \
	$(MBEDTLS_SOURCE_DIR)/havege.c              \
	$(MBEDTLS_SOURCE_DIR)/hkdf.c                \
	$(MBEDTLS_SOURCE_DIR)/hmac_drbg.c           \
	$(MBEDTLS_SOURCE_DIR)/md.c                  \
	$(MBEDTLS_SOURCE_DIR)/md2.c                 \
	$(MBEDTLS_SOURCE_DIR)/md4.c                 \
	$(MBEDTLS_SOURCE_DIR)/md5.c                 \
	$(MBEDTLS_SOURCE_DIR)/md_wrap.c             \
	$(MBEDTLS_SOURCE_DIR)/memory_buffer_alloc.c \
	$(MBEDTLS_SOURCE_DIR)/nist_kw.c             \
	$(MBEDTLS_SOURCE_DIR)/oid.c                 \
	$(MBEDTLS_SOURCE_DIR)/padlock.c             \
	$(MBEDTLS_SOURCE_DIR)/pem.c                 \
	$(MBEDTLS_SOURCE_DIR)/pk.c                  \
	$(MBEDTLS_SOURCE_DIR)/pk_wrap.c             \
	$(MBEDTLS_SOURCE_DIR)/pkcs11.c              \
	$(MBEDTLS_SOURCE_DIR)/pkcs12.c              \
	$(MBEDTLS_SOURCE_DIR)/pkcs5.c               \
	$(MBEDTLS_SOURCE_DIR)/pkparse.c             \
	$(MBEDTLS_SOURCE_DIR)/pkwrite.c             \
	$(MBEDTLS_SOURCE_DIR)/platform.c            \
	$(MBEDTLS_SOURCE_DIR)/platform_util.c       \
	$(MBEDTLS_SOURCE_DIR)/poly1305.c            \
	$(MBEDTLS_SOURCE_DIR)/ripemd160.c           \
	$(MBEDTLS_SOURCE_DIR)/rsa.c                 \
	$(MBEDTLS_SOURCE_DIR)/rsa_internal.c        \
	$(MBEDTLS_SOURCE_DIR)/sha1.c                \
	$(MBEDTLS_SOURCE_DIR)/sha256.c              \
	$(MBEDTLS_SOURCE_DIR)/sha512.c              \
	$(MBEDTLS_SOURCE_DIR)/ssl_cache.c           \
	$(MBEDTLS_SOURCE_DIR)/ssl_ciphersuites.c    \
	$(MBEDTLS_SOURCE_DIR)/ssl_cli.c             \
	$(MBEDTLS_SOURCE_DIR)/ssl_cookie.c          \
	$(MBEDTLS_SOURCE_DIR)/ssl_srv.c             \
	$(MBEDTLS_SOURCE_DIR)/ssl_ticket.c          \
	$(MBEDTLS_SOURCE_DIR)/ssl_tls.c             \
	$(MBEDTLS_SOURCE_DIR)/threading.c           \
	$(MBEDTLS_SOURCE_DIR)/timing.c              \
	$(MBEDTLS_SOURCE_DIR)/version.c             \
	$(MBEDTLS_SOURCE_DIR)/version_features.c    \
	$(MBEDTLS_SOURCE_DIR)/x509.c                \
	$(MBEDTLS_SOURCE_DIR)/x509_create.c         \
	$(MBEDTLS_SOURCE_DIR)/x509_crl.c            \
	$(MBEDTLS_SOURCE_DIR)/x509_crt.c            \
	$(MBEDTLS_SOURCE_DIR)/x509_csr.c            \
	$(MBEDTLS_SOURCE_DIR)/x509write_crt.c       \
	$(MBEDTLS_SOURCE_DIR)/x509write_csr.c       \
	$(MBEDTLS_SOURCE_DIR)/xtea.c

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
