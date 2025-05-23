#ifndef LED0201_H
#define LED0201_H

int led_0201_init(const char* spi_device, int latch_gpio);
void led_0201_show_raw(int value);
void led_0201_show_number(int number);
void led_0201_cleanup();

#endif

