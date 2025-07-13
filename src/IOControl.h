#ifndef IOCONTROL_H
#define IOCONTROL_H

class IOControl
{
public:
	IOControl ();
	void setPedalName(int pedal, const char* name);
	void setPedalStatus(int pedal, int status);
	void setPresetName(const char* name);
	void setPresetNumber(int number);
	void setFXStatus(bool status);
	void setLooperName(int status);
private:
	int num[11]; //0-9=pedal status; 10=FX status; 11=Preset number
	char lcdPedal[9]; //Pedal LCD
	char lcdPreset[17]; //Preset LCD
	char lcdLooper[5]; //Looper LCD
	void sendTo16LCD(int bus, uint8_t address, char* text);
	void sendTo8LCD(int bus, uint8_t address, char* text);
	void sendTo2LED(int bus, uint8_t address, int number);
	void sendTo4LED(int bus, uint8_t address, char* text);
	void sendToWS2812B(int address, int status);
	const char *charBus1;
	const char *charBus2;
	int bus1;
	int bus2;
	int open_i2c_device(const char * device);
};

#endif
