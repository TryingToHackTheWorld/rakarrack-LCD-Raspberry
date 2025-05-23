#ifndef LED0201_H
#define LED0201_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Inicializa el display SPI
int led_display_init(const char* device);

// Envía un byte al registro del display
void led_display_send(uint8_t reg, uint8_t data);

// Muestra un número de dos dígitos (00 a 99)
void led_display_show_number(int value);

// Borra el contenido del display
void led_display_clear(void);

// Libera recursos y apaga el display
void led_display_close(void);

#ifdef __cplusplus
}
#endif

#endif
