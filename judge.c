#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "judge.h"

/********************** JUDGE MAP FUNCTIONS ***********************/
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key) {
    return copyInt(key);
}

JudgeDataElement copyJudgeDataElement(JudgeDataElement data) {
    // casting
    JudgeData judge_data = (JudgeData)data;

    //memory allocation for the JudgeData and check
    JudgeData copy = malloc(sizeof(*copy));
    if (!copy) return NULL;
    //memory allocation for name string and check
    copy->name = malloc(strlen(judge_data->name) + 1);
    if (!copy->name) {
        free(copy);
        return NULL;
    }
    //copy name and results
    strcpy(copy->name, judge_data->name);

    for (int i = 0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        copy->results[i] = judge_data->results[i];
    }

    //return address
    return copy;
}

void freeJudgeKeyElement(JudgeKeyElement key) {
    //free allocation
    freeInt(key);
}

void freeJudgeDataElement(JudgeDataElement data) {
    JudgeData tmp = (JudgeData)data;
    //free strings allocation
    free(tmp->name);
    //free judgeData allocation
    free(tmp);
}

int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2) {
    return compareInts(key1, key2);
}
