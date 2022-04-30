include Makefile.inc

KERNEL=kernel.bin

SOURCES=$(wildcard *.c)
SOURCES_INTERRUPTIONS=$(wildcard interruptions/*.c)
SOURCES_INTERRUPTIONS_ASM=$(wildcard interruptions/*.asm)
SOURCES_ASM=$(wildcard asm/*.asm)

OBJECTS=$(SOURCES:%.c=obj/%.o)
OBJECTS_INTERRUPTIONS=$(SOURCES_INTERRUPTIONS:interruptions/%.c=obj/%.o)
OBJECTS_INTERRUPTIONS_ASM=$(SOURCES_INTERRUPTIONS_ASM:interruptions/%.asm=obj/%.asm.o)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:%.asm=obj/%.asm.o)
STATICLIBS=

ALL_OBJECTS= $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_INTERRUPTIONS_ASM)

all: $(KERNEL)

$(KERNEL): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel.elf --oformat=elf64-x86-64 $^

obj/%.asm.o : interruptions/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

obj/%.o: interruptions/%.c
	$(GCC) $(GCCFLAGS) -I./include -I./interruptions -c $< -o $@

obj/%.asm.o : asm/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

obj/%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I./interruptions -c $< -o $@

$(LOADEROBJECT):
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf obj

.PHONY: all clean
