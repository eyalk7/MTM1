#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

/*
 * These are included in functions.h:
 * #include "judge.h"
 */


/********************** JUDGE MAP FUNCTIONS ***********************/
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key) {
    return copyInt(key);    // get a copy of judge's ID
}

JudgeDataElement copyJudgeDataElement(JudgeDataElement data) {
    JudgeData judge_data = (JudgeData)data;

    // create JudgeData copy
    JudgeData copy = malloc(sizeof(*copy));
    if (!copy) return NULL;

    // allocate memory for judge's name
    copy->name = malloc(strlen(judge_data->name) + 1);
    if (!copy->name) {
        free(copy);
        return NULL;
    }

    strcpy(copy->name, judge_data->name);   // copy the name

    // copy the judge's points/results
    for (int i = 0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        copy->results[i] = judge_data->results[i];
    }

    return copy;
}

void freeJudgeKeyElement(JudgeKeyElement key) {
    freeInt(key);   // free memory allocation for judge's ID
}

void freeJudgeDataElement(JudgeDataElement data) {
    JudgeData judge_data = (JudgeData)data;
    free(judge_data->name); // deallocate the judge's name
    free(judge_data);       // deallocate the JudgeData struct
}

int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2) {
    return compareInts(key1, key2); // compare two judges' IDs
}
