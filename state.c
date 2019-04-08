#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"

typedef struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = stateId, data = no. of votes this state gives
} *StateData;

// copyStateDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
    //memory allocation for StateDate element and check
    //memory allocation for strings and check
    //mapCopy for tmp_votes maps and check
    //initialize the element
    //return address
// copyStateKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
    //outside function
        //allocation for int
        //intialize the int
        //return address
// freeStateDataElement - Function pointer to be used for removing data elements from the map
    //mapDestroy the Votes map
    //free strings alocations
    //free stateData alocation
// freeStateKeyElement - Function pointer to be used for removing key elements from the map
    //free allocation
// compareStateKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
    //outside function
        //compare the ints
        //return 0 if equal
        //return bigger than 0 if first is bigger
        //else the small than 0

// copyVotesDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
    //outside function
        //allocation for int
        //intialize the int
        //return address
// copyVotesKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
    //outside function
        //allocation for int
        //intialize the int
        //return address
// freeVotesDataElement - Function pointer to be used for removing data elements from the map
    //free
// freeVotesKeyElement - Function pointer to be used for removing key elements from the map
    //free
// compareVotesKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
    //outside function
        //compare the ints
        //return 0 if equal
        //return bigger than 0 if first is bigger
        //else the small than 0