#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"
#include "state.h"
#include "functions.h"

/***************************************************** STATE MAP FUNCTIONS ************************************************************/

// copyStateDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
StateDataElement copyStateDataElement(StateDataElement data) {
    // casting
    StateData state_data = (StateData)data;

    //memory allocation for StateData element and check
    StateData copy = malloc(sizeof(*copy));
    if (!copy) return NULL;

    //memory allocation for strings and check
    copy->name = malloc(strlen(state_data->name) + 1);
    if (!copy->name) {
        free(copy);
        return NULL;
    }
    copy->song_name = malloc(strlen(state_data->song_name) + 1);
    if (!copy->song_name) {
        free(copy->name);
        free(copy);
        return NULL;
    }

    //mapCopy for tmp_votes maps and check
    copy->votes = mapCopy(state_data->votes);
    if(!copy->votes) {
        free(copy->name);
        free(copy->song_name);
        free(copy);
        return NULL;
    }

    //initialize the element
    strcpy(copy->name, state_data->name);
    strcpy(copy->song_name, state_data->song_name);

    //return address
    return copy;
}
// copyStateKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
StateKeyElement copyStateKeyElement(StateKeyElement key) {
    return copyInteger(*((int*)key));
}
// freeStateDataElement - Function pointer to be used for removing data elements from the map
void freeStateDataElement(StateDataElement data) {
    StateData state_data = (StateData)data;
    //mapDestroy the Votes map
    mapDestroy(state_data->votes);
    //free strings allocations
    free(state_data->name);
    free(state_data->song_name);
    //free stateData allocation
    free(state_data);
}
// freeStateKeyElement - Function pointer to be used for removing key elements from the map
void freeStateKeyElement(StateKeyElement key) {
    //free allocation
    free(key);
}
// compareStateKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2) {
    //outside function
    return compareIntegers(*((int*)key1), *((int*)key2));
}


/***************************************************** VOTE MAP FUNCTIONS ************************************************************/

// copyVotesDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
VoteDataElement copyVoteDataElement(VoteDataElement data) {
    //outside function
    return copyInteger(*((int*)data));
}
// copyVotesKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
VoteKeyElement copyVoteKeyElement(VoteKeyElement key) {
    //outside function
    return copyInteger(*((int*)key));
}
// freeVotesDataElement - Function pointer to be used for removing data elements from the map
void freeVoteDataElement(VoteDataElement data) {
    //free
    free(data);
}
// freeVotesKeyElement - Function pointer to be used for removing key elements from the map
void freeVoteKeyElement(VoteKeyElement key) {
    //free
    free(key);
}
// compareVotesKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2) {
    return compareIntegers(*((int*)key1), *((int*)key2));
}
