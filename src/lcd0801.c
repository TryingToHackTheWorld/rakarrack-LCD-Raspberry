// lcd0801.c - Versión adaptada desde prueba Python funcional
#include "lcd0801.h"
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

// Definición de pines según PCF8574
#define LCD_RS  0x01  // P0
#define LCD_RW  0x02  // P1 (no se usa)
#define LCD_EN  0x04  // P2
#define LCD_BL  0x08  // P3

#define LCD_D4  0x10  // P4
#define LCD_D5  0x20  // P5
#define LCD_D6  0x40  // P6
#define LCD_D7  0x80  // P7

static void lcd0801WriteNibble(int bus, uint8_t addr, uint8_t nibble, uint8_t rs)
{
    uint8_t data = 0;
    data |= (nibble & 0x0F) << 4;  // D4-D7
    data |= rs ? LCD_RS : 0;
    data |= LCD_BL; // mantener backlight encendido
    data |= LCD_EN;
    i2c_smbus_write_byte(bus, data);
    usleep(1);
    data &= ~LCD_EN;
    i2c_smbus_write_byte(bus, data);
    usleep(40);
}

static void lcd0801WriteByte(int bus, uint8_t addr, uint8_t value, uint8_t rs)
{
    lcd0801WriteNibble(bus, addr, value >> 4, rs);
    lcd0801WriteNibble(bus, addr, value & 0x0F, rs);
}

static void lcd0801Command(int bus, uint8_t addr, uint8_t cmd)
{
    lcd0801WriteByte(bus, addr, cmd, 0);
    if (cmd == 0x01 || cmd == 0x02) usleep(1600); // clear/home tarda más
    else usleep(40);
}

static void lcd0801WriteChar(int bus, uint8_t addr, char c)
{
    lcd0801WriteByte(bus, addr, c, 1);
}

void init_lcd_0801(int bus, uint8_t addr)
{
    // Inicialización en modo 4 bits (HD44780-compatible)
    usleep(50000);
    lcd0801WriteNibble(bus, addr, 0x03, 0);
    usleep(4500);
    lcd0801WriteNibble(bus, addr, 0x03, 0);
    usleep(150);
    lcd0801WriteNibble(bus, addr, 0x03, 0);
    usleep(150);
    lcd0801WriteNibble(bus, addr, 0x02, 0); // modo 4 bits

    lcd0801Command(bus, addr, 0x28); // 4-bit, 1-line, 5x8 dots
    lcd0801Command(bus, addr, 0x0C); // display on, cursor off
    lcd0801Command(bus, addr, 0x06); // entry mode: cursor right
    lcd0801Command(bus, addr, 0x01); // clear display
    usleep(2000);
}

void lcd0801WriteString(int bus, uint8_t addr, const char *str)
{
    lcd0801Command(bus, addr, 0x80); // posición inicial
    for (int i = 0; i < 8 && str[i]; i++)
        lcd0801WriteChar(bus, addr, str[i]);
}
