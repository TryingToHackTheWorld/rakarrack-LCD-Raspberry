#include "led0401.h"

uint16_t font14seg[128] = { 0 }; // Inicializa todo a 0

void init_font14seg() {
    font14seg[' '] = 0x0000;
    font14seg['A'] = 0x0777;
    font14seg['L'] = 0x0038;
    font14seg['P'] = 0x0733;
    font14seg['S'] = 0x06D6;
    font14seg['T'] = 0x040F;
    font14seg['R'] = 0x0735;
    font14seg['C'] = 0x00D0;
    font14seg['E'] = 0x0670;
    font14seg['O'] = 0x00FC;
    font14seg['Y'] = 0x0456;
    font14seg['U'] = 0x00FC;
}

void init_ht16k33(int i2c_fd, uint8_t address) {
	
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        std::cout << "INIT FAIL\r\n";
    }else{	
		init_font14seg();
	
		uint8_t setup[2];

		// Turn on oscillator
		setup[0] = 0x21;
		write(i2c_fd, setup, 1);

		// Display on, no blinking
		setup[0] = 0x81;
		write(i2c_fd, setup, 1);

		// Full brightness
		setup[0] = 0xEF;
		write(i2c_fd, setup, 1);
	}
}

void led0401write(int bus, uint8_t address, char* text){
	int try = 0;
	
	while (try < 3){
		if (ioctl(bus, I2C_SLAVE, address) < 0) {
			std::cout << "FAIL writing\r\n";
			try++;
		}else{		
			uint8_t buffer[17];  // 0x00 register + 8x2 bytes = 17
			memset(buffer, 0, sizeof(buffer));
			buffer[0] = 0x00; // Start at address 0

			for (int i = 0; i < 4 && text[i]; ++i) {
				uint16_t segs = font14seg[(unsigned char)text[i]];
				buffer[1 + i * 2] = segs & 0xFF;
				buffer[2 + i * 2] = segs >> 8;
			}
			write(bus, buffer, sizeof(buffer));
			try = 3;
		}
	}
}
