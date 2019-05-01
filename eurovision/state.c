#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

/*
 * These are included in functions.h:
 * #include "map.h"
 * #include "state.h"
 * #include <stdbool.h>
 */

/**
 * Implementation of state.h
 */

struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = State's ID, data = no. of votes this state *gives*
};

/************************* VOTE MAP DEFINITIONS AND FUNCTION DECLARATIONS (STATIC) *******************************/
typedef void* VoteKeyElement;
typedef void* VoteDataElement;

/**
 * Copy function for the key element in the Votes map.
 * @param key - ID of the state that receives the votes
 * @return A copy of the state ID
 */
static VoteKeyElement copyVoteKeyElement(VoteKeyElement key);

/**
 * Copy function for the data element in the Votes map.
 * @param data - The amount of votes to give
 * @return A copy of the amount of votes
 */

static VoteDataElement copyVoteDataElement(VoteDataElement data);

/**
 * Function for deallocating the key element in the Votes map.
 * @param key - ID of the state that receives the votes
 */
static void freeVoteKeyElement(VoteKeyElement key);
/**
 * Function for deallocating the data element in the Votes map.
 * @param data - The amount of votes to give
 */
static void freeVoteDataElement(VoteDataElement data);

/***
 * Compare function for two keys in Votes map.
 * @param key1 - A state's ID
 * @param key2 - A state's ID
 * @return
 *   Positive integer if first key is bigger than the second
 *   Negative integer if first key is smaller than the second
 *   0 if keys are equal
 */
static int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2);

/************************* STATE MAP FUNCTIONS *******************************/
StateKeyElement copyStateKeyElement(StateKeyElement key) {
    return copyInt(key);    // get a copy of state's ID
}

StateDataElement copyStateDataElement(StateDataElement data) {
    StateData state_data = (StateData)data;

    // create StateData copy
    StateData copy = malloc(sizeof(*copy));
    if (!copy) return NULL;

    // allocate memory for state's name
    copy->name = malloc(strlen(state_data->name) + 1);
    if (!copy->name) {
        free(copy);
        return NULL;
    }

    // allocate memory for state's song name
    copy->song_name = malloc(strlen(state_data->song_name) + 1);
    if (!copy->song_name) {
        free(copy->name);
        free(copy);
        return NULL;
    }

    // copy the State's votes map
    copy->votes = mapCopy(state_data->votes);
    if(!copy->votes) {
        free(copy->name);
        free(copy->song_name);
        free(copy);
        return NULL;
    }

    // copy the state's name and song name
    strcpy(copy->name, state_data->name);
    strcpy(copy->song_name, state_data->song_name);

    return copy;
}

void freeStateKeyElement(StateKeyElement key) {
    freeInt(key);  // free memory allocation for the state's ID
}

void freeStateDataElement(StateDataElement data) {
    StateData state_data = (StateData)data;

    mapDestroy(state_data->votes); // free the state's votes map

    // deallocate state's name and song name
    free(state_data->name);
    free(state_data->song_name);

    free(state_data);               // deallocate the stateData struct
}

int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2) {
    return compareInts(key1, key2);     // compare two states' IDs
}

/************************* STATE DATA FUNCTIONS *******************************/
StateData stateDataCreate(const char *state_name, const char *song_name) {
    // allocate memory for a StateData struct as well as the state's name and song name
    // on each allocation check if allocation failed
    StateData data = malloc(sizeof(*data));
    if (!data) return NULL;

    char *name = malloc(strlen(state_name) + 1);
    if (!name) {
        free(data);
        return NULL;
    }

    char *song = malloc(strlen(song_name) + 1);
    if (!song) {
        free(data);
        free(name);
        return NULL;
    }

    // Create an empty votes map using appropriate vote copy, free and compare functions
    Map votes = mapCreate(copyVoteDataElement,
                          copyVoteKeyElement,
                          freeVoteDataElement,
                          freeVoteKeyElement,
                          compareVoteKeyElements);
    if (!votes) {
        free(data);
        free(name);
        free(song);
        return NULL;
    }

    // copy the state and song names
    strcpy(name, state_name);
    strcpy(song, song_name);

    // set the StateData's fields accordingly
    data->name = name;
    data->song_name = song;
    data->votes = votes;

    return data;
}

char *stateGetName(StateData data) {
    return data->name;
}

Map stateGetVotes(StateData data) {
    return data->votes;
}

int stateGetFavorite(StateData state) {
    Map votes = stateGetVotes(state);

    // no votes = no favorite state
    if(mapGetSize(votes) <= 0) return NO_STATE;

    VoteKeyElement favState = mapGetFirst(votes);   // state with max no. of votes
    int max = *(int*)mapGet(votes, favState);       // max no. of votes
    MAP_FOREACH(VoteKeyElement, state_id, votes) {
        int num_of_votes = *(int*)mapGet(votes, state_id);
        if (num_of_votes > max) {   // compare votes two state's receive
            // update the most voted
            favState = state_id;
            max = num_of_votes;
        }
    }

    return *(int*)favState;     // ID of most voted state
}

/************************* VOTE MAP FUNCTIONS IMPLEMENTATION *******************************/
static VoteKeyElement copyVoteKeyElement(VoteKeyElement key) {
    return copyInt(key);    // get a copy of the state_taker's ID
}

static VoteDataElement copyVoteDataElement(VoteDataElement data) {
    return copyInt(data);   // get a copy of the no. of votes
}

static void freeVoteKeyElement(VoteKeyElement key) {
    freeInt(key);           // deallocate the state_taker's ID
}

static void freeVoteDataElement(VoteDataElement data) {
    freeInt(data);          // deallocate the no. of votes
}

static int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2) {
    return compareInts(key1, key2);     // compare two state_takers' IDs
}
