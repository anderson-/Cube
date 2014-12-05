#include <ClickButton.h>
#include "Cube.h"

Cube cube;

class PotTest : public Activity {
public:
	PotTest() : mode(1){};
	void begin(Cube * cube);
	bool perform(Cube * cube, int pushes, int potVal);
	uint16_t getTimestep();
private:
	int mode;
};


void PotTest::begin(Cube * cube){
}

bool PotTest::perform(Cube * cube, int pushes, int potVal){
	int i = map(potVal, 0, 1023, 0, 69);
	int x = i%14;
	int y = i/14;
	
	if (pushes > 0){
		mode = pushes;
	}

	switch(mode){
		case 1:
		cube->blinkRegion(x,y,1,1,5,NULL);
		break;
		case 2:
		cube->blinkRegion(x,y,1,5,20,NULL);
		break;
		case 3:
		cube->blinkRegion(x,y,3,3,1,NULL);
		break;
		case 4:
		cube->blinkRegion(x,y,14,1,5,NULL);
		break;
		case 5:
		cube->blinkRegion(x,y,7,2,30,NULL);
		break;
	}
	return true;
}
uint16_t PotTest::getTimestep(){
	return 10;
}

PotTest start;

void setup(){
	cube.begin();
	cube.pushActivity(&start);
}

void loop(){
	cube.run();
}