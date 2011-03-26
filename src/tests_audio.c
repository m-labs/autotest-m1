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
#include <hw/ac97.h>
#include <irq.h>
#include <stdio.h>
#include <math.h>
#include <console.h>
#include "testdefs.h"

static unsigned int snd_ac97_read(unsigned int addr)
{
	int timeout;
	CSR_AC97_CRADDR = addr;
	irq_ack(IRQ_AC97CRREPLY);
	CSR_AC97_CRCTL = AC97_CRCTL_RQEN;
	timeout = 200000;
	while(!(irq_pending() & IRQ_AC97CRREPLY)) {
		timeout--;
		if(timeout == 0) {
			printf("Timeout waiting for AC97 register read\n");
			return 0;
		}
	}
	return CSR_AC97_CRDATAIN;
}

static int snd_ac97_write(unsigned int addr, unsigned int value)
{
	int timeout;
	CSR_AC97_CRADDR = addr;
	CSR_AC97_CRDATAOUT = value;
	irq_ack(IRQ_AC97CRREQUEST);
	CSR_AC97_CRCTL = AC97_CRCTL_RQEN|AC97_CRCTL_WRITE;
	timeout = 200000;
	while(!(irq_pending() & IRQ_AC97CRREQUEST)) {
		timeout--;
		if(timeout == 0) {
			printf("Timeout waiting for AC97 register write\n");
			return 0;
		}
	}
	return 1;
}

static int codecprobe()
{
	int reg;
	
	CSR_AC97_CRCTL = 0;
	CSR_AC97_DCTL = 0;
	CSR_AC97_UCTL = 0;

	reg = snd_ac97_read(0x00);
	if((reg != 0x0d50) && (reg != 0x6150)) {
		printf("Unknown codec ID: %04x\n", reg);
		return TEST_STATUS_FAILED;
	}

	return TEST_STATUS_PASSED;
}

static int lineout()
{
	unsigned short v;
	static unsigned short test_tone[2*48000];
	int i, j;
	char c;

	for(i=0;i<120;i++) {
		v = 12000.0*sinf(6.28*((float)i)*400.0/48000.0);
		for(j=0;j<400;j++)
			test_tone[240*j+2*i] = test_tone[240*j+2*i+1] = v;
	}
	if(!snd_ac97_write(0x02, 0x0000)) return TEST_STATUS_FAILED; /* master volume */
	if(!snd_ac97_write(0x04, 0x0f0f)) return TEST_STATUS_FAILED; /* headphones volume */
	if(!snd_ac97_write(0x18, 0x0000)) return TEST_STATUS_FAILED; /* PCM out volume */
	while(1) {
		CSR_AC97_DADDRESS = (unsigned int)test_tone;
		CSR_AC97_DREMAINING = sizeof(test_tone);
		CSR_AC97_DCTL = AC97_SCTL_EN;
		while(CSR_AC97_DREMAINING > 0);
		CSR_AC97_DCTL = 0;
		printf("Is the test tone OK? (y/n/s/r)\n");
		c = readchar();
		switch(c) {
			case 'y':
				return TEST_STATUS_PASSED;
			case 'n':
				return TEST_STATUS_FAILED;
			case 's':
				return TEST_STATUS_NOT_DONE;
		}
	}
}

static int input_test()
{
	static unsigned short buffer[AC97_MAX_DMASIZE/2];
	char c;

	while(1) {
		printf("Recording...");
		CSR_AC97_UADDRESS = (unsigned int)buffer;
		CSR_AC97_UREMAINING = sizeof(buffer);
		CSR_AC97_UCTL = AC97_SCTL_EN;
		while(CSR_AC97_UREMAINING > 0);
		CSR_AC97_UCTL = 0;
		printf("Playing...");
		CSR_AC97_DADDRESS = (unsigned int)buffer;
		CSR_AC97_DREMAINING = sizeof(buffer);
		CSR_AC97_DCTL = AC97_SCTL_EN;
		while(CSR_AC97_DREMAINING > 0);
		CSR_AC97_DCTL = 0;
		printf("\n");
		printf("Is the audio played back? (y/n/s/r)\n");
		c = readchar();
		switch(c) {
			case 'y':
				return TEST_STATUS_PASSED;
			case 'n':
				return TEST_STATUS_FAILED;
			case 's':
				return TEST_STATUS_NOT_DONE;
		}
	}
}

static int microphone()
{
	int r;
	if(!snd_ac97_write(0x0e, 0x0000)) return TEST_STATUS_FAILED; /* mic volume */
	if(!snd_ac97_write(0x1a, 0x0000)) return TEST_STATUS_FAILED; /* record select */
	if(!snd_ac97_write(0x1c, 0x0f0f)) return TEST_STATUS_FAILED; /* record gain */
	r = input_test();
	if(!snd_ac97_write(0x0e, 0xffff)) return TEST_STATUS_FAILED; /* mic volume */
	return r;
}

static int linein()
{
	int r;
	if(!snd_ac97_write(0x10, 0x0000)) return TEST_STATUS_FAILED; /* line in volume */
	if(!snd_ac97_write(0x1a, 0x0404)) return TEST_STATUS_FAILED; /* record select */
	if(!snd_ac97_write(0x1c, 0x0f0f)) return TEST_STATUS_FAILED; /* record gain */
	r = input_test();
	if(!snd_ac97_write(0x10, 0xffff)) return TEST_STATUS_FAILED; /* line in volume */
	return r;
}

struct test_description tests_audio[] = {
	{
		.name = "Codec probe",
		.run = codecprobe
	},
	{
		.name = "Line and headphones outputs",
		.run = lineout
	},
	{
		.name = "Microphone",
		.run = microphone
	},
	{
		.name = "Line input",
		.run = linein
	},
	{
		.name = NULL
	}
};
