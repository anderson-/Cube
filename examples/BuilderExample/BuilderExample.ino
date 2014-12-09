#include <ClickButton.h>
#include "Cube.h"

Cube cube;

class MatrixBuilder : public Activity {
public:
	virtual void begin(Cube * cube);
	virtual bool perform(Cube * cube, int pushes, int potVal);
	virtual uint16_t getTimestep();
protected:
	void addDataSource();
	void append(int index);
	void append(int index, int offset);
	void append(uint8_t * matrix, int bitLength, int offset);
	void append(void * matrix, int bitlineLength, int bitLength, int offset);

private:
};

 int bitCount ( int value) {
     int count = 0;
    while (value > 0) {           // until all bits are zero
        if ((value & 1) == 1)     // check lower bit
            count++;
        value >>= 1;              // shift bits, removing lower bit
    }
    return count;
}


uint8_t num [] {
	0b00010001,
	0b00011111,
	0b00000011,
	0b00011111,
	0b00010001,
	0b00011111,
};

uint16_t num2 [] {
	0b1000100010001000,
	0b1110111110011111,
	0b1000100010010001,
	0b1111111111110111,
	0b1001000100010001,
	0b1111111100011111,
};

uint32_t num3 [] {
	0b00001000000100000000100010001000,
	0b00000110011001100110111111111111,
	0b00000000000001100110100110010001,
	0b00001000000110000001111111110010,
	0b00000100001001000010000110010100,
	0b00000011110000111100000111111000,
};

class BuilderExample : public MatrixBuilder {
public:
	virtual void begin(Cube * cube);
	virtual bool perform(Cube * cube, int pushes, int potVal);
	virtual uint16_t getTimestep();
private:
};

void BuilderExample::begin(Cube * cube){
}

bool BuilderExample::perform(Cube * cube, int pushes, int potVal){
	return true;
}
uint16_t BuilderExample::getTimestep(){
	return 10;
}

BuilderExample start;

void setup(){
	cube.begin();
	cube.pushActivity(&start);
}

void loop(){
	cube.run();
}