#ifndef CUBE_H
#define CUBE_H

#include <Arduino.h>
#include "Activity.h"
#include "../ClickButton/ClickButton.h"

#define SH_CLK 9
#define SH_DTA 8
#define SH_LTC 7

#define STACK_CAPACITY 5
#define WIDTH  7
#define HEIGHT 5
#define FRAME_SIZE 10

#define BTN 2
#define LED_DEBUG 13
#define LED_BTN_G 3
#define LED_BTN_R 4
#define LED_SYS_G 5
#define LED_SYS_R 6
#define POT A0

class Cube{
public:
	Cube() : button(2, HIGH) {}
	void begin();
	void print();
	void clear();
	bool isBusy();
	void run();
	bool pushActivity(Activity * g);
	Activity * peekActivity();
	Activity * popActivity();
	void update();
	bool getPixel(uint8_t x, uint8_t y);
	void setPixel(uint8_t x, uint8_t y, uint8_t val);
	uint8_t * getFrame ();
	void blinkLED(byte targetPin, int numBlinks, int blinkRate);
	void blinkRegion(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t blinkInterval,uint8_t *data);
private:
	uint8_t frame[FRAME_SIZE];
	bool busy;
	Activity * stack[STACK_CAPACITY];
	int8_t stackCounter;
	ClickButton button;
	uint16_t activityTimestep;
	long time;
	long time_blink;
	uint8_t x,y,w,h, blinkInterval, * blinkData;
};


#endif
