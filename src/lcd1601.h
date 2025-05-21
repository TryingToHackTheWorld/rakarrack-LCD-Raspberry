#ifndef LCD1602_H
#define LCD1602_H

void lcd1601WriteString(int i2c_fd, uint8_t address, char* text);
void lcd0801WriteString(int i2c_fd, uint8_t address, char* text);
void lcdWriteString(char *szText);
void init_lcd(int i2c_fd, uint8_t address);
static void WriteCommand(unsigned char ucCMD);

#endif
