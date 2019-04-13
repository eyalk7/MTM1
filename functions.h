#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "map.h"
#include "eurovision.h"
#include "judge.h"

#define SPACE ' '
typedef enum mapType_t {
    STATES_MAP,
    JUDGES_MAP
} MapType;

bool isIDValid(Map map, MapType type, int id);
bool isLowerCase(char c);
bool checkValidName(char* name);
bool resultsContain (Map states, Map judges, int judge_id, int state_id);

int* copyInteger(int num);
int compareIntegers(int a, int b);

#endif //FUNCTIONS_H
