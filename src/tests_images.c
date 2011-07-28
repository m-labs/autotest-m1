/*
 * Milkymist One automated testing program
 * Copyright (C) 2011 Xiangfu Liu
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
#include <string.h>
#include <crc.h>

#include <hw/interrupts.h>
#include <hw/hpdmc.h>
#include <hw/flash.h>

#include "testdefs.h"

struct m1_image {
	char *name;
	unsigned int *addr;
	unsigned int crc;
	unsigned int len;
};

static struct m1_image images[] = {
	{
		.name = "standby.fpg",
		.addr = (unsigned int *)FLASH_OFFSET_STANDBY_BITSTREAM,
		.crc = 0xc58e8905,
		.len = 495060
	},
	{
		.name = "soc-rescue.fpg",
		.addr = (unsigned int *)FLASH_OFFSET_RESCUE_BITSTREAM,
		.crc = 0x30dcc535,
		.len = 1484404
	},
	{
		.name = "bios-rescue.bin(CRC)",
		.addr = (unsigned int *)FLASH_OFFSET_RESCUE_BIOS,
		.crc = 0,
		.len = 73812
	},
	{
		.name = "splash-rescue.raw",
		.addr = (unsigned int *)FLASH_OFFSET_RESCUE_SPLASH,
		.crc = 0xe8ff824f,
		.len = 614400
	},
	{
		.name = "flickernoise.fbi(rescue)(CRC)",
		.addr = (unsigned int *)FLASH_OFFSET_RESCUE_APP,
		.crc = 0,
		.len = 0
	},
	{
		.name = "soc.fpg",
		.addr = (unsigned int *)FLASH_OFFSET_REGULAR_BITSTREAM,
		.crc = 0x3a31e737,
		.len = 1484404
	},
	{
		.name = "bios.bin(CRC)",
		.addr = (unsigned int *)FLASH_OFFSET_REGULAR_BIOS,
		.crc = 0,
		.len = (73724 - 4)
	},
	{
		.name = "splash.raw",
		.addr = (unsigned int *)FLASH_OFFSET_REGULAR_SPLASH,
		.crc = 0x978f860c,
		.len = 614400
	},
	{
		.name = "flickernoise.fbi(CRC)",
		.addr = (unsigned int *)FLASH_OFFSET_REGULAR_APP,
		.crc = 0,
		.len = 0
	},
	{
		.name = NULL
	}
};

static int compare_crc(unsigned int *flashbase, unsigned int crc, unsigned int length)
{
	unsigned int got_crc;

	if(length == 0 && crc == 0) {
		length = *flashbase++;
		crc = *flashbase++;
	}

	if(length != 0 && crc == 0)
		crc = *(flashbase + (length / 4));

	if((length < 32) || (length > 4*1024*1024)) {
		printf("Invalid flash image length\n");
		return TEST_STATUS_FAILED;
	}

	unsigned int membase = SDRAM_BASE + 2000000;
	memcpy((void *)membase, flashbase, length);
	got_crc = crc32((unsigned char *)membase, length);
	if(crc != got_crc) {
		printf("CRC failed (expected %08x, got %08x)\n", crc, got_crc);
		return TEST_STATUS_FAILED;
	}

	printf("CRC passed (got %08x)\n", crc);
	return TEST_STATUS_PASSED;
}

static int compare_loop()
{
	int i, ret, result;

	result = TEST_STATUS_PASSED;
	i = 0;
	while(images[i].name != NULL) {
		printf("  Checking : %s\t", images[i].name);
		ret = compare_crc(images[i].addr, images[i].crc, images[i].len);
		if(ret == TEST_STATUS_FAILED)
			result = TEST_STATUS_FAILED;
		i++;
	}

	printf("\n");
	return result;
}

struct test_description tests_images[] = {
	{
		.name = "Images CRC",
		.run = compare_loop
	},
	{
		.name = NULL
	}
};
