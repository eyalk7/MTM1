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

StateData createStateData(const char *state_name, const char *song_name) {
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

/************************* VOTE MAP FUNCTIONS *******************************/
VoteKeyElement copyVoteKeyElement(VoteKeyElement key) {
    return copyInt(key);    // get a copy of the state_taker's ID
}

VoteDataElement copyVoteDataElement(VoteDataElement data) {
    return copyInt(data);   // get a copy of the no. of votes
}

void freeVoteKeyElement(VoteKeyElement key) {
    freeInt(key);           // deallocate the state_taker's ID
}

void freeVoteDataElement(VoteDataElement data) {
    freeInt(data);          // deallocate the no. of votes
}

int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2) {
    return compareInts(key1, key2);     // compare two state_takers' IDs
}

/********************** FRIENDLY STATE FUNCTIONS ***********************/
int stringCompare(void* str1, void* str2) {
    return strcmp(str1, str2);  // lexicographical comparison
}

int getFavoriteState(Map votes) {
    // no votes = no favorite state
    if(mapGetSize(votes) <= 0) return NO_FAVORITE_STATE;

    VoteKeyElement favState = mapGetFirst(votes);   // state with max no. of votes
    int max = *(int*)mapGet(votes, favState);       // max no. of votes
    MAP_FOREACH(VoteKeyElement, state, votes) {
        int num_of_votes = *(int*)mapGet(votes, state);
        if (num_of_votes > max) {   // compare votes two state's receive
            // update the most voted
            favState = state;
            max = num_of_votes;
        }
    }

    return *(int*)favState;     // ID of most voted state
}

bool areFriendlyStates(const int *stateId1, const int *favState1,
                       const int *stateId2, const int *favState2) {
    // if received NULL pointer return false
    if (!stateId1 || !favState1 || !stateId2 || !favState2) return false;

    // check if first state's most voted state is the second state and vice-versa
    return (*stateId1 == *favState2 && *stateId2 == *favState1);
}

Map getStateFavorites(Map states) {
    // create a map that matches each state to its most voted state
    // (key = state ID, value = favorite state ID)
    Map state_favorites = mapCreate(copyInt, copyInt,
                                    freeInt, freeInt,
                                    compareInts);
    if (!state_favorites) return NULL;

    // initialize the favorite states map
    MAP_FOREACH(int*, stateId, states) {
        // for each state
        StateData state = mapGet(states, stateId);
        if (!state) {
            mapDestroy(state_favorites);
            return NULL;
        }

        // get the ID of the state's most voted state
        int favState = getFavoriteState(state->votes);

        // insert the state along with it's most voted state to the favorite states map
        MapResult result = mapPut(state_favorites, stateId, &favState);
        if (result != MAP_SUCCESS) {
            mapDestroy(state_favorites);
            return NULL;
        }
    }

    return state_favorites;
}

char *getStatePair(StateData state1, StateData state2) {
    // get the states' names
    char *name1 = state1->name;
    char *name2 = state2->name;

    // get the lengths of the states' names
    int len1 = strlen(name1);
    int len2 = strlen(name2);

    // allocate memory for the friendly states string
    char *statePair = malloc(len1 + len2 + NUM_OF_EXTRA_CHARS + 1);
    if (!statePair) return NULL;
    statePair[0] = '\0';    // initialize as empty string

    // order the states' names lexicographically
    char *min = name2, *max = name1;
    if (strcmp(name1, name2) < 0) {
        min = name1;
        max = name2;
    }

    // build the friendly states string
    // FORMAT = "{smaller state's name} - {bigger state's name}"
    strcat(statePair, min);
    strcat(statePair, EXTRA_CHARS);
    strcat(statePair, max);

    return statePair;
}

List getFriendlyStates(Map states) {
    // create empty string list
    List friendly_states = listCreate(copyString, freeString);
    if (!friendly_states) return NULL;  // allocation failed

    // get state favorites map - key = state's ID, value = favorite state's ID
    Map state_favorites = getStateFavorites(states);
    if (!state_favorites) {
        listDestroy(friendly_states);
        return NULL;                    // allocation failed
    }

    // for each state in state favorites
    MAP_FOREACH(int*, stateId, state_favorites) {
        // get its favorite state's ID
        int *favState1 = mapGet(state_favorites, stateId);
        // get the ID of the favorite state's favorite state
        int *stateId2 = favState1;
        int *favState2 = mapGet(state_favorites, stateId2);

        // check if the states are favorites of each other
        // (outside function also checks if pointer are NULL)
        if (areFriendlyStates(stateId, favState1, stateId2, favState2)) {
            // if states are a pair get their data
            StateData state1 = mapGet(states, stateId);
            StateData state2 = mapGet(states, stateId2);

            /// mark as if they have no favorite state **to prevent duplicates**
            *favState1 = NO_FAVORITE_STATE;
            *favState2 = NO_FAVORITE_STATE;

            // create the string that contains the state names (ordered lexicographically)
            char *statePair = getStatePair(state1, state2);
            if (!statePair) {
                mapDestroy(state_favorites);
                listDestroy(friendly_states);
                return NULL;            // allocation failed
            }

            // add the string to the list
            ListResult result = listInsertLast(friendly_states, statePair);

            free(statePair);        // deallocate the string with the state names

            if (result != LIST_SUCCESS) {
                mapDestroy(state_favorites);
                listDestroy(friendly_states);
                return NULL;            // list insert failed
            }
        }
    }

    mapDestroy(state_favorites);    // destroy state favorites map

    return friendly_states;
}

