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
#include <hal/usb.h>
#include <console.h>
#include "testdefs.h"

static int initialized;
static volatile int success;

static void keyboard_cb(unsigned char modifiers, unsigned char key)
{
	if(key == 0x28)
		success = 1;
}

static void mouse_cb(unsigned char buttons, char dx, char dy, unsigned char wheel)
{
	if(buttons == 0x01)
		success = 1;
}

static int test_keyboard()
{
	char c;
	
	if(!initialized) {
		usb_init();
		initialized = 1;
	}
	printf("Press ENTER at the USB keyboard or press the left mouse button...\n");
	success = 0;
	usb_set_keyboard_cb(keyboard_cb);
	usb_set_mouse_cb(mouse_cb);
	while(!success) {
		if(readchar_nonblock()) {
			c = readchar();
			if(c == 'f')
				return TEST_STATUS_FAILED;
			if(c == 's')
				return TEST_STATUS_NOT_DONE;
		}
	}
	usb_set_mouse_cb(NULL);
	usb_set_keyboard_cb(NULL);
	return TEST_STATUS_PASSED;
}

struct test_description tests_usb[] = {
	{
		.name = "Port A",
		.run = test_keyboard
	},
	{
		.name = "Port B",
		.run = test_keyboard
	},
	{
		.name = NULL
	}
};
