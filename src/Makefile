include $(MMDIR)/software/include.mak

SRCS    = $(wildcard *.c)
COBJS   = $(SRCS:.c=.o)
OBJECTS = crt0.o $(COBJS)

SEGMENTS= -j .text -j .data -j .rodata

LDFLAGS+= -T linker.ld -N
LDFLAGS+= -L$(MMDIR)/software/libbase -L$(MMDIR)/software/libmath -L$(MMDIR)/software/libnet -L$(MMDIR)/software/libhal
LDLIBS= -lbase -lmath -lnet -lhal
LIBS=$(MMDIR)/software/libhpdmc/libhpdmc.a $(MMDIR)/software/libbase/libbase.a \
     $(MMDIR)/software/libhal/libhal.a     $(MMDIR)/software/libnet/libnet.a

CFLAGS+= -DVERSION='"$(GIT_VERSION)$(GIT_STATUS)"'

GIT_VERSION:=$(shell git rev-parse HEAD | cut -c 1-7)
GIT_STATUS:=$(shell [ -z "`git status -s -uno`" ] || echo +)


all: boot.bin

%.bin: %.elf
	$(OBJCOPY) $(SEGMENTS) -O binary $< $@
	chmod -x $@
	$(MAKE) -C $(MMDIR)/tools
	$(MMDIR)/tools/mkmmimg $@

boot.elf: linker.ld $(OBJECTS) $(LIBS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)
	chmod -x $@

load: boot.bin
	make -C $(MMDIR)/tools/
	$(MMDIR)/tools/flterm --port /dev/ttyUSB0 --kernel boot.bin

$(MMDIR)/software/libhpdmc/libhpdmc.a:
	make -C $(MMDIR)/software/libhpdmc/

$(MMDIR)/software/libbase/libbase.a:
	make -C $(MMDIR)/software/libbase/

$(MMDIR)/software/libhal/libhal.a:
	make -C $(MMDIR)/software/libhal/

$(MMDIR)/software/libnet/libnet.a:
	make -C $(MMDIR)/software/libnet/

.PHONY: clean depend load boot.crc.bin $(LIBS)

boot.crc.bin: boot.bin
	IMAGES_DIR=$(IMAGES_DIR) ./append_crc_len.sh

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	rm -f $(OBJECTS) boot.elf boot.bin boot.crc.*.bin .*~ *~ Makefile.bak
