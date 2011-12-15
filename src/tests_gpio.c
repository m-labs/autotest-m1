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
#include <console.h>
#include <hw/sysctl.h>
#include <hw/gpio.h>
#include "testdefs.h"

static int generic_sw(unsigned int b)
{
	char c;
	
	printf("Press the button. f to fail test, s to skip.\n");
	if(CSR_GPIO_IN & b) return TEST_STATUS_FAILED;
	while(!(CSR_GPIO_IN & b)) {
		if(readchar_nonblock()) {
			c = readchar();
			if(c == 'f')
				return TEST_STATUS_FAILED;
			if(c == 's')
				return TEST_STATUS_NOT_DONE;
		}
	}
	while((CSR_GPIO_IN & b)) {
		if(readchar_nonblock()) {
			c = readchar();
			if(c == 'f')
				return TEST_STATUS_FAILED;
			if(c == 's')
				return TEST_STATUS_NOT_DONE;
		}
	}
	return TEST_STATUS_PASSED;
}

static int generic_led(unsigned int l)
{
	char c;
	CSR_GPIO_OUT = l;
	printf("Is the LED on? (y/n/s)\n");
	while(1) {
		c = readchar();
		switch(c) {
			case 'y':
				CSR_GPIO_OUT = 0;
				return TEST_STATUS_PASSED;
			case 'n':
				CSR_GPIO_OUT = 0;
				return TEST_STATUS_FAILED;
			case 's':
				CSR_GPIO_OUT = 0;
				return TEST_STATUS_NOT_DONE;
		}
	}
}

static int sw1(void)
{
	return generic_sw(GPIO_BTN1);
}

static int sw2(void)
{
	return generic_sw(GPIO_BTN2);
}

static int sw3(void)
{
	return generic_sw(GPIO_BTN3);
}

static int led2(void)
{
	return generic_led(GPIO_LED1);
}

static int led3(void)
{
	return generic_led(GPIO_LED2);
}

struct test_description tests_gpio[] = {
	{
		.name = "SW1",
		.run = sw1
	},
	{
		.name = "SW2",
		.run = sw2
	},
	{
		.name = "SW3",
		.run = sw3
	},
	{
		.name = "LED2",
		.run = led2
	},
	{
		.name = "LED3",
		.run = led3
	},
	{
		.name = NULL
	}
};
