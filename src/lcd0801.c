#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "lcd0801.h"

// Pines del PCF8574
const uint8_t PIN_RS = 0x01;
const uint8_t PIN_RW = 0x02;
const uint8_t PIN_EN = 0x04;
const uint8_t PIN_BL = 0x08;

static int file_i2c_0801 = -1;

void i2c_write_byte(uint8_t data) {
    if (write(file_i2c_0801, &data, 1) != 1) {
        perror("Error al escribir en el bus I2C");
        exit(1);
    }
}

void lcd_strobe(uint8_t data) {
    i2c_write_byte(data | PIN_EN | PIN_BL);
    usleep(500);
    i2c_write_byte((data & ~PIN_EN) | PIN_BL);
    usleep(100);
}

void lcd_write4bits(uint8_t data) {
    i2c_write_byte(data | PIN_BL);
    lcd_strobe(data);
}

void lcd_send(uint8_t cmd, uint8_t mode = 0) {
    uint8_t high = mode | (cmd & 0xF0);
    uint8_t low = mode | ((cmd << 4) & 0xF0);
    lcd_write4bits(high);
    lcd_write4bits(low);
}

void init_lcd_0801(int i2c_fd, uint8_t address){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c_0801 = i2c_fd;
		
		usleep(50000); // Esperar más de 40ms tras encender
		lcd_write4bits(0x30);
		usleep(5000);
		lcd_write4bits(0x30);
		usleep(1000);
		lcd_write4bits(0x30);
		usleep(1000);
		lcd_write4bits(0x20);  // 4-bit mode

		lcd_send(0x28); // Function set: 4-bit, 2 lines (aunque solo se muestre una), 5x8 dots
		lcd_send(0x0C); // Display ON, cursor OFF
		lcd_send(0x06); // Entry mode set
		lcd_send(0x01); // Clear display
		usleep(2000);
	}
}

void lcd0801WriteString(char *text){
	for (int i = 0; i < 8; ++i) {
        char c = text[i];
        if (c == '\0') break;
        lcd_send(c, PIN_RS);
    }
}

void lcd0801WriteString(int i2c_fd, uint8_t address, char* text){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{
		file_i2c_1601 = i2c_fd;
		
		// Borrado linea 1
		lcd_send(0x80);
		lcd_send(0x00);  // Set cursor to 0x00
		lcd0801WriteString("        ");
		
		// Línea 1 - posición 0x00
		lcd_send(0x80);
		lcd_send(0x00);  // Set cursor to 0x00
		lcd0801WriteString(text);
	}
}
