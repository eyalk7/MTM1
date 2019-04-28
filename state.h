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
#define NO_STATE (-1)

/********************** STATE MAP DEFINITIONS ***********************/
typedef struct StateData_t * StateData;

typedef void* StateKeyElement;
typedef void* StateDataElement;

typedef void* VoteKeyElement;
typedef void* VoteDataElement;

/********************** STATE MAP FUNCTIONS ***********************/
/***
 * Copy function for the key in States map.
 * @param key - A state's ID
 * @return Returns a copy of the state ID
 */
StateKeyElement copyStateKeyElement(StateKeyElement key);

/***
 * Copy function for the data element in States map.
 * @param data - StateData struct with state's name, song name and votes map
 * @return A copy of the StateData struct
 */
StateDataElement copyStateDataElement(StateDataElement data);

/***
 * Function for deallocating a key element in State map.
 * @param key - The state's ID
 */
void freeStateKeyElement(StateKeyElement key);

/***
 * Function for deallocating a data element in States map.
 * @param data - StateData struct with state's name, song name and votes
 */
void freeStateDataElement(StateDataElement data);

/***
 * Compare function for two keys in States map.
 * @param key1 - A state's ID
 * @param key2 - A state's ID
 * @return
 *   Positive integer if first key is bigger than the second
 *   Negative integer if first key is smaller than the second
 *   0 if keys are equal
 */
int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2);

/************************* STATE DATA FUNCTIONS *******************************/
/**
 * Create a data element for the States map.
 * @param state_name - The state name for the StateData struct.
 * @param song_name  - The state song name for the StateData struct.
 * @return
 *   NULL if a memory allocation failed
 *   A StateData struct with the given state name and song name
 */
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
/**
 * Copy function for the key element in the Votes map.
 * @param key - ID of the state that receives the votes
 * @return A copy of the state ID
 */
VoteKeyElement copyVoteKeyElement(VoteKeyElement key);

/**
 * Copy function for the data element in the Votes map.
 * @param data - The amount of votes to give
 * @return A copy of the amount of votes
 */
VoteDataElement copyVoteDataElement(VoteDataElement data);

/**
 * Function for deallocating the key element in the Votes map.
 * @param key - ID of the state that receives the votes
 */
void freeVoteKeyElement(VoteKeyElement key);

/**
 * Function for deallocating the data element in the Votes map.
 * @param data - The amount of votes to give
 */
void freeVoteDataElement(VoteDataElement data);


/***
 * Compare function for two keys in Votes map.
 * @param key1 - A state's ID
 * @param key2 - A state's ID
 * @return
 *   Positive integer if first key is bigger than the second
 *   Negative integer if first key is smaller than the second
 *   0 if keys are equal
 */
int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2);

/********************** FRIENDLY STATE FUNCTIONS ***********************/
/**
 * String compare function for string list lexicographical sort
 * @param str1
 * @param str2
 * @return
 *   Positive integer if first string is after the second in the dictionary
 *   Negative integer if first string is before the second in the dictionary
 *   0 if strings are equal
 */
int stringCompare(void* str1, void* str2);

/**
 * A given state's favorite state
 * @param state - A state's data (name, song name and votes)
 * @return
 *   Returns the ID of the state which got the most votes
 *   in the given StateData's votes map
 */
int getFavoriteState(StateData state);

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

/**
 * Get a map that shows each state's "favorite state"
 * (key = state's id, value = favorite state's id)
 * @param states - A map of states
 * @return Returns a map that matches each state's ID
 *   to the ID of the state that they gave most votes to
 */
Map getStateFavorites(Map states);

/**
 * Returns a string of two states that are "friendly".
 * (the state names are ordered lexicographically)
 * @param state1 - The first state's data
 * @param state2 - The first state's data
 * @return Return a string of the two states' names
 *   in the format defined in the assigment:
 *   "{first state's name} - {second state's name}"
 */
char *getStatePair(StateData state1, StateData state2);

 /**
  * Get a string list of states that are "friendly".
  * Each string is ordered lexicographically (not the list)
  * @param states - Map of states
  * @return
  *   Returns a list of "friendly" states as defined in the assignment:
  *   A list of strings of state name pairs in which each state's most votes went to the other state
  *   in the pair.
  */
List getFriendlyStates(Map states);

#endif //STATES_H
