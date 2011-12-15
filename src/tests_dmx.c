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
#include <hw/dmx.h>
#include <hw/sysctl.h>
#include "testdefs.h"

static int loopback(void)
{
	unsigned int i;
	
	for(i=0;i<512;i++)
		CSR_DMX_TX(i) = i;
	CSR_DMX_THRU = 0;
	
	/* wait for all channels to be transmitted */
	CSR_TIMER0_COUNTER = 0;
	CSR_TIMER0_COMPARE = 0xffffffff;
	CSR_TIMER0_CONTROL = TIMER_ENABLE;
	while(CSR_TIMER0_COUNTER < 3787878);
	CSR_TIMER0_CONTROL = 0;
	
	for(i=0;i<512;i++)
		if(CSR_DMX_RX(i) != (i & 0xff))
			return TEST_STATUS_FAILED;
	
	return TEST_STATUS_PASSED;
}

struct test_description tests_dmx[] = {
	{
		.name = "DMX512 Loopback",
		.run = loopback
	},
	{
		.name = NULL
	}
};
