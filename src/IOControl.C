#include "IOControl.h"
#include <iostream>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include "lcd1601.c"
#include "led0401.c"
#include "led0201.c"
#include <wiringPi.h>

uint8_t address[10] = {0x27, 0x28, 0x29, 0x30, 0x31, 0x32, 0x32, 0x33, 0x34, 0x35};

void
IOControl::sendTo8LCD(int bus, uint8_t address, char* text)
{
	std::cout << "Pedal Name - " << text << "\r\n";
	//lcd0801WriteString(bus,address,text);
};

void 
IOControl::sendTo16LCD(int bus, uint8_t address, char* text)
{
	std::cout << "preset - " << text << "\r\n";
	lcd1601WriteString(bus,address,text);
};

void
IOControl::sendTo2LED(int bus, uint8_t address, int number)
{
	std::cout << "preset number - " << number << "\r\n";
}

void
IOControl::sendTo4LED(int bus, uint8_t address, char* text)
{
	std::cout << "Looper - " << text << "\r\n";
	led0401write(bus,address,text);
}

void
IOControl::setPresetName(const char* name)
{
	snprintf(lcdPreset, sizeof(lcdPreset), "%s", name);
	sendTo16LCD(bus1, 0x27, lcdPreset);
	setLooperName(5);
};

IOControl::IOControl(){
	//Encender ventilador
	wiringPiSetup(); // o wiringPiSetupGpio() si usas número BCM
    pinMode(21, OUTPUT); // Usando BCM GPIO17
    digitalWrite(21, HIGH); // Activar el pin
	
	const char *charBus1 = "/dev/i2c-1";
	const char *charBus2 = "/dev/i2c-2";
	bus1 = open_i2c_device(charBus1);
	bus2 = open_i2c_device(charBus2);
	init_ht16k33(bus1, 0x70);
	init_lcd(bus1, 0x27);
	/*
	for todos los address[10], init_lcd(bus2, address[cont]);
	*/
};

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
IOControl::setPedalName(int pedal, const char* name)
{
	snprintf(lcdPedal, sizeof(lcdPedal), "%s", name);
	sendTo8LCD(bus2, address[pedal], lcdPedal);
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
	sendTo2LED(bus1, 0x71, number);
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
    *  5 = "    "
    */
	switch(status)
	{
        case 0:
            //lcdLooper = "STOP";
			strncpy(lcdLooper, "STOP", 5);
            break;
        case 1:
            //lcdLooper = "RECO";
			strncpy(lcdLooper, "RECO", 5);
            break;
        case 2:
            //lcdLooper = "PLAY";
			strncpy(lcdLooper, "PLAY", 5);
            break;
        case 3:
            //lcdLooper = "PAUS";
			strncpy(lcdLooper, "PAUS", 5);
            break;
		case 4:
            //lcdLooper = "PLPS";
			strncpy(lcdLooper, "PLPS", 5);
            break;
		case 5:
            //lcdLooper = "    ";
			strncpy(lcdLooper, "    ", 5);
            break;
    }
	sendTo4LED(bus1, 0x70, lcdLooper);
};
