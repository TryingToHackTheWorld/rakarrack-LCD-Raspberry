#ifndef LCD0801_H
#define LCD0801_H

void init_lcd_0801(int fd, uint8_t address);
void lcd0801WriteString(int fd, uint8_t address, const char* text);

#endif
