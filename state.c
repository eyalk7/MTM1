#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"
#include "functions.h"
#include "state.h"

struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = stateId, data = no. of votes this state gives
};

/***************************************************** STATE MAP FUNCTIONS ************************************************************/

// copyStateDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
StateDataElement copyStateDataElement(StateDataElement data) {
    //memory allocation for StateData element and check
    StateData copy = malloc(sizeof(*copy));
    if (!copy) return NULL;

    //memory allocation for strings and check
    copy->name = malloc(sizeof(*(copy->name)));
    if (!copy->name) {
        free(copy);
        return NULL;
    }
    copy->song_name = malloc(sizeof(*(copy->song_name)));
    if (!copy->song_name) {
        free(copy->name);
        free(copy);
        return NULL;
    }

    //mapCopy for tmp_votes maps and check
    copy->votes = mapCopy(((StateData)data)->votes);
    if(!copy->votes) {
        free(copy->name);
        free(copy->song_name);
        free(copy);
        return NULL;
    }

    //initialize the element
    strcpy(copy->name, ((StateData)data)->name);
    strcpy(copy->song_name, ((StateData)data)->song_name);

    //return address
    return copy;
}
// copyStateKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
StateKeyElement copyStateKeyElement(StateKeyElement key) {
    return copyInteger(*((int*)key));
}
// freeStateDataElement - Function pointer to be used for removing data elements from the map
void freeStateDataElement(StateDataElement data) {
    //mapDestroy the Votes map
    //free strings alocations
    //free stateData alocation
}
// freeStateKeyElement - Function pointer to be used for removing key elements from the map
void freeStateKeyElement(StateKeyElement key) {
    //free allocation
}
// compareStateKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2) {
    //outside function
}

/***************************************************** VOTE MAP FUNCTIONS ************************************************************/

// copyVotesDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
VoteDataElement copyVoteDataElement(VoteDataElement data) {
    //outside function
}
// copyVotesKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
VoteKeyElement copyVoteKeyElement(VoteKeyElement key) {
    //outside function
}
// freeVotesDataElement - Function pointer to be used for removing data elements from the map
void freeVoteDataElement(VoteDataElement data) {
    //free
}
// freeVotesKeyElement - Function pointer to be used for removing key elements from the map
void freeVoteKeyElement(VoteKeyElement key) {
    //free
}
// compareVotesKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2) {
    //outside function
}
