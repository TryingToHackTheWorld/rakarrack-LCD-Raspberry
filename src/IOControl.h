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
	void LCDClean();
private:
	int num[11]; //0-9=pedal status; 10=FX status; 11=Preset number
	char lcdPedal[10][9]; //10 x LCD 8char screen
	char lcdPreset[17]; //Preset LCD
	const char *lcdLooper; //Looper LCD
	const char *charBus1;
	const char *charBus2;
	int bus1;
	int bus2;
	int open_i2c_device(const char * device);
	void sendToI2C(int bus, uint8_t address, char* text);
};

#endif
