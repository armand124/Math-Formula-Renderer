#pragma once
#include <iostream>
namespace str {

	const static unsigned short int MAX_SIZE_OF_STRING = 6e4;

	bool nullPtrReceived(char* ptr){ return (*ptr == NULL); }

	unsigned short int sizeOfString(char* string) {
		unsigned short int size = 0;
		while (!nullPtrReceived((string + size))) size++;
		return size; 
	}
	
	bool isDigit(char x) {
		return int(x - '0') >= 0 && int(x - '0') <= 9;
	}

}
