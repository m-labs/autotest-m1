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
#include <hw/vga.h>
#include "testdefs.h"

static int i2c_started;

static int i2c_init(void)
{
	unsigned int timeout;

	i2c_started = 0;
	CSR_VGA_DDC = VGA_DDC_SDC;
	/* Check the I2C bus is ready */
	timeout = 1000;
	while((timeout > 0) && (!(CSR_VGA_DDC & VGA_DDC_SDAIN))) timeout--;

	return timeout;
}

static void i2c_delay(void)
{
	unsigned int i;

	for(i=0;i<1000;i++) __asm__("nop");
}

/* I2C bit-banging functions from http://en.wikipedia.org/wiki/I2c */
static unsigned int i2c_read_bit(void)
{
	unsigned int bit;

	/* Let the slave drive data */
	CSR_VGA_DDC = 0;
	i2c_delay();
	CSR_VGA_DDC = VGA_DDC_SDC;
	i2c_delay();
	bit = CSR_VGA_DDC & VGA_DDC_SDAIN;
	i2c_delay();
	CSR_VGA_DDC = 0;
	return bit;
}

static void i2c_write_bit(unsigned int bit)
{
	if(bit) {
		CSR_VGA_DDC = VGA_DDC_SDAOE|VGA_DDC_SDAOUT;
	} else {
		CSR_VGA_DDC = VGA_DDC_SDAOE;
	}
	i2c_delay();
	CSR_VGA_DDC |= VGA_DDC_SDC;
	i2c_delay();
	CSR_VGA_DDC &= ~VGA_DDC_SDC;
}

static void i2c_start_cond(void)
{
	if(i2c_started) {
		/* set SDA to 1 */
		CSR_VGA_DDC = VGA_DDC_SDAOE|VGA_DDC_SDAOUT;
		i2c_delay();
		CSR_VGA_DDC |= VGA_DDC_SDC;
		i2c_delay();
	}
	/* SCL is high, set SDA from 1 to 0 */
	CSR_VGA_DDC = VGA_DDC_SDAOE|VGA_DDC_SDC;
	i2c_delay();
	CSR_VGA_DDC = VGA_DDC_SDAOE;
	i2c_started = 1;
}

static void i2c_stop_cond(void)
{
	/* set SDA to 0 */
	CSR_VGA_DDC = VGA_DDC_SDAOE;
	i2c_delay();
	CSR_VGA_DDC = VGA_DDC_SDAOE|VGA_DDC_SDC;
	/* SCL is high, set SDA from 0 to 1 */
	CSR_VGA_DDC = VGA_DDC_SDC;
	i2c_delay();
	i2c_started = 0;
}

static unsigned int i2c_write(unsigned char byte)
{
	unsigned int bit;
	unsigned int ack;

	for(bit = 0; bit < 8; bit++) {
		i2c_write_bit(byte & 0x80);
		byte <<= 1;
	}
	ack = !i2c_read_bit();
	return ack;
}

static unsigned char i2c_read(int ack)
{
	unsigned char byte = 0;
	unsigned int bit;

	for(bit = 0; bit < 8; bit++) {
		byte <<= 1;
		byte |= i2c_read_bit();
	}
	i2c_write_bit(!ack);
	return byte;
}

static int vga_read_edid(char *buffer)
{
	int i;

	i2c_start_cond();
	if(!i2c_write(0xA0)) {
		printf("No ack for 0xA0 address\n");
		return 0;
	}
	if(!i2c_write(0x00)) {
		printf("No ack for EDID offset\n");
		return 0;
	}
	i2c_start_cond();
	if(!i2c_write(0xA1)) {
		printf("No ack for 0xA1 address\n");
		return 0;
	}

	for(i=0;i<255;i++)
		buffer[i] = i2c_read(1);
	buffer[255] = i2c_read(0);
	i2c_stop_cond();

	return 1;
}

static int ddc(void)
{
	unsigned int buffer[64];

	if(!i2c_init()) {
		printf("I2C bus initialization problem\n");
		return TEST_STATUS_FAILED;
	}
	if(!vga_read_edid((char *)buffer))
		return TEST_STATUS_FAILED;
	if((buffer[0] != 0x00ffffff)||(buffer[1] != 0xffffff00)) {
		printf("Unexpected EDID data\n");
		return TEST_STATUS_FAILED;
	}
	return TEST_STATUS_PASSED;
}

#define RSHIFT 11
#define GSHIFT 5
#define BSHIFT 0
#define MAKERGB565(r, g, b) ((((r) & 0x1f) << RSHIFT) | (((g) & 0x3f) << GSHIFT) | (((b) & 0x1f) << BSHIFT))

short int fb[640*480] __attribute__((aligned(32)));
static int testcard(void)
{
	unsigned int x, y;
	unsigned int r, g, b;
	char c;
	
	CSR_VGA_BASEADDRESS = (unsigned int)fb;
	CSR_VGA_RESET = 0;
	for(y=0;y<240;y++) {
		g = 64*y/240;
		for(x=0;x<256;x++) {
			r = x >> 3;
			b = 31-r;
			fb[640*y+x] = MAKERGB565(r, g, b);
		}
	}
	for(y=0;y<240;y++) {
		r = b = 32*y/240;
		g = 64*y/240;
		for(x=0;x<384;x++)
			fb[640*y+x+256] = MAKERGB565(r, g, b);
	}

	for(x=256;x<640;x+=2) {
		for(y=240;y<480;y++)
			fb[640*y+x] = MAKERGB565(31, 63, 31);
	}
	for(x=0;x<640;x+=64) {
		for(y=0;y<480;y++)
			fb[640*y+x] = MAKERGB565(31, 63, 31);
	}
	for(y=0;y<480;y+=48) {
		for(x=0;x<640;x++)
			fb[640*y+x] = MAKERGB565(31, 63, 31);
	}
	
	printf("Is the picture OK? (y/n/s)\n");
	while(1) {
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

struct test_description tests_vga[] = {
	{
		.name = "DDC",
		.run = ddc
	},
	{
		.name = "Test card",
		.run = testcard
	},
	{
		.name = NULL
	}
};
