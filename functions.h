#ifndef HW1_MTM_FUNCTIONS_H
#define HW1_MTM_FUNCTIONS_H

#include <stdbool.h>
#include "map.h"

#define SPACE ' '

bool isIDValid(Map map, int id);
bool isLowerCase(char c);
bool checkValidName(char* name);

int* createIntegerCopy(int num);
int compareIntegers(int a, int b);

#endif //HW1_MTM_FUNCTIONS_H
