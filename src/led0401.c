#include "led0401.h"

uint16_t font14seg[128] = { 0 }; // Inicializa todo a 0

void init_font14seg() {
    font14seg[' '] = 0x0000;
    font14seg['A'] = 0x00F7;
    font14seg['C'] = 0x0039;
    font14seg['E'] = 0x0079;
    font14seg['L'] = 0x0038;
    font14seg['O'] = 0x003F;
    font14seg['P'] = 0x00F3;
    font14seg['R'] = 0x20F3;
    font14seg['S'] = 0x00ED;
    font14seg['T'] = 0x1201;
    font14seg['U'] = 0x003E; 
    font14seg['Y'] = 0x1500;
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
	int cont = 0;
	
	while (cont < 3){
		if (ioctl(bus, I2C_SLAVE, address) < 0) {
			std::cout << "FAIL writing\r\n";
			cont++;
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
			cont = 3;
		}
	}
}
