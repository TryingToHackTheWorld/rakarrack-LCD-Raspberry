#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <iostream>
#include "led0201.h"

static int spi_fd = -1;

int led_display_init(const char* device) {
    spi_fd = open(device, O_WRONLY);
    if (spi_fd < 0) {
        perror("Error al abrir el dispositivo SPI");
        return -1;
    }

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = 1000000;

    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    // Configuración inicial (para MAX7219)
    led_display_send(0x0F, 0x00); // Test mode OFF
    led_display_send(0x0C, 0x01); // Normal mode
    led_display_send(0x0B, 0x01); // Solo dígitos 0 y 1
    led_display_send(0x09, 0x00); // Decode mode OFF
    led_display_clear();

    return 0;
}

void led_display_send(uint8_t reg, uint8_t data) {
    if (spi_fd < 0) return;
    uint8_t tx[2] = { reg, data };
    write(spi_fd, tx, sizeof(tx));
}

void led_display_show_number(int value) {
    if (value < 0 || value > 99) return;
    uint8_t unidades = value % 10;
    uint8_t decenas = (value / 10) % 10;
    led_display_send(0x01, unidades); // Dígito derecho
    led_display_send(0x02, decenas);  // Dígito izquierdo
}

void led_display_clear(void) {
    led_display_send(0x01, 0x0F); // Apagar dígito 0
    led_display_send(0x02, 0x0F); // Apagar dígito 1
}

void led_display_close(void) {
    if (spi_fd >= 0) {
        led_display_clear();
        close(spi_fd);
        spi_fd = -1;
    }
}

