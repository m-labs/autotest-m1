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
#include <hw/rc5.h>
#include <hw/interrupts.h>
#include "testdefs.h"

static int reception()
{
	char c;
	int rx;

	printf("Press the remote's \"power\" button...\n");
	printf("Waiting for remote. f to fail test, s to skip.\n");
	while(1) {
		while(!(irq_pending() & IRQ_IR)) {
			if(readchar_nonblock()) {
				c = readchar();
				if(c == 'f')
					return TEST_STATUS_FAILED;
				if(c == 's')
					return TEST_STATUS_NOT_DONE;
			}
		}
		rx = CSR_RC5_RX;
		irq_ack(IRQ_IR);
		if((rx & 0x003f) == 12)
			return TEST_STATUS_PASSED;
	}
}

struct test_description tests_ir[] = {
	{
		.name = "RC5 reception",
		.run = reception
	},
	{
		.name = NULL
	}
};
