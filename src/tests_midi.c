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
#include <hw/midi.h>
#include <hw/interrupts.h>
#include "testdefs.h"

static int loopback()
{
	unsigned int c;

	irq_ack(IRQ_MIDITX|IRQ_MIDIRX);
	for(c=0;c<256;c++) {
		CSR_MIDI_RXTX = c;
		while(!(irq_pending() & IRQ_MIDITX));
		irq_ack(IRQ_MIDITX);
		
		if(!(irq_pending() & IRQ_MIDIRX)) return TEST_STATUS_FAILED;
		if(CSR_MIDI_RXTX != c) return TEST_STATUS_FAILED;
		irq_ack(IRQ_MIDIRX);
	}
	
	return TEST_STATUS_PASSED;
}

struct test_description tests_midi[] = {
	{
		.name = "Loopback",
		.run = loopback
	},
	{
		.name = NULL
	}
};
