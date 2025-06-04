// lcd0801.c - Controlador de pantalla LCD 0801B via I2C con PCF8574
#include "lcd0801.h"
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>

#define LCD_ENABLE      0x04
#define LCD_BACKLIGHT   0x08
#define LCD_COMMAND     0
#define LCD_DATA        1

static void lcd0801_write_nibble(int fd, uint8_t address, uint8_t nibble, uint8_t mode) {
    uint8_t data = (nibble & 0xF0) | LCD_BACKLIGHT | (mode ? 0x01 : 0x00);

    ioctl(fd, I2C_SLAVE, address);
    uint8_t buffer[1];

    buffer[0] = data | LCD_ENABLE;
    write(fd, buffer, 1);
    usleep(1);

    buffer[0] = data & ~LCD_ENABLE;
    write(fd, buffer, 1);
    usleep(50);
}

static void lcd0801_write_byte(int fd, uint8_t address, uint8_t byte, uint8_t mode) {
    lcd0801_write_nibble(fd, address, byte & 0xF0, mode);
    lcd0801_write_nibble(fd, address, (byte << 4) & 0xF0, mode);
}

void init_lcd_0801(int fd, uint8_t address) {
    ioctl(fd, I2C_SLAVE, address);
    usleep(50000);

    // Inicialización en 4 bits
    lcd0801_write_nibble(fd, address, 0x30, LCD_COMMAND);
    usleep(4500);
    lcd0801_write_nibble(fd, address, 0x30, LCD_COMMAND);
    usleep(4500);
    lcd0801_write_nibble(fd, address, 0x30, LCD_COMMAND);
    usleep(150);
    lcd0801_write_nibble(fd, address, 0x20, LCD_COMMAND);
    usleep(150);

    lcd0801_write_byte(fd, address, 0x28, LCD_COMMAND); // 4-bit, 1 line, 5x8 dots
    lcd0801_write_byte(fd, address, 0x08, LCD_COMMAND); // Display OFF
    lcd0801_write_byte(fd, address, 0x01, LCD_COMMAND); // Clear display
    usleep(2000);
    lcd0801_write_byte(fd, address, 0x06, LCD_COMMAND); // Entry mode
    lcd0801_write_byte(fd, address, 0x0C, LCD_COMMAND); // Display ON, cursor OFF
}

void lcd0801WriteString(int fd, uint8_t address, const char* text) {
    ioctl(fd, I2C_SLAVE, address);
    for (int i = 0; i < 8 && text[i]; ++i) {
        lcd0801_write_byte(fd, address, text[i], LCD_DATA);
    }
}
