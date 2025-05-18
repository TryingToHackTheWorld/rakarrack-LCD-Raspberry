#include "IOControl.h"
#include <iostream>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include "lcd1602.h"

int
IOControl::open_i2c_device(const char * device)
{
  int fd = open(device, O_RDWR);
  if (fd == -1)
  {
    perror(device);
    return -1;
  }
  return fd;
};

void IOControl::sendToI2C(int bus_fd, uint8_t address, const char* text, int length) {
    if (bus_fd < 0) {
        std::cerr << "Bus I2C no inicializado correctamente.\n";
        return;
    }

    if (ioctl(bus_fd, I2C_SLAVE, address) < 0) {
        std::cerr << "Error al seleccionar el dispositivo I2C en 0x"
                  << std::hex << (int)address << std::endl;
        return;
    }

    if (write(bus_fd, text, length) != length) {
        std::cerr << "Fallo al escribir en el dispositivo I2C 0x"
                  << std::hex << (int)address << std::endl;
        return;
    }
};

void
IOControl::setPresetName(const char* name)
{
	strncpy(lcdPreset, name, 16);
	std::cout << "preset " << " - " << lcdPreset << "\r\n";
	sendToI2C(bus1, 0x27, name, 16);
};

IOControl::IOControl(){
	const char *charBus1 = "/dev/i2c-1";
	const char *charBus2 = "/dev/i2c-2";
	bus1 = open_i2c_device(charBus1);
	bus2 = open_i2c_device(charBus2);
};

void
IOControl::setPedalName(int pedal, const char* name)
{
	strncpy(lcdPedal[pedal], name, 9);
	std::cout << "Pedal Name " << pedal << " - " << lcdPedal[pedal] << "\r\n";
	//int result = jrk_set_target(fd, address, new_target);
	//jrk_set_target(fd, address, new_target);
};

void
IOControl::setPedalStatus(int pedal, int status)
{
	/* 0 = both off
	*  1 = green
	*  2 = red
	*  3 = both on
	*/
	num[pedal] = status;
	std::cout << "Pedal Status " << pedal << " - ";
	switch(status)
	{
        case 0:
	    //Pedal is OFF
	    if(num[10] == 0){
		//FX is OFF
		std::cout << "OFF";
	    }else{
		//FX is ON
		std::cout << "GREEN";
	    }
            break;
        case 1:
	    //Pedal is ON
	    if(num[10] == 0){
		//FX is OFF
		std::cout << "BOTH";
	    }else{
		//FX is ON
		std::cout << "RED";
	    }
            break;
    }
	std::cout << "\r\n";
};

void
IOControl::setPresetNumber(int number)
{
    num[11] = number;
    std::cout << "preset number " << " - " << num[11] << "\r\n";
};

void
IOControl::setFXStatus(bool status)
{
    if(status){
        num[10] = 1;
        std::cout << "FX " << " - ON\r\n";
    }else{
        num[10] = 0;
        std::cout << "FX " << " - OFF\r\n";
    }
};

void
IOControl::setLooperName(int status){
    /* 0 = STOP
    *  1 = RECO
    *  2 = PLAY
    *  3 = PAUS
    *  4 = PLPS
    *  5 = ""
    */
	switch(status)
	{
        case 0:
            lcdLooper = "STOP";
            break;
        case 1:
            lcdLooper = "RECO";
            break;
        case 2:
            lcdLooper = "PLAY";
            break;
        case 3:
            lcdLooper = "PAUS";
            break;
	case 4:
            lcdLooper = "PLPS";
            break;
	case 5:
            lcdLooper = "";
            break;
    }
	std::cout << "Looper - " << lcdLooper << "\r\n";
//P=i2cset -y 1 0x70 0x00 0xf3
//L=i2cset -y 1 0x70 0x02 0x38
//A=i2cset -y 1 0x70 0x04 0xf7
//Y=i2cset -y 1 0x70 0x05 0x
//  i2cset -y 1 0x70 0x06 0x
};

void
IOControl::LCDClean(){
    for(int cont=0;cont<10;cont++){
	setPedalStatus(cont, 0);
	setPedalName(cont, "");
    }
    setLooperName(5);
};
