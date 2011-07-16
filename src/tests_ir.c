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
	int pre_rx,i,rx;
	int result;

	int rc5_code[3] = {0x01, 0x03, 0x05};

	char *code_strings[] = {
		"[1] passed",
		"[3] passed",
		"[5] passed",
	};

	pre_rx = 0;
	i = 0;
	result = TEST_STATUS_NOT_DONE;

	printf("Press the remote's [1] [3] [5], one by one\n");
	printf("Waiting for remote. e for exit\n");

	while(1) {
		while(!(irq_pending() & IRQ_IR)) {
			if(readchar_nonblock()) {
				c = readchar();
				if(c == 'e')
					return result;
			}
		}

		rx = CSR_RC5_RX;
		irq_ack(IRQ_IR);

		if(i == 3 || i == -1) { /* 3: all passed, -1: failed */
			printf("Now press 'e' exit Infrared test. rx [0x%04x]\n", rx);
			continue;
		}

		printf("  should be [0x%02x], get [0x%02x], [0x%04x].\n",
		       rc5_code[i], rx & 0x003f, rx);

		rx = rx & 0x003f;

		if(rx != pre_rx) {
			if(rx != rc5_code[i]) {
				result = TEST_STATUS_FAILED;
				i = -1;
				continue;
			}

			printf("%s\n", code_strings[i]);
			pre_rx = rx;
			i++;

			if(i == 3) {
				result = TEST_STATUS_PASSED;
				continue;
			}
		}
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
