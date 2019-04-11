#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "functions.h"
#include "judge.h"

#define NUMBER_OF_STATES_TO_RANK 10

struct JudgeData_t {
    char *name;
    unsigned int states[NUMBER_OF_STATES_TO_RANK];   // list of stateIds in order of points to give
};


// copyJudgeDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
JudgeDataElement copyJudgeDataElement(JudgeDataElement data) {
    //memory allocation for the JudgeData and check
    JudgeData copy = malloc(sizeof(*copy));
    if (!copy) return NULL;
    //memory allocation for name string and check
    copy->name = malloc(sizeof(*(copy->name)));
    if (!copy->name) {
        free(copy);
        return NULL;
    }
    //copy name and results
    strcpy(copy->name, ((JudgeData)data)->name);

    for (int i = 0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        copy->states[i] = ((JudgeData)data)->states[i];
    }

    //return address
    return copy;
}

// copyJudgeKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key) {
    return copyInteger(*((int*)key));
}

// freeJudgeDataElement - Function pointer to be used for removing data elements from the map
void freeJudgeDataElement(JudgeDataElement data) {
    //free strings allocation
    //free judgeData allocation
}

// freeJudgeKeyElement - Function pointer to be used for removing key elements from the map
void freeJudgeKeyElement(JudgeKeyElement key) {
    //free allocation
}

// compareJudgeKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareJudgeKeyElements(int* key1, int* key2) {
    return compareIntegers(*key1, *key2);
}
