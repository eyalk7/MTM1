#ifndef STATES_H
#define STATES_H

typedef struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = stateId, data = no. of votes this state gives
} *StateData;


typedef void* StateDataElement;
typedef void* StateKeyElement;

typedef void* VoteDataElement;
typedef void* VoteKeyElement;

/********************** STATE MAP FUNCTIONS ***********************/

StateDataElement copyStateDataElement(StateDataElement data);
StateKeyElement copyStateKeyElement(StateKeyElement key);
void freeStateDataElement(StateDataElement data);
void freeStateKeyElement(StateKeyElement key);
int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2);



/********************** VOTE MAP FUNCTIONS ***********************/

VoteDataElement copyVoteDataElement(VoteDataElement data);
VoteKeyElement copyVoteKeyElement(VoteKeyElement key);
void freeVoteDataElement(VoteDataElement data);
void freeVoteKeyElement(VoteKeyElement key);
int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2);

#endif //STATES_H
