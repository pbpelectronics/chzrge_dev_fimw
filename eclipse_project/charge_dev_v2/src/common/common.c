#include "common.h"
#include "../adc/adc.h"
#include "../display/images_printing.h"

void delayMs(int delayTime) {
	uint64_t nextTime = time_ms + delayTime;
	while (time_ms < nextTime) {
		__NOP();
	}
}

int getMax(int val1, int val2) {
	if (val1 >= val2) {
		return val1;
	} else {
		return val2;
	}
}

void Int2Str(char *pStr, unsigned int value, int charCount) {
	pStr = pStr + (charCount - 1);
	do {
		*pStr-- = (value % 10) + '0';
		value /= 10;
	} while (charCount--);
}

bool isBattaryConnected() {
	return uFromBattery >= MIN_BATTERY_VOLTAGE;
}

void checkIfBattaryIsConnected() {
	if (isBattaryConnected()) {
		return;
	}

	BATTARY_POWER_OUT = 0;
	printBatteryWaitImage();
	while (!isBattaryConnected()) {
		__NOP();
	}
	nlcd_Clear();
	printMainImageTemplate();
}

void checkAmperage() {
	if (amperage > MAX_AMPERAGE) {
		BATTARY_POWER_OUT = 0;
	}
}

void enableCooler() {

}

void checkSystemTemperature() {
	if (systemTemperature >= TEMPERATURE_FOR_ENABLING_COOL) {
		enableCooler();
	}
	if (systemTemperature <= TEMPERATURE_FOR_DISABLING_COOL) {
		disableCooler();
	}
}