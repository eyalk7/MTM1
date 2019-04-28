#ifndef STATES_H
#define STATES_H

#include "list.h"

/**
 *  File containing all macros, enums, structs and functions
 *  related to the States map and the data element in said map.
 */

/********************** MACROS & ENUMS ***********************/
/** macros for friendly states string */
#define EXTRA_CHARS " - "
#define NUM_OF_EXTRA_CHARS 3    // a space, a dash and a space = 3 chars

/** macro for states that dont have a favorite state (no votes) */
#define NO_FAVORITE_STATE (-1)

/********************** STATE MAP DEFINITIONS ***********************/
/*typedef struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = State's ID, data = no. of votes this state *gives*
} *StateData;*/

typedef struct StateData_t * StateData;

typedef void* StateKeyElement;
typedef void* StateDataElement;

typedef void* VoteKeyElement;
typedef void* VoteDataElement;

/********************** STATE MAP FUNCTIONS ***********************/
/** returns a copy of the state's id */
StateKeyElement copyStateKeyElement(StateKeyElement key);

/** returns a copy of the state's data (name, song name and votes) */
StateDataElement copyStateDataElement(StateDataElement data);

/** frees the state's id */
void freeStateKeyElement(StateKeyElement key);

/** frees the state's data (name, song name and votes) */
void freeStateDataElement(StateDataElement data);

/** compare between two state's ids */
int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2);

/************************* STATE DATA FUNCTIONS *******************************/
/** Create a StateData struct with the given state name and song name
 *  Returns NULL if a memory allocation failed */
StateData createStateData(const char *state_name, const char *song_name);

/***
 * Get the state's name
 * @param data - State data element (StateData struct)
 * @return The state's name
 */
char *getStateName(StateData data);

/***
 * Get the votes the state gives
 * @param data - Data Element in State's map (StateData struct)
 * @return
 */
Map getStateVotes(StateData data);

/********************** VOTE MAP FUNCTIONS ***********************/

/** returns a copy of the id of the state that receives the votes */
VoteKeyElement copyVoteKeyElement(VoteKeyElement key);

/** returns a copy of the amount of votes being given */
VoteDataElement copyVoteDataElement(VoteDataElement data);

/** frees the id of the state that receives the votes */
void freeVoteKeyElement(VoteKeyElement key);

/** frees the amount of votes being given */
void freeVoteDataElement(VoteDataElement data);

/** compares between the ids of two states that receive the votes */
int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2);

/********************** FRIENDLY STATE FUNCTIONS ***********************/
/** string compare function for string list lexicographical sort */
int stringCompare(void* str1, void* str2);

/** receives the votes a state gives
 *  returns the id of the state which got the most votes from that state */
int getFavoriteState(Map votes);

/***
 * Check if states are friendly by the assigment definition
 * @param stateId1 - first state's id
 * @param favState1 - first state's "favorite state"'s id
 * @param stateId2 - second state's id
 * @param favState2 - second state's "favorite state"'s id
 * @return
 *   false if one of the given pointers is NULL or states are not friendly
 *   true if states are friendly
 */
bool areFriendlyStates(const int *stateId1, const int *favState1,
                       const int *stateId2, const int *favState2);

/** returns a map that shows each state's "favorite state"
 * (key = state's id, value = favorite state's id) */
Map getStateFavorites(Map states);

/** returns a string of the state names in the format defined in the assigment:
 * "{first state's name} - {second state's name}"
 * (the state names are ordered lexicographically) */
char *getStatePair(StateData state1, StateData state2);

/** Returns a list of "friendly" states as defined in the assignment:
 *  A list of strings of state name pairs in which each state's most votes went to the other state
 *  in the pair. Each string is ordered lexicographically (not the list)
 * */
List getFriendlyStates(Map states);

#endif //STATES_H
