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
#include <irq.h>
#include <uart.h>
#include <stdio.h>
#include "testdefs.h"

void isr()
{
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

	if(irqs & IRQ_UARTRX)
		uart_async_isr_rx();
	if(irqs & IRQ_UARTTX)
		uart_async_isr_tx();
}

int tf1()
{
	return TEST_STATUS_PASSED;
}

struct test_description tests[] = {
	{
		.name = "Toto a l'ecole",
		.run = tf1
	},
	{
		.name = NULL
	}
};

struct test_category categories[] = {
	{
		.name = "Category 1",
		.tests = tests
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
			printf("  SKIP  ");
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

static void run_all_tests(struct test_category *cat)
{
	int ic;
	int it;

	ic = 0;
	while(cat[ic].name != NULL) {
		printf("*** Running tests in category: %s\n", cat[ic].name);
		it = 0;
		while(cat[ic].tests[it].name != NULL) {
			printf("%s:\n", cat[ic].tests[it].name);
			cat[ic].tests[it].status = cat[ic].tests[it].run();
			print_test_status(cat[ic].tests[it].status);
			it++;
		}
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
	irq_setmask(0);
	irq_enable(1);
	uart_async_init();
	printf("*** Milkymist One automated tests starting...\n\n");
	run_all_tests(categories);
	printf("******** TEST SUMMARY ********\n");
	print_summary(categories);
	printf("*** Power off/reset board now...\n");
	while(1);
	return 0;
}
