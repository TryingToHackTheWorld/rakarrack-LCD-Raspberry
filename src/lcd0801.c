//
// 2x16 LCD display (HD44780 controller + I2C chip)
//
// Copyright (c) BitBank Software, Inc.
// Written by Larry Bank
// bitbank@pobox.com
// Project started 12/6/2017
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include "lcd0801.h"
//
// The LCD controller is wired to the I2C port expander with the upper 4 bits
// (D4-D7) connected to the data lines and the lower 4 bits (D0-D3) used as
// control lines. Here are the control line definitions:
//
// Command (0) / Data (1) (aka RS) (D0)
// R/W                             (D1)
// Enable/CLK                      (D2) 
// Backlight control               (D3)
//
// The data must be manually clocked into the LCD controller by toggling
// the CLK line after the data has been placed on D4-D7
//
#define PULSE_PERIOD 500
#define CMD_PERIOD 4100

#define BACKLIGHT 8
#define DATA 1
static int iBackLight0801 = BACKLIGHT;
static int file_i2c_0801 = -1;

void lcd0801WriteString(int i2c_fd, uint8_t address, char* text){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c_0801 = i2c_fd;
		
		WriteCommand0801(0x80 + 0x00);  // Set cursor to 0x00
		lcd0801WriteString("        ");  // 8 espacios
		
		WriteCommand0801(0x80 + 0x00);  // Set cursor to 0x00
		lcd0801WriteString(text);
	}
}

void init_lcd_0801(int i2c_fd, uint8_t address){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c_0801 = i2c_fd;
		
		iBackLight0801 = BACKLIGHT; // turn on backlight
	
		WriteCommand0801(0x02); // Set 4-bit mode of the LCD controller
		WriteCommand0801(0x28); // 2 lines, 5x8 dot matrix
		WriteCommand0801(0x0c); // display on, cursor off
		WriteCommand0801(0x06); // inc cursor to right when writing and don't scroll
		WriteCommand0801(0x80); // set cursor to row 1, column 1
		WriteCommand0801(0x0E); // clear the screen
		WriteCommand0801(0x0C); // display on, cursor off
	}
}

static void WriteCommand0801(unsigned char ucCMD)
{
	unsigned char uc;

	uc = (ucCMD & 0xf0) | iBackLight0801; // most significant nibble sent first
	write(file_i2c_0801, &uc, 1);
	usleep(PULSE_PERIOD); // manually pulse the clock line
	uc |= 4; // enable pulse
	write(file_i2c_0801, &uc, 1);
	usleep(PULSE_PERIOD);
	uc &= ~4; // toggle pulse
	write(file_i2c_0801, &uc, 1);
	usleep(CMD_PERIOD);
	uc = iBackLight0801 | (ucCMD << 4); // least significant nibble
	write(file_i2c_0801, &uc, 1);
	usleep(PULSE_PERIOD);
        uc |= 4; // enable pulse
        write(file_i2c_0801, &uc, 1);
        usleep(PULSE_PERIOD);
        uc &= ~4; // toggle pulse
        write(file_i2c_0801, &uc, 1);
	usleep(CMD_PERIOD);
}

void lcd0801WriteString(char *text)
{
	unsigned char ucTemp[2];
	int i = 0;
	
	while (i<16 && *text)
	{
		ucTemp[0] = iBackLight0801 | DATA | (*text & 0xf0);
		write(file_i2c_0801, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] |= 4; // pulse E
		write(file_i2c_0801, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] &= ~4;
		write(file_i2c_0801, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] = iBackLight0801 | DATA | (*text << 4);
		write(file_i2c_0801, ucTemp, 1);
		ucTemp[0] |= 4; // pulse E
                write(file_i2c_0801, ucTemp, 1);
                usleep(PULSE_PERIOD);
                ucTemp[0] &= ~4;
                write(file_i2c_0801, ucTemp, 1);
                usleep(CMD_PERIOD);
		text++;
		i++;
	}
}
