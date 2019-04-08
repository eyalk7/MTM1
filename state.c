#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = stateId, data = no. of votes this state received
} *StateData;

// copyStateDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
    //memory allocation for StateDate
    //mapCopy for votes
    //
// copyStateKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
    //allocation for int
    //intialize the int
    //return address
// freeStateDataElement - Function pointer to be used for removing data elements from the map
    //mapDestroy the Votes map
// freeStateKeyElement - Function pointer to be used for removing key elements from the map
    //free allocation
// compareStateKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.

// copyVotesDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
// copyVotesKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
// freeVotesDataElement - Function pointer to be used for removing data elements from the map
// freeVotesKeyElement - Function pointer to be used for removing key elements from the map
// compareVotesKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
