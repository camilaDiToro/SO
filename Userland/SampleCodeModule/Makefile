include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
MODULE_ELF=$(MODULE:%.bin=%.elf)

SOURCES=$(wildcard [^_]*.c)
SOURCES_LOADER=$(wildcard _*.c)
SOURCES_ASM=$(wildcard asm/*.asm)

OBJECTS=$(SOURCES:%.c=obj/%.o)
OBJECTS_LOADER=$(SOURCES_LOADER:%.c=obj/%.o)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

ALL_OBJECTS=$(OBJECTS) $(OBJECTS_ASM)

all: $(MODULE)

$(MODULE): $(ALL_OBJECTS) $(OBJECTS_LOADER)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld $(OBJECTS_LOADER) $(ALL_OBJECTS) -o ../$(MODULE)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld $(OBJECTS_LOADER) $(ALL_OBJECTS) -o ../$(MODULE_ELF) --oformat=elf64-x86-64

obj/%.o: %.c
	mkdir -p obj
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

obj/%.asm.o : asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf obj
	rm *.elf *.bin | true

.PHONY: all clean print
