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
#include <net/mdio.h>
#include <net/microudp.h>
#include "testdefs.h"

static int mdio()
{
	int r;
	r = mdio_read(0x01, 0x02);
	if(r == 0x0022)
		return TEST_STATUS_PASSED;
	else {
		printf("Unexpected PHY ID: %04x\n", r);
		return TEST_STATUS_FAILED;
	}
}

static char macaddr[6] = {0xf8, 0x71, 0xfe, 0x01, 0x02, 0x03};
static int arp_resolution()
{
	int ethbuf[MICROUDP_BUFSIZE/4];
	
	microudp_start(macaddr, IPTOINT(192, 168, 0, 42), ethbuf);
	if(!microudp_arp_resolve(IPTOINT(192, 168, 0, 14)))
		return TEST_STATUS_FAILED;
	microudp_shutdown();
	return TEST_STATUS_PASSED;
}

struct test_description tests_ethernet[] = {
	{
		.name = "MDIO",
		.run = mdio
	},
	{
		.name = "ARP resolution",
		.run = arp_resolution
	},
	{
		.name = NULL
	}
};
