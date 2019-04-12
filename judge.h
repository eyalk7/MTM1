#ifndef JUDGES_H
#define JUDGES_H

#define NUMBER_OF_STATES_TO_RANK 10

typedef struct JudgeData_t {
    char *name;
    unsigned int states[NUMBER_OF_STATES_TO_RANK];   // list of stateIds in order of points to give
} *JudgeData;

typedef void* JudgeDataElement;
typedef void* JudgeKeyElement;

JudgeDataElement copyJudgeDataElement(JudgeDataElement data);
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key);
void freeJudgeDataElement(JudgeDataElement data);
void freeJudgeKeyElement(JudgeKeyElement key);
int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2);

#endif //JUDGES_H
