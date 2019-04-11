#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "map.h"

#define SPACE ' '

bool isIDValid(Map map, int id);
bool isLowerCase(char c);
bool checkValidName(char* name);

int* copyInteger(int num);
int compareIntegers(int a, int b);

#endif //FUNCTIONS_H
