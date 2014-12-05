#include "Cube.h"

void Cube::begin(){
	pinMode(SH_CLK, OUTPUT);
	pinMode(SH_DTA, OUTPUT);
	pinMode(SH_LTC, OUTPUT);
	pinMode(LED_DEBUG, OUTPUT);
	pinMode(LED_BTN_G, OUTPUT);
	pinMode(LED_BTN_R, OUTPUT);
	pinMode(LED_SYS_G, OUTPUT);
	pinMode(LED_SYS_R, OUTPUT);
	time_blink = time = millis();
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	blinkInterval = 0;
	blinkData = NULL;

	busy = false;
	stackCounter = -1;
	for (int i = 0; i < STACK_CAPACITY; i++){
		stack[i] = NULL;
	}

        button.debounceTime   = 20;   // Debounce timer in ms
        button.multiclickTime = 200;  // Time limit for multi clicks
        button.longClickTime  = 500; // time until "held-down clicks" register
    }

    void print_bin (uint32_t integer){
    	for (int i = 0; i < 32; i++){
    		//Serial.print((integer >> (31-i)) & 1);
    		if ((i+1) % 8 == 0){
    			//Serial.print(" ");
    		}
    	}
    	//Serial.println();
    }

    void Cube::print(){
    	static uint8_t i = 0;
    	static  union{
    		uint8_t array[3];
    		uint32_t integer;
    	} shiftData;
    	static  union{
    		uint8_t array[2];
    		uint16_t integer;
    	} currentLine, blinkLine;
    	static bool blink = false;

    	digitalWrite(SH_LTC, 0);
    	shiftOut(SH_DTA, SH_CLK,MSBFIRST, shiftData.array[2]);
    	shiftOut(SH_DTA, SH_CLK,MSBFIRST, shiftData.array[1]);
    	shiftOut(SH_DTA, SH_CLK,MSBFIRST, shiftData.array[0]);
    	digitalWrite(SH_LTC, 1);

		//delay(10); // proteus

    	currentLine.array[0] = frame[HEIGHT+i];
    	currentLine.array[1] = frame[i];

    	if (w > 0 && h > 0){
    		if (millis()-time_blink >= blinkInterval*(
    			uint16_t)10){
    			time_blink = millis();
    		blink = !blink;
    	}
    	if (i >= y && i < y+h){
    		if (blink){
    			if (blinkData == NULL){
    				blinkLine.array[0] = ~0;
    				blinkLine.array[1] = ~0;
    			} else {
    				blinkLine.array[0] = blinkData[HEIGHT+i];
    				blinkLine.array[1] = blinkData[i];
    			}
    		} else {
    			if (blinkData == NULL){
    				blinkLine.array[0] = 0;
    				blinkLine.array[1] = 0;
    			} else {
    				blinkLine.array[0] = frame[HEIGHT+i];
    				blinkLine.array[1] = frame[i];
    			}
    		}
    		uint16_t mask = (((uint16_t)1 << (w)) - 1) << (15-w-x);
    		currentLine.integer = (currentLine.integer & ~mask) | (blinkLine.integer & mask);
    	}
    }

    shiftData.integer = ((uint32_t)((1 << i) & 0b11111) << 19) | ((uint32_t)((~(currentLine.array[0]>>1)) & 0b1111111) << 12) | ((uint32_t)((1 << i) & 0b11111) << WIDTH) | ((~currentLine.array[1]) & 0b1111111);

    if (i == 4){
    	i = 0;
    	busy = false;
    } else {
    	i++;
    	busy = true;
    }
}

void Cube::clear(){
	for (int i = 0; i < FRAME_SIZE; i++){
		frame[i] = 0;
	}
}

bool Cube::isBusy(){
	return busy;
}

void Cube::run(){
	button.Update();

	int pushes = button.clicks;
	if (pushes >= 0){
		blinkLED(6,pushes,50);
	} else {
		blinkLED(5,-pushes,200);
	}

	int potVal = analogRead(A0);

	if (millis()-time >= activityTimestep){
		time = millis();
		if (stackCounter != -1 && stack[stackCounter] != NULL){
			if (!stack[stackCounter]->perform(this, pushes, potVal)){
				popActivity();
			}
		}
		if (stackCounter != -1 && stack[stackCounter] != NULL){
			activityTimestep = stack[stackCounter]->getTimestep();
		}
	}

	print();
}

bool Cube::pushActivity(Activity * g){
	if (stackCounter >= STACK_CAPACITY || !g) return false;
	stackCounter++;
	stack[stackCounter] = g;
	g->begin(this);
	return true;
}

Activity * Cube::peekActivity(){
	if (stackCounter == -1) return NULL;
	return stack[stackCounter];
}

Activity * Cube::popActivity(){
	if (stackCounter == -1) return NULL;
	Activity * tmp = stack[stackCounter];
	stack[stackCounter] = NULL;
	stackCounter--;
	return tmp;
}

bool Cube::getPixel(uint8_t x, uint8_t y){
	if (x >= WIDTH){
		y += HEIGHT;
		x -= WIDTH;
	}
	uint8_t mask = 0b01000000 >> x;
	return frame[y] & mask;
}

void Cube::setPixel(uint8_t x, uint8_t y, uint8_t val){
	if (x >= WIDTH){
		y += HEIGHT;
		x -= WIDTH;
	}
	uint8_t mask = 0b01000000 >> x;

	if (val){
		frame[y] |= mask;
	} else if (frame[y] & mask){
		frame[y] ^= mask;
	}
}

uint8_t * Cube::getFrame (){
	return frame;
}

void Cube::blinkLED(byte targetPin, int numBlinks, int blinkRate) {
	for (int i=0; i < numBlinks; i++) {
    digitalWrite(targetPin, HIGH);   // sets the LED on
    delay(blinkRate);                     // waits for blinkRate milliseconds
    digitalWrite(targetPin, LOW);    // sets the LED off
    delay(blinkRate);
}
}

void Cube::blinkRegion(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t blinkInterval,uint8_t *blinkData){
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->blinkInterval = blinkInterval;
	this->blinkData = blinkData;
}