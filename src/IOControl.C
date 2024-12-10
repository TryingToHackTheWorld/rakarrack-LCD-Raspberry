#include "IOControl.h"
#include <iostream>
#include <linux/i2c-dev.h>

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

IOControl::IOControl(){
	const char *charBus1 = "/dev/i2c-1";
	const char *charBus2 = "/dev/i2c-2";
	int bus1 = open_i2c_device(charBus1);
	int bus2 = open_i2c_device(charBus2);
};

void
IOControl::setPedalName(int pedal, const char* name)
{
	strncpy(lcdPedal[pedal], name, 8);
	std::cout << "Pedal " << pedal << " - " << lcdPedal[pedal] << "\r\n";
	//int result = jrk_set_target(fd, address, new_target);
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
};

void
IOControl::LCDClean(){
    for(int cont=0;cont<10;cont++){
	setPedalStatus(cont, 0);
	setPedalName(cont, "");
    }
    setLooperName(5);
};
