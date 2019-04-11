#ifndef HW1_MTM_JUDGES_H
#define HW1_MTM_JUDGES_H

typedef struct JudgeData_t *JudgeData;

typedef void* JudgeDataElement;
typedef void* JudgeKeyElement;

JudgeDataElement copyJudgeDataElement(JudgeDataElement data);
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key);
void freeJudgeDataElement(JudgeDataElement data);
void freeJudgeKeyElement(JudgeKeyElement key);
int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2);


#endif //HW1_MTM_JUDGES_H
