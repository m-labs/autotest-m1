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

static int compare_crc(unsigned int *flashbase, unsigned int crc, unsigned int length)
{
	unsigned int got_crc;

	/* Flickernoise have the length and crc at begin, See makefile of Flickernoise */
	if((unsigned int)flashbase == FLASH_OFFSET_RESCUE_APP ||
	   (unsigned int)flashbase == FLASH_OFFSET_REGULAR_APP) {
		length = *flashbase++;
		crc = *flashbase++;
	}

	/* BIOS have the crc at end, See makefile of BIOS */
	if((unsigned int)flashbase == FLASH_OFFSET_RESCUE_BIOS ||
	   (unsigned int)flashbase == FLASH_OFFSET_REGULAR_BIOS)
		crc = *(flashbase + (length / 4));

	if((length < 32) || (length > 4*1024*1024)) {
		printf("Invalid flash image length: %d\n", length);
		return TEST_STATUS_FAILED;
	}

	got_crc = crc32((unsigned char *)flashbase, length);
	if(crc != got_crc) {
		printf("CRC failed (expected %08x, got %08x)\n", crc, got_crc);
		return TEST_STATUS_FAILED;
	}

	printf("CRC passed (got %08x)\n", crc);
	return TEST_STATUS_PASSED;
}

extern unsigned int _edata;

#define IMAGES_COUNT 9

static int compare_loop()
{
	int i, ret, count;

	unsigned int crc, len;
	unsigned int images[IMAGES_COUNT];

	unsigned int * file_end = (unsigned int *) ((unsigned int)&_edata);

	char * images_name[IMAGES_COUNT] = {
		"standby.fpg",
		"soc-rescue.fpg",
		"bios-rescue.bin",
		"splash-rescue.raw",
		"flickernoise.fbi",
		"soc.fpg",
		"bios.bin",
		"splash.raw",
		"flickernoise.fbi"
	};

	images[0] = FLASH_OFFSET_STANDBY_BITSTREAM;
	images[1] = FLASH_OFFSET_RESCUE_BITSTREAM;
	images[2] = FLASH_OFFSET_RESCUE_BIOS;
	images[3] = FLASH_OFFSET_RESCUE_SPLASH;
	images[4] = FLASH_OFFSET_RESCUE_APP;
	images[5] = FLASH_OFFSET_REGULAR_BITSTREAM;
	images[6] = FLASH_OFFSET_REGULAR_BIOS;
	images[7] = FLASH_OFFSET_REGULAR_SPLASH;
	images[8] = FLASH_OFFSET_REGULAR_APP;

	/*
	 * images crc and len write at the end of boot.bin
	 * see "append_crc_len.sh" for more info
	 */

	count = 0;
	for(i = 0; i < IMAGES_COUNT; i++) {
		crc = *file_end++;
		len = *file_end++;

		printf("  %s\t", images_name[i]);

		ret = compare_crc((unsigned int *)images[i], crc, len);
		if(ret == TEST_STATUS_FAILED)
			count++;
	}

	if(count == 0)
		return TEST_STATUS_PASSED;
	else {
		printf("%d images failed\n", count);
		return TEST_STATUS_FAILED;
	}
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
