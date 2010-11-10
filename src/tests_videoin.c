/*
 * Milkymist One automated testing program
 * Copyright (C) 2010 Sebastien Bourdeauducq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <hw/vga.h>
#include <hw/bt656cap.h>
#include <hw/interrupts.h>
#include <irq.h>
#include <hal/vin.h>
#include <console.h>
#include <system.h>
#include "testdefs.h"

static int decoder_probe()
{
	int reg;
	
	vin_init();
	reg = vin_read_reg(0x11);
	if((reg == 0x11) || (reg == 0x13))
		return TEST_STATUS_PASSED;
	else
		return TEST_STATUS_FAILED;
}

/* Must be run after the VGA test to initialize the frambuffer! */
static short int fb[640*480] __attribute__((aligned(32)));
static short vbuffer[720*288] __attribute__((aligned(32)));
static int capture()
{
	int x, y;
	char c;
	
	CSR_VGA_BASEADDRESS = (unsigned int)fb;
	CSR_VGA_RESET = 0;

	for(x=0;x<640*480;x++)
		fb[x] = 0;
	for(x=0;x<720*288;x++)
		vbuffer[x] = 0;
	
	printf("Waiting for a captured frame. f to fail test, s to skip.\n");
	
	irq_ack(IRQ_VIDEOIN);
	CSR_BT656CAP_BASE = (unsigned int)vbuffer;
	CSR_BT656CAP_FILTERSTATUS = BT656CAP_FILTERSTATUS_FIELD1;
	while(!(irq_pending() & IRQ_VIDEOIN)) {
		if(readchar_nonblock()) {
			c = readchar();
			if(c == 'f')
				return TEST_STATUS_FAILED;
			if(c == 's')
				return TEST_STATUS_NOT_DONE;
		}
	}
	irq_ack(IRQ_VIDEOIN);
	while(!(irq_pending() & IRQ_VIDEOIN)) {
		if(readchar_nonblock()) {
			c = readchar();
			if(c == 'f')
				return TEST_STATUS_FAILED;
			if(c == 's')
				return TEST_STATUS_NOT_DONE;
		}
	}
	irq_ack(IRQ_VIDEOIN);
	CSR_BT656CAP_FILTERSTATUS = 0;
	while(CSR_BT656CAP_FILTERSTATUS & BT656CAP_FILTERSTATUS_INFRAME) {
		if(readchar_nonblock()) {
			c = readchar();
			if(c == 'f')
				return TEST_STATUS_FAILED;
			if(c == 's')
				return TEST_STATUS_NOT_DONE;
		}
	}
	flush_bridge_cache();
	for(y=0;y<288;y++)
		for(x=0;x<640;x++)
			fb[640*y+x] = vbuffer[720*y+x];
	flush_bridge_cache();

	printf("Is the picture OK? (y/n)\n");
	while(1) {
		c = readchar();
		switch(c) {
			case 'y':
				CSR_VGA_RESET = 1;
				return TEST_STATUS_PASSED;
			case 'n':
				CSR_VGA_RESET = 1;
				return TEST_STATUS_FAILED;
		}
	}
}

struct test_description tests_videoin[] = {
	{
		.name = "Decoder probe",
		.run = decoder_probe
	},
	{
		.name = "Capture",
		.run = capture
	},
	{
		.name = NULL
	}
};
