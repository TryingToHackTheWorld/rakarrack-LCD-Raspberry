#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <gpiod.h>
#include "led0201.h"

static int spi_fd = -1;
static struct gpiod_chip* chip = NULL;
static struct gpiod_line* latch = NULL;

static const uint8_t digitToSegment[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

void led_0201_init(const char* device, int latch_gpio) {
    spi_fd = open(device, O_RDWR);
    if (spi_fd < 0) {
        perror("Error abriendo SPI");
        return;
    }

    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 500000;

    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    chip = gpiod_chip_open_by_name("gpiochip0");
    latch = gpiod_chip_get_line(chip, latch_gpio);
    gpiod_line_request_output(latch, "led_latch", 0);
}

void led_0201_displayNumber(int number) {
    if (spi_fd < 0 || number < 0 || number > 99) return;

    uint8_t data[2];
    data[0] = digitToSegment[number / 10];
    data[1] = digitToSegment[number % 10];

    gpiod_line_set_value(latch, 0);
    write(spi_fd, data, 2);
    gpiod_line_set_value(latch, 1);
}

void led_0201_cleanup() {
    if (spi_fd >= 0) close(spi_fd);
    if (chip) gpiod_chip_close(chip);
}
