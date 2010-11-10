include $(MMDIR)/software/include.mak

OBJECTS=crt0.o main.o tests_sdram.o tests_gpio.o tests_vga.o tests_memorycard.o tests_audio.o tests_ethernet.o tests_videoin.o tests_midi.o tests_dmx.o tests_ir.o tests_usb.o
SEGMENTS=-j .text -j .data -j .rodata

all: boot.bin

%.bin: %.elf
	$(MAKE) -C $(MMDIR)/tools
	$(OBJCOPY) $(SEGMENTS) -O binary $< $@
	chmod -x $@
	$(MMDIR)/tools/mkmmimg $@

boot.elf: linker.ld $(OBJECTS)
	$(LD) $(LDFLAGS) -T linker.ld -N -o $@ -L$(MMDIR)/software/libbase -L$(MMDIR)/software/libmath -L$(MMDIR)/software/libnet --start-group $(OBJECTS) -lbase -lmath -lnet --end-group
	chmod -x $@

load: boot.bin
	$(MMDIR)/tools/flterm --port /dev/ttyUSB0 --kernel boot.bin

.PHONY: clean depend load

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	rm -f $(OBJECTS) boot.elf boot.bin .*~ *~ Makefile.bak
