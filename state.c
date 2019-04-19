#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"
#include "state.h"
#include "functions.h"

/************************* STATE MAP FUNCTIONS *******************************/
StateKeyElement copyStateKeyElement(StateKeyElement key) {
    return copyInt(key);
}

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

void freeStateKeyElement(StateKeyElement key) {
    //free allocation
    freeInt(key);
}

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

int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2) {
    //outside function
    return compareInts(key1, key2);
}

/************************* VOTE MAP FUNCTIONS *******************************/
VoteKeyElement copyVoteKeyElement(VoteKeyElement key) {
    return copyInt(key);
}

VoteDataElement copyVoteDataElement(VoteDataElement data) {
    return copyInt(data);
}

void freeVoteKeyElement(VoteKeyElement key) {
    //free
    freeInt(key);
}

void freeVoteDataElement(VoteDataElement data) {
    //free
    freeInt(data);
}

int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2) {
    return compareInts(key1, key2);
}

/********************** FRIENDLY STATE FUNCTIONS ***********************/
int stringCompare(void* str1, void* str2) {
    return strcmp(str1, str2);
}

int getFavoriteState(Map votes) {
    // return most voted state's id
    if(mapGetSize(votes) <= 0) return NO_FAVORITE_STATE;

    VoteKeyElement favState = mapGetFirst(votes);
    int max = *(int*)mapGet(votes, favState);
    MAP_FOREACH(VoteKeyElement, state, votes) {
        int num_of_votes = *(int*)mapGet(votes, state);
        if (num_of_votes > max) {
            favState = state;
            max = num_of_votes;
        }
    }

    return *(int*)favState; // id of most voted state
}

bool checkFriendlyStates(const int* stateId1,const int* favState1,const int* stateId2,const int* favState2) {
    if (!stateId1 || !favState1 || !stateId2 || !favState2) return false;

    return (*stateId1 == *favState2 && *stateId2 == *favState1);
}

Map getStateFavorites(Map states) {
    //create map state_favorites - key = stateId, value = favStateId
    Map state_favorites = mapCreate(copyInt, copyInt, freeInt, freeInt, compareInts);
    if (!state_favorites) return NULL;

    //iterate on the states map and fill on each state:
    MAP_FOREACH(int*, stateId, states) {
        StateData state = mapGet(states, stateId);
        if (!state) {
            mapDestroy(state_favorites);
            return NULL;
        }
        //outside function - getFavoriteStates(votes map) - second column
        int favState = getFavoriteState(state->votes);

        //insert stateId first column
        MapResult result = mapPut(state_favorites, stateId, &favState);

        if (result != MAP_SUCCESS) {
            mapDestroy(state_favorites);
            return NULL;
        }
    }

    return state_favorites;
}

char *getStatePair(StateData state1, StateData state2) {
    char *name1 = state1->name;
    char *name2 = state2->name;

    char *statePair = malloc(strlen(name1) + strlen(name2) + NUM_OF_EXTRA_CHARS + 1);
    if (!statePair) return NULL;
    statePair[0] = '\0';

    char *min = name2, *max = name1;
    if (strcmp(name1, name2) < 0) {
        min = name1;
        max = name2;
    }
    strcat(statePair, min);
    strcat(statePair, SPACE);
    strcat(statePair, DASH);
    strcat(statePair, SPACE);
    strcat(statePair, max);

    return statePair;
}
