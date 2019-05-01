#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

/*
 * These are included in functions.h:
 * #include "judge.h"
 */

/**
 * Implementation of state.h
 */

struct JudgeData_t {
    char *name;
    int results[NUMBER_OF_RANKINGS];   // list of state IDs in the order of points to give
};

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
    for (int i = 0; i < NUMBER_OF_RANKINGS; i++) {
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

/********************** JUDGE DATA FUNCTIONS ***********************/
JudgeData judgeDataCreate(const char *judge_name, const int *judge_results) {
    // allocate memory for a JudgeData struct as well as judge's name
    // on each allocation check if allocation failed
    JudgeData data = malloc(sizeof(*data));
    if (!data) return NULL;

    char *name = malloc(strlen(judge_name) + 1);
    if (!name) {
        free(data);
        return NULL;
    }

    // copy the judge name
    strcpy(name, judge_name);

    // set the JudgeData's fields accordingly
    data->name = name;

    for (int i=0; i < NUMBER_OF_RANKINGS; i++) {
        data->results[i] = judge_results[i];
    }

    return data;
}

int *judgeGetResults(JudgeData data) {
    return data->results;
}

bool judgeResultsContain(JudgeData judge, int state_id) {
    for (int i=0; i < NUMBER_OF_RANKINGS; i++) {
        if (judge->results[i] == state_id) return true;
    }
    return false;
}
