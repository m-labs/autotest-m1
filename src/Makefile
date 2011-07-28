include $(MMDIR)/software/include.mak

SRCS    = $(wildcard *.c)
COBJS   = $(SRCS:.c=.o)
OBJECTS = crt0.o $(COBJS)

SEGMENTS= -j .text -j .data -j .rodata

LDFLAGS+= -T linker.ld -N
LDFLAGS+= -L$(MMDIR)/software/libbase -L$(MMDIR)/software/libmath -L$(MMDIR)/software/libnet -L$(MMDIR)/software/libhal
LIBS    = -lbase -lmath -lnet -lhal -lbase

all: boot.bin

%.bin: %.elf
	$(OBJCOPY) $(SEGMENTS) -O binary $< $@
	chmod -x $@
	$(MAKE) -C $(MMDIR)/tools
	$(MMDIR)/tools/mkmmimg $@

boot.elf: linker.ld $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@  $(LIBS)
	chmod -x $@

load: boot.bin
	$(MMDIR)/tools/flterm --port /dev/ttyUSB0 --kernel boot.bin

.PHONY: clean depend load

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	rm -f $(OBJECTS) boot.elf boot.bin .*~ *~ Makefile.bak
