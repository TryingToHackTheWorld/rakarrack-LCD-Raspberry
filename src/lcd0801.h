#ifndef LCD0801_H
#define LCD0801_H

void lcd0801WriteString(int i2c_fd, uint8_t address, char* text);
void lcd0801WriteString(char *szText);
void init_lcd_0801(int i2c_fd, uint8_t address);
static void WriteCommand0801(unsigned char ucCMD);

#endif
