#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "map.h"
#include "eurovision.h"
#include "state.h"
#include "judge.h"
#include "list.h"

/********************** MACROS & ENUMS ***********************/
/** macros for strings states list */
#define SPACE " "
#define DASH "-"
#define NUM_OF_EXTRA_CHARS 3

/** macro for audience precent */
#define ONE_HUNDREND_PRECENT 1

/** macros for points compare and enum for points distribution */
#define FIRST_BEFORE_SECOND (-1)
#define SECOND_BEFORE_FIRST 1

typedef enum {
    TENTH_PLACE = 1,
    NINTH_PLACE,
    EIGHT_PLACE,
    SEVENTH_PLACE,
    SIXTH_PLACE,
    FIFTH_PLACE,
    FOURTH_PLACE,
    THIRD_PLACE,
    SECOND_PLACE = 10,
    FIRST_PLACE = 12
} Ranking;

/**  enum for isIDValid function */
typedef enum mapType_t {
    STATES_MAP,
    JUDGES_MAP
} MapType;

/********************** EUROVISION HELP FUNCTIONS ***********************/
/** Check if given state/judge ID is valid */
EurovisionResult isIDValid(Map map, MapType type, int id);

/** change the vote count from state to state by a given amount */
EurovisionResult eurovisionChangeVote(Map states, int stateGiver, int stateTaker, int difference);

/** check if a given character is a lower case alphabet letter */
bool isLowerCase(char c);

/** check valid judge/song/state name */
bool checkValidName(const char* name);

/** Map ADT functions for Judge/State/state_favorites keysElements
 * and for votes/state_favorites dataElements */
void* copyInt(void* integer);
void freeInt(void* integer);
int compareInts(MapKeyElement integer1, MapKeyElement integer2);

/** List ADT functions for using a string list of state names */
ListElement copyString(ListElement str);
void freeString(ListElement str);

/********************** COUNT LIST FUNCTIONS & STRUCTS ***********************
* Count List is a List of CountData for saving the points/votes of each state/judge */

/** struct for summing up the amount of votes a state gives or receives */
typedef struct countData_t {
    int id;
    unsigned int voteCount;
} *CountData;

/** copy, free & compare functions for CountData element */
ListElement copyCountData(ListElement elem);
void freeCountData(ListElement elem);
int compareCountData(ListElement data1, ListElement data2);

/** create CountData List. Initializes vote counts to zero
 * Assumption: Given map has keys of type int* */
List countListCreate(Map map);

/** converts given votes map to CountData List */
List convertVotesToList(Map votes);

/** Converts final ranking of states (in CountData List)
 * to list of state names (strings List) */
List convertToStringlist(List finalResults, Map states);

/********************** CONTEST FUNCTIONS ***********************/
/** calculate the audience point (multiplied by audience percent)
 * of all states in given States map and returns CountData List */
List audiencePoints(Map states, int audiencePercent);

/********************** FRIENDLY STATE FUNCTIONS ***********************/
/** string compare function for the state names List sort */
int stringCompare(void* str1, void* str2);

/** Check if states are friendly by the assigment definition */
bool statesAreFriendly(const int *stateId1,const int *favState1,const int *stateId2,const int *favState2);

/** returns a map that shows each state's "favorite state"
 * (key = state's id, value = favorite state's id) */
Map getStateFavorites(Map states);

/** returns a string of the state names in the format defined in the assigment:
 * "{first state's name} - {second state's name}"
 * (the state names are ordered lexicographically) */
char *getStatePair(StateData state1, StateData state2);

#endif //FUNCTIONS_H
