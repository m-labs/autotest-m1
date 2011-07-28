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

#include <hw/interrupts.h>
#include <hal/usb.h>
#include <irq.h>
#include <uart.h>
#include <stdio.h>
#include <console.h>
#include "testdefs.h"

void isr()
{
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

	if(irqs & IRQ_UARTRX)
		uart_isr_rx();
	if(irqs & IRQ_UARTTX)
		uart_isr_tx();

	if(irqs & IRQ_USB)
		usb_isr();
}

extern struct test_description tests_sdram[];
extern struct test_description tests_gpio[];
extern struct test_description tests_vga[];
extern struct test_description tests_memorycard[];
extern struct test_description tests_audio[];
extern struct test_description tests_ethernet[];
extern struct test_description tests_videoin[];
extern struct test_description tests_midi[];
extern struct test_description tests_dmx[];
extern struct test_description tests_ir[];
extern struct test_description tests_usb[];
extern struct test_description tests_images[];

struct test_category categories[] = {
	{
		.name = "SDRAM",
		.tests = tests_sdram
	},
	{
		.name = "GPIO",
		.tests = tests_gpio
	},
	{
		.name = "VGA",
		.tests = tests_vga
	},
	{
		.name = "Memory card",
		.tests = tests_memorycard
	},
	{
		.name = "Audio",
		.tests = tests_audio
	},
	{
		.name = "Ethernet",
		.tests = tests_ethernet
	},
	{
		.name = "Video input",
		.tests = tests_videoin
	},
	{
		.name = "MIDI",
		.tests = tests_midi
	},
	{
		.name = "DMX512",
		.tests = tests_dmx
	},
	{
		.name = "Infrared",
		.tests = tests_ir
	},
	{
		.name = "USB",
		.tests = tests_usb
	},
	{
		.name = "Images",
		.tests = tests_images
	},
	{
		.name = NULL
	}
};

static void print_test_status(int status)
{
	printf("\x1b\x5b\x41\x1b\x5b\x37\x34\x47\x5b\x20");
	switch(status) {
		case TEST_STATUS_NOT_DONE:
			printf(" SKIPPED ");
			break;
		case TEST_STATUS_FAILED:
			printf("\x1b\x5b\x33\x31\x6d FAILED ");
			break;
		case TEST_STATUS_PASSED:
			printf("\x1b\x5b\x33\x32\x6d PASSED ");
			break;
	}
	printf("\x1b\x5b\x33\x39\x3b\x34\x39\x6d\x20\x5d\n");
}

static void run_in_category(struct test_category *cat, int ic)
{
	int it;
	
	it = 0;
	while(cat[ic].tests[it].name != NULL) {
		printf("%s:\n", cat[ic].tests[it].name);
		cat[ic].tests[it].status = cat[ic].tests[it].run();
		print_test_status(cat[ic].tests[it].status);
		it++;
	}
}

static void run_all_tests(struct test_category *cat)
{
	int ic;

	ic = 0;
	while(cat[ic].name != NULL) {
		printf("*** Running tests in category: %s\n", cat[ic].name);
		run_in_category(cat, ic);
		ic++;
		printf("\n");
	}
}

static void print_summary(struct test_category *cat)
{
	int skipped, failed, passed;
	int ic, it;

	
	skipped = failed = passed = 0;

	printf("Failed tests:\n");
	ic = 0;
	while(cat[ic].name != NULL) {
		it = 0;
		while(cat[ic].tests[it].name != NULL) {
			switch(cat[ic].tests[it].status) {
				case TEST_STATUS_NOT_DONE:
					skipped++;
					break;
				case TEST_STATUS_FAILED:
					failed++;
					printf("  - %s/%s\n", cat[ic].name, cat[ic].tests[it].name);
					break;
				case TEST_STATUS_PASSED:
					passed++;
					break;
			}
			it++;
		}
		ic++;
	}
	if(failed == 0)
		printf("  - None\n");
	printf("\n");
	printf("Skipped tests: %d\n", skipped);
	printf("Failed tests:  %d\n", failed);
	printf("Passed tests:  %d\n", passed);
	printf("Total tests:   %d\n", skipped+failed+passed);
	printf("\n");
	if(failed == 0)
		printf("******** ALL TESTS PASSED ********\n");
	else
		printf("******** SOME TESTS FAILED ********\n");
	if(skipped != 0)
		printf("******** THERE WERE SOME SKIPPED TESTS ********\n");
}

int main()
{
	char c;
	int i;
	
	irq_setmask(0);
	irq_enable(1);
	uart_init();
	printf("*** Milkymist One automated tests starting...\n\n");
	while(1) {
		printf("*** Select a test category below, or hit ENTER to run all tests:\n");
		i = 0;
		while(categories[i].name != NULL) {
			printf("  %c: %s\n", 'a'+i, categories[i].name);
			i++;
		}
		c = readchar();
		if(c == '\n') {
			run_all_tests(categories);
			printf("******** TEST SUMMARY ********\n");
			print_summary(categories);
		} else if((c >= 'a') && ((c - 'a') < i)) {
			printf("*** %c pressed\n", c);
			run_in_category(categories, c - 'a');
		}
	}
	return 0;
}
