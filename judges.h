#ifndef HW1_MTM_JUDGES_H
#define HW1_MTM_JUDGES_H

typedef struct JudgeData_t *JudgeData;

typedef void* JudgeDataElement
typedef void* JugeKeyElement


JudgeDataElement copyJudgeDataElement(JudgeDataElement data);
int* copyJudgeKeyElement(int* key);
void freeJudgeDataElement(JudgeData data);
void freeJudgeKeyElement(int* key);
int compareJudgeKeyElements(int* key1, int* key2);


#endif //HW1_MTM_JUDGES_H
