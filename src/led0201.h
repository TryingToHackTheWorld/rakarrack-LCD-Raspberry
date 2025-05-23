#ifndef LED0201_H
#define LED0201_H

void led_0201_init(const char* device, int latch_gpio);
void led_0201_displayNumber(int number);
void led_0201_cleanup();

#endif
