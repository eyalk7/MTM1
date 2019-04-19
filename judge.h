#ifndef JUDGES_H
#define JUDGES_H

#include <stdbool.h>

/********************** MACROS & ENUMS ***********************/
/** number of states the judge has to rank */
#define NUMBER_OF_STATES_TO_RANK 10

/********************** JUDGE MAP DEFINITIONS ***********************/
typedef struct JudgeData_t {
    char *name;
    int results[NUMBER_OF_STATES_TO_RANK];   // list of state IDs in the order of points to give
} *JudgeData;

typedef void* JudgeDataElement;
typedef void* JudgeKeyElement;

/********************** JUDGE MAP FUNCTIONS ***********************/
/** returns a copy of the judge's id */
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key);

/** returns a copy of the judge's data (name and array of states ranked) */
JudgeDataElement copyJudgeDataElement(JudgeDataElement data);

/** frees the judge's id */
void freeJudgeKeyElement(JudgeKeyElement key);

/** frees the judge's data (name and array of states ranked) */
void freeJudgeDataElement(JudgeDataElement data);

/** compare between two judge's ids */
int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2);

#endif //JUDGES_H
