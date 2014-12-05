#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <stdint.h>
class Cube;

class Activity {
public:
	virtual void begin(Cube * cube);
	virtual bool perform(Cube * cube, int pushes, int potVal);
	virtual uint16_t getTimestep();
protected:
		//void (*func)(uint8_t * buffer, uint8_t size);
		//virtual void update (uint8_t * buffer, uint8_t size);
};

#endif
