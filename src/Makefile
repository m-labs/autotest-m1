include $(MMDIR)/software/include.mak

OBJECTS=crt0.o main.o
SEGMENTS=-j .text -j .data -j .rodata

all: boot.bin

%.bin: %.elf
	$(MAKE) -C $(MMDIR)/tools
	$(OBJCOPY) $(SEGMENTS) -O binary $< $@
	chmod -x $@
	$(MMDIR)/tools/crc32 $@

boot.elf: linker.ld $(OBJECTS)
	$(LD) $(LDFLAGS) -T linker.ld -N -o $@ -L$(MMDIR)/software/libbase --start-group $(OBJECTS) -lbase --end-group
	chmod -x $@

load: boot.bin
	$(MMDIR)/tools/flterm --port /dev/ttyUSB0 --kernel boot.bin

.PHONY: clean depend load

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	rm -f $(OBJECTS) boot.elf boot.bin .*~ *~ Makefile.bak
