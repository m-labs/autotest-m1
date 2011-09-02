include $(MMDIR)/software/include.mak

SRCS    = $(wildcard *.c)
COBJS   = $(SRCS:.c=.o)
OBJECTS = crt0.o $(COBJS)

SEGMENTS= -j .text -j .data -j .rodata

LDFLAGS+= -T linker.ld -N
LDFLAGS+= -L$(MMDIR)/software/libbase -L$(MMDIR)/software/libmath -L$(MMDIR)/software/libnet -L$(MMDIR)/software/libhal
LIBS    = -lbase -lmath -lnet -lhal

all: boot.bin

%.bin: %.elf
	$(OBJCOPY) $(SEGMENTS) -O binary $< $@
	chmod -x $@
	$(MAKE) -C $(MMDIR)/tools
	$(MMDIR)/tools/mkmmimg $@

boot.elf: linker.ld $(OBJECTS)
	make -C ${MMDIR}/software/libbase
	make -C ${MMDIR}/software/libmath
	make -C ${MMDIR}/software/libnet
	make -C ${MMDIR}/software/libhal
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@  $(LIBS)
	chmod -x $@

load: boot.bin
	$(MMDIR)/tools/flterm --port /dev/ttyUSB0 --kernel boot.bin

.PHONY: clean depend load boot.crc.bin

boot.crc.bin: boot.bin
	IMAGES_DIR=$(IMGDIR) ./append_crc_len.sh

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	rm -f $(OBJECTS) boot.elf boot.bin boot.crc.bin .*~ *~ Makefile.bak
