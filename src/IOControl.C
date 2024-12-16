#include "IOControl.h"
#include <iostream>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>

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

void
IOControl::send_data(int busN, uint8_t address, uint16_t value)
{
  int bus;
  if(busN == 1){
    bus = bus1;
  }else{
    bus = bus2;
  }
  uint8_t command[] = {
    (uint8_t)(0xC0 + (value & 0x1F)),
    (uint8_t)((value >> 5) & 0x7F),
  };
  struct i2c_msg message = {
    address,
    0,
    sizeof(command),
    command
  };
  struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };
  /*int result = */ioctl(bus, I2C_RDWR, &ioctl_data);
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
	strncpy(lcdPedal[pedal], name, 8);
	std::cout << "Pedal " << pedal << " - " << lcdPedal[pedal] << "\r\n";
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
	std::cout << "Pedal " << pedal << " - ";
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
IOControl::setPresetName(const char* name)
{
	strncpy(lcdPreset, name, 16);
	std::cout << "preset " << " - " << lcdPreset << "\r\n";
	//const uint8_t address = 27;
	//send_data(bus1, address, lcdPreset);
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
