#ifndef JUDGES_H
#define JUDGES_H

#include <stdbool.h>

#define NUMBER_OF_STATES_TO_RANK 10

typedef struct JudgeData_t *JudgeData;

typedef void* JudgeDataElement;
typedef void* JudgeKeyElement;

JudgeDataElement copyJudgeDataElement(JudgeDataElement data);
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key);
void freeJudgeDataElement(JudgeDataElement data);
void freeJudgeKeyElement(JudgeKeyElement key);
int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2);

JudgeData createJudgeData();
void setJudgeName(JudgeData data, char* name);
void setJudgeResults(JudgeData data, int results[]);


bool resultsContainState(JudgeDataElement judge, int stateId);

#endif //JUDGES_H
