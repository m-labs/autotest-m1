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

#include <system.h>
#include <stdio.h>
#include "testdefs.h"

#define TEST_WORDS (1024*1024)

static int random()
{
	unsigned int test_buffer[TEST_WORDS];
	int i;
	unsigned int r;

	r = 0;
	for(i=0;i<TEST_WORDS;i++) {
		test_buffer[i] = r;
		r = 1664525*r + 1013904223;
	}
	flush_bridge_cache();
	r = 0;
	for(i=0;i<TEST_WORDS;i++) {
		if(test_buffer[i] != r)
			return TEST_STATUS_FAILED;
		r = 1664525*r + 1013904223;
	}
	return TEST_STATUS_PASSED;
}

static int hammer()
{
	unsigned int test_buffer[TEST_WORDS];
	int i;

	for(i=0;i<TEST_WORDS;i++)
		test_buffer[i] = i & 1 ? 0xffffffff : 0x00000000;
	flush_bridge_cache();
	for(i=0;i<TEST_WORDS;i++)
		if(test_buffer[i] != (i & 1 ? 0xffffffff : 0x00000000))
			return TEST_STATUS_FAILED;
	return TEST_STATUS_PASSED;
}

static int crosstalk()
{
	unsigned int test_buffer[TEST_WORDS];
	int i;

	for(i=0;i<TEST_WORDS;i++)
		test_buffer[i] = i & 1 ? 0xaaaaaaaa : 0x55555555;
	flush_bridge_cache();
	for(i=0;i<TEST_WORDS;i++)
		if(test_buffer[i] != (i & 1 ? 0xaaaaaaaa : 0x55555555))
			return TEST_STATUS_FAILED;
	return TEST_STATUS_PASSED;
}

struct test_description tests_sdram[] = {
	{
		.name = "Random pattern",
		.run = random
	},
	{
		.name = "Hammer pattern",
		.run = hammer
	},
	{
		.name = "Crosstalk pattern",
		.run = crosstalk
	},
	{
		.name = NULL
	}
};
