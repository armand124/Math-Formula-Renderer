#include "SFML/Graphics.hpp"
#include "StringHandle.h"
#include <iostream>

char S[str::MAX_SIZE_OF_STRING];

int main()
{
    std::cin.getline(S, 100);
    std::cout << str::sizeOfString(S) << '\n';
    return 0;
}