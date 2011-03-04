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

static void flush_caches()
{
	asm volatile(
		"wcsr DCC, r0\n"
		"nop\n"
	);
	flush_bridge_cache();
}

static int a8()
{
	unsigned int adr1, adr2;
	int j;

	for(j=0;j<2000;j++) {
		adr1 = 0x40000000 + 0x1400000;
		adr2 = 0x40000000 + 0x1000000;
		*((unsigned int *)adr1) = 0x2991fe76;
		*((unsigned int *)adr2) = 0x860eab97;
		flush_caches();
		if(*((unsigned int *)adr1) != 0x2991fe76) return TEST_STATUS_FAILED;
		if(*((unsigned int *)adr2) != 0x860eab97) return TEST_STATUS_FAILED;
	}
	return TEST_STATUS_PASSED;
}

static int test_a(unsigned int a)
{
	unsigned int adr1, adr2;
	int j;
	
	for(j=0;j<2000;j++) {
		adr1 = 0x40000000 + 0x400000;
		adr2 = adr1 | ((1024*4*4) << a);
		*((unsigned int *)adr1) = (0x3a499876+a);
		*((unsigned int *)adr2) = (0x08af8401+a);
		flush_caches();
		if(*((unsigned int *)adr1) != (0x3a499876+a)) return TEST_STATUS_FAILED;
		if(*((unsigned int *)adr2) != (0x08af8401+a)) return TEST_STATUS_FAILED;
	}
	return TEST_STATUS_PASSED;
}

static int a9() { return test_a(9); }
static int a10() { return test_a(10); }
static int a11() { return test_a(11); }
static int a12() { return test_a(12); }

#define TEST_WORDS (512*1024)

static int random()
{
	unsigned int test_buffer[TEST_WORDS];
	int i, j;
	unsigned int r;

	for(j=0;j<15;j++) {
		r = 0;
		for(i=0;i<TEST_WORDS;i++) {
			test_buffer[i] = r;
			r = 1664525*r + 1013904223;
		}
		flush_caches();
		r = 0;
		for(i=0;i<TEST_WORDS;i++) {
			if(test_buffer[i] != r)
				return TEST_STATUS_FAILED;
			r = 1664525*r + 1013904223;
		}
	}
	return TEST_STATUS_PASSED;
}

static int hammer()
{
	unsigned int test_buffer[TEST_WORDS];
	int i, j;

	for(j=0;j<15;j++) {
		for(i=0;i<TEST_WORDS;i++)
			test_buffer[i] = i & 1 ? 0xffffffff : 0x00000000;
		flush_caches();
		for(i=0;i<TEST_WORDS;i++)
			if(test_buffer[i] != (i & 1 ? 0xffffffff : 0x00000000))
				return TEST_STATUS_FAILED;
	}
	return TEST_STATUS_PASSED;
}

static int crosstalk()
{
	unsigned int test_buffer[TEST_WORDS];
	int i, j;

	for(j=0;j<15;j++) {
		for(i=0;i<TEST_WORDS;i++)
			test_buffer[i] = i & 1 ? 0xaaaaaaaa : 0x55555555;
		flush_caches();
		for(i=0;i<TEST_WORDS;i++)
			if(test_buffer[i] != (i & 1 ? 0xaaaaaaaa : 0x55555555))
				return TEST_STATUS_FAILED;
	}
	return TEST_STATUS_PASSED;
}

struct test_description tests_sdram[] = {
	{
		.name = "A8",
		.run = a8
	},
	{
		.name = "A9",
		.run = a9
	},
	{
		.name = "A10",
		.run = a10
	},
	{
		.name = "A11",
		.run = a11
	},
	{
		.name = "A12",
		.run = a12
	},
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
