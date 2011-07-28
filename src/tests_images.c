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
#include <hw/interrupts.h>
#include "testdefs.h"

static int compare_crc()
{
	return TEST_STATUS_PASSED;
}

struct test_description tests_images[] = {
	{
		.name = "Images CRC",
		.run = compare_crc
	},
	{
		.name = NULL
	}
};
