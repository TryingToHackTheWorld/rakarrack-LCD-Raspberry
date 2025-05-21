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
static int iBackLight = BACKLIGHT;
static int file_i2c = -1;

void lcd0801WriteString(int i2c_fd, uint8_t address, char* text){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c = i2c_fd;
		
		lcdWriteString("        ");  // 8 espacios
		lcdWriteString(text);
	}
}

void lcd1601WriteString(int i2c_fd, uint8_t address, char* text){
	char buf[9] = {0};
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c = i2c_fd;
		
		// Borrado linea 1
		WriteCommand(0x80 + 0x00);  // Set cursor to 0x00
		lcdWriteString("        ");
		
		// Borrado linea 2
		WriteCommand(0x80 + 0x40);  // Set cursor to 0x40
		lcdWriteString("        ");
		
		// Línea 1 - posición 0x00
		strncpy(buf, text, 8);
		WriteCommand(0x80 + 0x00);  // Set cursor to 0x00
		lcdWriteString(buf);

		// Línea lógica 2 - posición 0x40
		strncpy(buf, text + 8, 8);
		WriteCommand(0x80 + 0x40);  // Set cursor to 0x40
		lcdWriteString(buf);
	}
}

void init_lcd(int i2c_fd, uint8_t address){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c = i2c_fd;
		
		iBackLight = BACKLIGHT; // turn on backlight
	
		WriteCommand(0x02); // Set 4-bit mode of the LCD controller
		WriteCommand(0x28); // 2 lines, 5x8 dot matrix
		WriteCommand(0x0c); // display on, cursor off
		WriteCommand(0x06); // inc cursor to right when writing and don't scroll
		WriteCommand(0x80); // set cursor to row 1, column 1
		WriteCommand(0x0E); // clear the screen
		WriteCommand(0x0C); // display on, cursor off
	}
}

static void WriteCommand(unsigned char ucCMD)
{
	unsigned char uc;

	uc = (ucCMD & 0xf0) | iBackLight; // most significant nibble sent first
	write(file_i2c, &uc, 1);
	usleep(PULSE_PERIOD); // manually pulse the clock line
	uc |= 4; // enable pulse
	write(file_i2c, &uc, 1);
	usleep(PULSE_PERIOD);
	uc &= ~4; // toggle pulse
	write(file_i2c, &uc, 1);
	usleep(CMD_PERIOD);
	uc = iBackLight | (ucCMD << 4); // least significant nibble
	write(file_i2c, &uc, 1);
	usleep(PULSE_PERIOD);
        uc |= 4; // enable pulse
        write(file_i2c, &uc, 1);
        usleep(PULSE_PERIOD);
        uc &= ~4; // toggle pulse
        write(file_i2c, &uc, 1);
	usleep(CMD_PERIOD);
}

void lcdWriteString(char *text)
{
	unsigned char ucTemp[2];
	int i = 0;
	
	while (i<16 && *text)
	{
		ucTemp[0] = iBackLight | DATA | (*text & 0xf0);
		write(file_i2c, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] |= 4; // pulse E
		write(file_i2c, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] &= ~4;
		write(file_i2c, ucTemp, 1);
		usleep(PULSE_PERIOD);
		ucTemp[0] = iBackLight | DATA | (*text << 4);
		write(file_i2c, ucTemp, 1);
		ucTemp[0] |= 4; // pulse E
                write(file_i2c, ucTemp, 1);
                usleep(PULSE_PERIOD);
                ucTemp[0] &= ~4;
                write(file_i2c, ucTemp, 1);
                usleep(CMD_PERIOD);
		text++;
		i++;
	}
}
