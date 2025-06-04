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
#include "lcd1601.h"
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
static int iBackLight1601 = BACKLIGHT;
static int file_i2c_1601 = -1;

void lcd1601WriteString(int i2c_fd, uint8_t address, char* text){
	char buf[9] = {0};
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c_1601 = i2c_fd;
		
		// Borrado linea 1
		WriteCommand1601(0x80 + 0x00);  // Set cursor to 0x00
		lcd1601WriteString("        ");
		
		// Borrado linea 2
		WriteCommand1601(0x80 + 0x40);  // Set cursor to 0x40
		lcd1601WriteString("        ");
		
		// Línea 1 - posición 0x00
		strncpy(buf, text, 8);
		WriteCommand1601(0x80 + 0x00);  // Set cursor to 0x00
		lcd1601WriteString(buf);

		// Línea lógica 2 - posición 0x40
		strncpy(buf, text + 8, 8);
		WriteCommand1601(0x80 + 0x40);  // Set cursor to 0x40
		lcd1601WriteString(buf);
	}
}

void init_lcd_1601(int i2c_fd, uint8_t address){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c_1601 = i2c_fd;
		
		iBackLight1601 = BACKLIGHT; // turn on backlight
	
		WriteCommand1601(0x02); // Set 4-bit mode of the LCD controller
		WriteCommand1601(0x28); // 2 lines, 5x8 dot matrix
		WriteCommand1601(0x0c); // display on, cursor off
		WriteCommand1601(0x06); // inc cursor to right when writing and don't scroll
		WriteCommand1601(0x80); // set cursor to row 1, column 1
		WriteCommand1601(0x0E); // clear the screen
		WriteCommand1601(0x0C); // display on, cursor off
	}
}

static void WriteCommand1601(unsigned char ucCMD)
{
	unsigned char uc;

	uc = (ucCMD & 0xf0) | iBackLight1601; // most significant nibble sent first
	write(file_i2c_1601, &uc, 1);
	usleep(PULSE_PERIOD); // manually pulse the clock line
	uc |= 4; // enable pulse
	write(file_i2c_1601, &uc, 1);
	usleep(PULSE_PERIOD);
	uc &= ~4; // toggle pulse
	write(file_i2c_1601, &uc, 1);
	usleep(CMD_PERIOD);
	uc = iBackLight1601 | (ucCMD << 4); // least significant nibble
	write(file_i2c_1601, &uc, 1);
	usleep(PULSE_PERIOD);
        uc |= 4; // enable pulse
        write(file_i2c_1601, &uc, 1);
        usleep(PULSE_PERIOD);
        uc &= ~4; // toggle pulse
        write(file_i2c_1601, &uc, 1);
	usleep(CMD_PERIOD);
}

void lcd1601WriteString(char *text)
{
	unsigned char ucTemp[2];
	int i = 0;
	
	while (i<16 && *text)
	{
		ucTemp[0] = iBackLight1601 | DATA | (*text & 0xf0);
		write(file_i2c_1601, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] |= 4; // pulse E
		write(file_i2c_1601, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] &= ~4;
		write(file_i2c_1601, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] = iBackLight1601 | DATA | (*text << 4);
		write(file_i2c_1601, ucTemp, 1);
		ucTemp[0] |= 4; // pulse E
                write(file_i2c_1601, ucTemp, 1);
                usleep(PULSE_PERIOD);
                ucTemp[0] &= ~4;
                write(file_i2c_1601, ucTemp, 1);
                usleep(CMD_PERIOD);
		text++;
		i++;
	}
}
