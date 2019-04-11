#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "functions.h"
#include "judges.h"

struct JudgeData_t {
    char *name;
    unsigned int results[10];   // list of stateIds in order of points to give
};


// copyJudgeDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
JudgeData copyJudgeDataElement(JudgeData data) {
    //memory allocation for the JudgeData and check
    JudgeData copy = malloc(sizeof(*copy));
    if (!copy) return NULL;
    //memory allocation for name string and check
    copy->name = malloc(sizeof(*(copy->name)));
    if (!copy->name) {
        free(copy);
        return NULL;
    }
    //initialize results - all ids = 0
    copy->results = { 0 };
    //return address
    return copy;
}

// copyJudgeKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
int* copyJudgeKeyElement(int* key) {
    return createIntegerCopy(*key);
}

// freeJudgeDataElement - Function pointer to be used for removing data elements from the map
void freeJudgeDataElement(JudgeData data) {
    //free strings allocation
    //free judgeData allocation
}

// freeJudgeKeyElement - Function pointer to be used for removing key elements from the map
void freeJudgeKeyElement(int* key) {
    //free allocation
}

// compareJudgeKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareJudgeKeyElements(int* key1, int* key2) {
    return compareIntegers(*key1, *key2);
}
