#ifndef HW1_MTM_STATES_H
#define HW1_MTM_STATES_H

typedef struct StateData_t *StateData;

StateData copyStateDataElement(StateData data);
int* copyStateKeyElement(int* key);
void freeStateDataElement(StateData data);
void freeStateKeyElement(int* key);
int compareStateKeyElements(int* key1, int* key2);

int* copyVoteDataElement(int* data);
int* copyVoteKeyElement(int* key);
void freeVoteDataElement(int* data);
void freeVoteKeyElement(int* key);
int compareVoteKeyElements(int* key1, int* key2);

#endif //HW1_MTM_STATES_H
