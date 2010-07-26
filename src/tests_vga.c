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
#include "testdefs.h"

static int ddc()
{
	return TEST_STATUS_FAILED; // FIXME
}

static int pattern()
{
	return TEST_STATUS_PASSED; // FIXME
}

struct test_description tests_vga[] = {
	{
		.name = "DDC",
		.run = ddc
	},
	{
		.name = "Pattern",
		.run = pattern
	},
	{
		.name = NULL
	}
};
