#ifndef STATES_H
#define STATES_H

typedef struct StateData_t *StateData;

typedef void* StateDataElement;
typedef void* StateKeyElement;

typedef void* VoteDataElement;
typedef void* VoteKeyElement;

StateDataElement copyStateDataElement(StateDataElement data);
StateKeyElement copyStateKeyElement(StateKeyElement key);
void freeStateDataElement(StateDataElement data);
void freeStateKeyElement(StateKeyElement key);
int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2);

VoteDataElement copyVoteDataElement(VoteDataElement data);
VoteKeyElement copyVoteKeyElement(VoteKeyElement key);
void freeVoteDataElement(VoteDataElement data);
void freeVoteKeyElement(VoteKeyElement key);
int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2);

#endif //STATES_H
