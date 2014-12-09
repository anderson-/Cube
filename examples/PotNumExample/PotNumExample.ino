#include <ClickButton.h>
#include "Cube.h"


uint32_t numbers [] = {
	0b10100100100100100100100100100100,
	0b00111111111111111101111111001111,
	0b00101101001100100101001001011101,
	0b11111111010111111111011111001101,
	0b00001101010101001001001100001101,
	0b00001111010111111001111111001111,
};

Cube cube;

struct Item{
	void * data;
	uint8_t size;
	uint8_t lineBitLen;
	uint16_t * lookup;
};

class MatrixBuilder : public Activity {
public:
	virtual void begin(Cube * cube);
	virtual bool perform(Cube * cube, int pushes, int potVal);
	virtual uint16_t getTimestep();
protected:
	template <typename Type>
	void addDataSource(Type * data, uint8_t size);
	
	void clear();
	uint8_t append(int index, int x);
	void append(uint8_t * matrix, int bitLength, int offset, int x, int w);
	
	void printNum (uint16_t num);
	
private:
	template <typename Type>
	void calcNthBitPosAndWidthToNPlus1Bit (Type integer, uint8_t index, uint8_t * outPos, uint8_t * outWidth);
	template <typename Type>
	uint8_t placeSubMatrix (Type * subMatrix, uint8_t index, uint8_t x);
	uint8_t placeSubMatrix (void * subMatrix, uint8_t lineBitLen, uint8_t index, uint8_t x);
	template <typename Type>
	uint8_t countBits(Type integer);
	void * binarySearch (uint16_t index, void * data, uint8_t size, uint8_t lineBitLen,	uint16_t * lookup, uint8_t * bitIndex);
	
	uint8_t * frame;
	uint16_t mapSize = 0;
	Item map [3];
};

void MatrixBuilder::begin(Cube * cube){
	frame = cube->getFrame();
}

void MatrixBuilder::clear(){
	int y;
	for (y = 0; y < 10; y++){
		frame[y] = 0;
	}
}

template <typename Type>
void MatrixBuilder::calcNthBitPosAndWidthToNPlus1Bit (Type integer, uint8_t index, uint8_t * outPos, uint8_t * outWidth){
	uint8_t lineBitLen = sizeof(Type)*8, i = 0;
	Type mask = 1;
	(*outPos) = 0;
	for ((*outWidth) = 1; (*outWidth) <= lineBitLen; (*outWidth)++, mask <<= 1){
		if (mask & integer){
			if (i == index-1){
				(*outPos) = (*outWidth);
			} else if (i == index){
				(*outWidth)-= (*outPos);
				break;
			}
			i++;
		}
		if ((*outWidth) == lineBitLen){
			(*outWidth) = (*outPos) = 0;
			break;
		}
	}
}

template <typename Type>
uint8_t MatrixBuilder::placeSubMatrix (Type * subMatrix, uint8_t index, uint8_t x){
	uint8_t offset, i, w;
	uint16_t line, mask; //display size
	calcNthBitPosAndWidthToNPlus1Bit(subMatrix[0], index, &offset, &w);
	int8_t shift = offset+w+x-14;
	for (i = 0; i < 5; i++){
		if (shift < 0){
			line = subMatrix[i+1] << -shift;
		} else {
			line = subMatrix[i+1] >> shift;
		}
		if (offset-shift < 0){
			mask = (((uint16_t)1 << (w)) - 1) >> -(offset-shift);
		} else {
			mask = (((uint16_t)1 << (w)) - 1) << (offset-shift);
		}
		
		mask = ((frame[i]<<8 | frame[i+5]) & ~mask) | (line & mask);
		
    	frame[i] = mask>>8;
    	frame[i+5] = mask;
	}
	return w;
}

uint8_t MatrixBuilder::placeSubMatrix (void * subMatrix, uint8_t lineBitLen, uint8_t index, uint8_t x){
	if (lineBitLen == 8){
		return placeSubMatrix((uint8_t*)subMatrix,index,x);
	} else if (lineBitLen == 16){
		return placeSubMatrix((uint16_t*)subMatrix,index,x);
	} else if (lineBitLen == 32){
		return placeSubMatrix((uint32_t*)subMatrix,index,x);
	} else if (lineBitLen == 64){
		return placeSubMatrix((uint64_t*)subMatrix,index,x);
	}
	return 0;
}

template <typename Type>
uint8_t MatrixBuilder::countBits(Type integer){
	//Brian Kernighan Method
	uint8_t c; // c accumulates the total bits set in v
	for (c = 0; integer; c++){
		integer &= integer - 1; // clear the least significant bit set
	}
	return c;
}

template <typename Type>
void MatrixBuilder::addDataSource(Type * data, uint8_t size){
	if (mapSize < 3){
		uint16_t pos = 0;
		if (mapSize > 0){
			pos = map[mapSize-1].lookup[map[mapSize-1].size-1];
		}
		map[mapSize] = (Item){.data = data,.size = size,.lineBitLen = sizeof(Type)*8};
		uint16_t index;
		map[mapSize].lookup = (uint16_t *) malloc(size * 2);
		for (index = 0; index < size; index++){
			data += index * 6;
			pos += countBits(data[0]);
			map[mapSize].lookup[index] = pos;
		}
		mapSize++;
	}
}

void * MatrixBuilder::binarySearch (uint16_t index, void * data, uint8_t size, uint8_t lineBitLen,	uint16_t * lookup, uint8_t * bitIndex){
	int imin = 0, imax = size-1;
	while (imax >= imin){
		//printf("[%d,%d]\n",imin,imax);
		int imid = (imin + imax)/2;
		if((imid == 0 || lookup[imid-1] <= index) && index < lookup[imid]){
			*bitIndex = (imid > 0? index - lookup[imid-1] : 255);
			//printf("[%d] %d\n",imid, *bitIndex);
			return &((uint8_t*)data)[imid * lineBitLen/8 * 6];
		} else if (index >= lookup[imid]){
			imin = imid + 1;
		} else {
			imax = imid - 1;
		}
	}
    return NULL;
}

uint8_t MatrixBuilder::append(int index, int x){
	uint8_t mapIndex;
	for (mapIndex = 0; mapIndex < mapSize; mapIndex++){
		if ((mapIndex == 0 || index >= map[mapIndex-1].lookup[map[mapIndex-1].size-1]) &&
			index < map[mapIndex].lookup[map[mapIndex].size-1]){
			Item i = map[mapIndex];
			uint8_t bitIndex, w = 0;
			void * data = binarySearch(index,i.data,i.size,i.lineBitLen,i.lookup, &bitIndex);
			bitIndex = (bitIndex != 255)? bitIndex : (index - (mapIndex == 0? 0 : (map[mapIndex-1].lookup[map[mapIndex-1].size-1])));
			if (data != NULL){
				//uint8_t ppos = (uint8_t)((uintptr_t)data - (uintptr_t)map[mapIndex].data);
				//printf("OK: %d %d %d %d\n", mapIndex, bitIndex, ppos, map[mapIndex].lookup[ppos/6/(i.lineBitLen/8)]);
				w = placeSubMatrix(data,i.lineBitLen, bitIndex,x);
			} 
			//else {
				//printf("Search FAIL\n");
			//}
			return w;
		}
	}
	//printf("Non existant index FAIL\n");
	return 0;
}

void MatrixBuilder::append(uint8_t * matrix, int bitLength, int offset, int x, int w){
	uint8_t i;
	int8_t shift = offset+w+x-14;
	uint16_t line, mask;
	for (i = 0; i < 5; i++){
		if (shift < 0){
			line = matrix[i] << -shift;
		} else {
			line = matrix[i] >> shift;
		}
		if (offset-shift < 0){
			mask = (((uint16_t)1 << (w)) - 1) >> -(offset-shift);
		} else {
			mask = (((uint16_t)1 << (w)) - 1) << (offset-shift);
		}
		mask = ((frame[i]<<8 | frame[i+5]) & ~mask) | (line & mask);
		
    	frame[i] = mask>>8;
    	frame[i+5] = mask;
	}
}

void MatrixBuilder::printNum (uint16_t num){
	uint8_t d;
	uint8_t x = -2;
	d = num/1000;
	if (d){
		x += append(d,x)+1;
	} else {
		x += 4;
	}
	num -= d*1000;
	d = num/100;
	x += append(d,x)+1;
	num -= d*100;
	d = num/10;
	x += append(d,x)+1;
	num -= d*10;
	append(num,x);
}









int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


class BuilderExample : public MatrixBuilder {
public:
	virtual void begin(Cube * cube);
	virtual bool perform(Cube * cube, int pushes, int potVal);
	virtual uint16_t getTimestep();
private:
};

void BuilderExample::begin(Cube * cube){
	MatrixBuilder::begin(cube);
	addDataSource(numbers,1);
}

bool BuilderExample::perform(Cube * cube, int pushes, int potVal){
	clear();
	printNum(potVal);
	if (pushes == 1){
		clear();
		printNum(freeRam());
		cube->delay(2000);
	} else if (pushes > 1){
		clear();
		printNum(pushes);
		cube->delay(2000);
	}
	return true;
}
uint16_t BuilderExample::getTimestep(){
	return 100;
}

BuilderExample start;

void setup(){
	cube.begin();
	cube.pushActivity(&start);
}

void loop(){
	cube.run();
}
