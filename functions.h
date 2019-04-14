#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "map.h"
#include "eurovision.h"
#include "judge.h"
#include "list.h"

#define SPACE ' '

typedef enum mapType_t {
    STATES_MAP,
    JUDGES_MAP
} MapType;

typedef struct countData_t {
    unsigned int id;
    int count;
} *CountData;


EurovisionResult isIDValid(Map map, MapType type, int id);
bool isLowerCase(char c);
bool checkValidName(const char* name);
bool resultsContain (Map states, Map judges, int judge_id, int state_id);

int* copyInteger(int num);
int compareIntegers(int a, int b);

/********************** COUNT TABLE FUNCTIONS ***********************/

CountData* countTableCreate(Map map);
CountData* convertVotesToCountTable(Map votes);
void sortCountTable(CountData* table);
void freeCountTable(CountData* countTable);
List convertCountTableToList(CountData* countTable);

#endif //FUNCTIONS_H
