#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "judge.h"

// copyJudgeDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
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

// copyJudgeKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key) {
    return copyInt(key);
}
// freeJudgeDataElement - Function pointer to be used for removing data elements from the map
void freeJudgeDataElement(JudgeDataElement data) {
    JudgeData tmp = (JudgeData)data;
    //free strings allocation
    free(tmp->name);
    //free judgeData allocation
    free(tmp);
}
// freeJudgeKeyElement - Function pointer to be used for removing key elements from the map
void freeJudgeKeyElement(JudgeKeyElement key) {
    //free allocation
    freeInt(key);
}
// compareJudgeKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2) {
    return compareInts(key1, key2);
}

bool resultsContainState(JudgeDataElement judge, int state_id) {
    JudgeData tmp_judge = (JudgeData)judge;
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        if (tmp_judge->results[i] == state_id) {
            return true;
        }
    }
    return false;
}
