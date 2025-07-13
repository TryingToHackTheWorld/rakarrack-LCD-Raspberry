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

void i2c_write_byte(int i2c_fd, uint8_t data) {
    if (write(i2c_fd, &data, 1) != 1) {
        perror("Error al escribir en el bus I2C");
        //exit(1);
    }
}

void lcd_strobe(int i2c_fd, uint8_t data) {
    i2c_write_byte(i2c_fd, data | PIN_EN | PIN_BL);
    usleep(500);
    i2c_write_byte(i2c_fd, (data & ~PIN_EN) | PIN_BL);
    usleep(100);
}

/*void lcd_write4bits(int i2c_fd, uint8_t data) {
    i2c_write_byte(i2c_fd, data | PIN_BL);
    lcd_strobe(i2c_fd, data);
}*/
void lcd_write4bits(int i2c_fd, uint8_t data) {
    uint8_t out = (data & 0xF0) | (data & (PIN_RS | PIN_RW)) | PIN_BL;
    i2c_write_byte(i2c_fd, out);
    lcd_strobe(i2c_fd, out);
}

/*void lcd_send(int i2c_fd, uint8_t cmd, uint8_t mode = 0) {
    uint8_t high = mode | (cmd & 0xF0);
    uint8_t low = mode | ((cmd << 4) & 0xF0);
    lcd_write4bits(i2c_fd, high);
    lcd_write4bits(i2c_fd, low);
}*/
void lcd_send(int i2c_fd, uint8_t cmd, uint8_t mode = 0) {
    uint8_t high = (cmd & 0xF0) | mode;
    uint8_t low  = ((cmd << 4) & 0xF0) | mode;
    lcd_write4bits(i2c_fd, high);
    lcd_write4bits(i2c_fd, low);
}

void init_lcd_0801(int i2c_fd, uint8_t address){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        //std::cout << "INIT FAIL\r\n";
    }else{
		//usleep(50000); // Esperar más de 40ms tras encender
		lcd_write4bits(i2c_fd, 0x30);
		usleep(5000);
		lcd_write4bits(i2c_fd, 0x30);
		usleep(1000);
		lcd_write4bits(i2c_fd, 0x30);
		usleep(1000);
		lcd_write4bits(i2c_fd, 0x20);  // 4-bit mode
		lcd_send(i2c_fd, 0x28, 0); // Function set: 4-bit, 2 lines (aunque solo se muestre una), 5x8 dots
		lcd_send(i2c_fd, 0x0C, 0); // Display ON, cursor OFF
		lcd_send(i2c_fd, 0x06, 0); // Entry mode set
		lcd_send(i2c_fd, 0x01, 0); // Clear display
		usleep(2000);
	}
}

void lcd0801WriteString(int i2c_fd, char *text){
	for (int i = 0; i < 8; ++i) {
        char c = text[i];
        if (c == '\0') break;
        lcd_send(i2c_fd, c, PIN_RS);
    }
}

void lcd0801WriteString(int i2c_fd, uint8_t address, char* text){
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        //std::cout << "INIT FAIL\r\n";
    }else{
		
		// Borrado linea 1
		lcd_send(i2c_fd, 0x80, 0);
		lcd_send(i2c_fd, 0x00, 0);  // Set cursor to 0x00
		lcd0801WriteString(i2c_fd, "        ");
		
		// Línea 1 - posición 0x00
		lcd_send(i2c_fd, 0x80, 0);
		lcd_send(i2c_fd, 0x00, 0);  // Set cursor to 0x00
		lcd0801WriteString(i2c_fd, text);
	}
}
