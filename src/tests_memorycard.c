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
#include <blockdev.h>
#include "testdefs.h"

static int blkread()
{
	unsigned char buffer[512];
	unsigned short eos;
	
	if(!bd_init(BLOCKDEV_MEMORY_CARD)) {
		printf("Cannot intialize memory card\n");
		return TEST_STATUS_FAILED;
	}
	if(!bd_readblock(0, buffer)) {
		printf("Cannot read block\n");
		return TEST_STATUS_FAILED;
	}
	bd_done();
	eos = (((unsigned short)buffer[510]) << 8)|buffer[511];
	if(eos != 0x55aa) {
		printf("Unexpected end of block (%04x)\n", eos);
		return TEST_STATUS_FAILED;
	}
	return TEST_STATUS_PASSED;
}

struct test_description tests_memorycard[] = {
	{
		.name = "Block read",
		.run = blkread
	},
	{
		.name = NULL
	}
};
