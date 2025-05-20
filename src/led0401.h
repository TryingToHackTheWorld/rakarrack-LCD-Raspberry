#ifndef LED0401_H
#define LED0401_H

void init_ht16k33(int i2c_fd, uint8_t address);
void led0401write(int bus, uint8_t address, char* text);
extern uint16_t font14seg[128];
void init_font14seg();

#endif
