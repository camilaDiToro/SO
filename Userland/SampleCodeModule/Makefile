include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:%.c=obj/%.o)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

ALL_OBJECTS= $(OBJECTS) $(OBJECTS_ASM)

all: $(MODULE)

$(MODULE): $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld $(ALL_OBJECTS) -o ../$(MODULE)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld  $(ALL_OBJECTS) -o ../0000-sampleCodeModule.elf --oformat=elf64-x86-64
#	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) $(OBJECTS_ASM) -o ../0000-sampleCodeModule.elf

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
