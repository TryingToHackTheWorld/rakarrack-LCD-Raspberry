#ifndef LCD1602_H
#define LCD1602_H

void lcd1601WriteString(int i2c_fd, uint8_t address, char* text);
void lcd1601WriteString(char *szText);
void init_lcd_1601(int i2c_fd, uint8_t address);
static void WriteCommand1601(unsigned char ucCMD);

#endif
