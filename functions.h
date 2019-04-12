#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "map.h"
#include "eurovision.h"

#define SPACE ' '

bool isIDValid(Map map, int id);
bool isLowerCase(char c);
bool checkValidName(char* name);
bool resultsContain (Eurovision eurovision, int judge_id, int state_id);

int* copyInteger(int num);
int compareIntegers(int a, int b);

#endif //FUNCTIONS_H
