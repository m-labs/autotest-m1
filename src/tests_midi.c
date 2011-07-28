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
#include <irq.h>
#include <console.h>
#include <hw/midi.h>
#include <hw/interrupts.h>
#include "testdefs.h"

static int loopback()
{
	unsigned int c = 0;
	char e;
	int result = TEST_STATUS_PASSED;

	printf("press 'e' exit MIDI test\n");
	irq_ack(IRQ_MIDITX|IRQ_MIDIRX);
	while(1) {
		if (c == 256) {
			printf("0 ~ 255 sent out, press 'e' for exit\n");
			c = 0;
		}
		CSR_MIDI_RXTX = c;
		while(!(irq_pending() & IRQ_MIDITX)) {
			if(readchar_nonblock()) {
				e = readchar();
				if(e == 'e') return result;
			}
		}
		irq_ack(IRQ_MIDITX);

		if(!(irq_pending() & IRQ_MIDIRX)) {
			printf("Failed: RX receive problem\n");
			result = TEST_STATUS_FAILED;
		}

		if(CSR_MIDI_RXTX != c) {
			printf("Failed: TX: %d, but RX: %d\n", c, CSR_MIDI_RXTX);
			result = TEST_STATUS_FAILED;
		}
		irq_ack(IRQ_MIDIRX);
		c++;
	}
	
	return result;
}

struct test_description tests_midi[] = {
	{
		.name = "MIDI Loopback",
		.run = loopback
	},
	{
		.name = NULL
	}
};
