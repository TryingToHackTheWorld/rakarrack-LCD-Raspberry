#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "led0201.h"

static int spi_fd = -1;
static int gpio_latch = -1;

static const uint8_t digits[10] = {
	0b11000000, // 0
	0b11111001, // 1
	0b10100100, // 2
	0b10110000, // 3
	0b10011001, // 4
	0b10010010, // 5
	0b10000010, // 6
	0b11111000, // 7
	0b10000000, // 8
	0b10010000  // 9
};

static void gpio_write(int gpio, int value) {
	char path[64];
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio);
	int fd = open(path, O_WRONLY);
	if (fd >= 0) {
		dprintf(fd, "%d", value);
		close(fd);
	}
}

static void gpio_export(int gpio) {
	char path[64];
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", gpio);
	if (access(path, F_OK) == -1) {
		int fd = open("/sys/class/gpio/export", O_WRONLY);
		if (fd >= 0) {
			dprintf(fd, "%d", gpio);
			close(fd);
		}
	}
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio);
	int fd = open(path, O_WRONLY);
	if (fd >= 0) {
		write(fd, "out", 3);
		close(fd);
	}
}

int led_0201_init(const char* spi_device, int latch_gpio_pin) {
	gpio_latch = latch_gpio_pin;
	gpio_export(gpio_latch);
	gpio_write(gpio_latch, 0);

	spi_fd = open(spi_device, O_WRONLY);
	if (spi_fd < 0) {
		perror("SPI open failed");
		return -1;
	}

	uint8_t mode = SPI_MODE_0;
	uint8_t bits = 8;
	uint32_t speed = 1000000;

	ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
	ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

	return 0;
}

void led_0201_show_raw(int value) {
std::cout << "Wrinting: " << value << "\r\n";
	if (spi_fd < 0) return;
std::cout << "spi_fd abierto\r\n";
	uint8_t bytes[2] = {
		(uint8_t)((value >> 8) & 0xFF),
		(uint8_t)(value & 0xFF)
	};

	write(spi_fd, bytes, 2);
std::cout << "Escrito\r\n";
	gpio_write(gpio_latch, 0);
	usleep(1);
	gpio_write(gpio_latch, 1);
	usleep(1);
	gpio_write(gpio_latch, 0);
}

void led_0201_show_number(int number) {
	//led_0201_cleanup();
	if (number < 0 || number > 99) return;
	
	int units = number / 10;
	int tens = number % 10;
	
	uint8_t byte1 = digits[tens];
	uint8_t byte2 = digits[units];

	uint16_t packed = (byte1 << 8) | byte2;
	led_0201_show_raw(packed);
}

void led_0201_cleanup() {
	if (spi_fd >= 0) {
		close(spi_fd);
		spi_fd = -1;
	}
}
