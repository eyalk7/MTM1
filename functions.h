#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "map.h"
#include "eurovision.h"
#include "judge.h"
#include "list.h"
#include "state.h"

#define SPACE ' '

typedef enum mapType_t {
    STATES_MAP,
    JUDGES_MAP
} MapType;

typedef struct countData_t {
    int id;
    int count;
} *CountData;


EurovisionResult isIDValid(Map map, MapType type, int id);
bool isLowerCase(char c);
bool checkValidName(const char* name);
bool resultsContain (Map states, Map judges, int judge_id, int state_id);

int* copyInteger(int num);
int compareIntegers(int a, int b);

/********************** COUNT LIST FUNCTIONS ***********************/

ListElement copyCountData(ListElement elem);
void freeCountData(ListElement elem);
int compareCountData(ListElement data1, ListElement data2);

List countListCreate(Map map);

// Assumption: Given map has keys of type int*
List countListCreate(Map map);
List convertVotesToList(Map votes);
// Converts final results to list of state names
List convertToStringlist(List finalResults, Map states);
ListElement copyString(ListElement str);
FreeListElement freeString(ListElement str);

#endif //FUNCTIONS_H
