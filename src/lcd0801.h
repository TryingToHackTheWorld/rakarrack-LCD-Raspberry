#ifndef LCD0801_H
#define LCD0801_H

#include <stdint.h>

void init_lcd_0801(int bus, uint8_t addr);
void lcd0801WriteString(int bus, uint8_t addr, const char *str);

#endif
