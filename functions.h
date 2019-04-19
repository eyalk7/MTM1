#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "map.h"
#include "eurovision.h"
#include "state.h"
#include "judge.h"
#include "list.h"

/********************** MACROS & ENUMS ***********************/
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
EurovisionResult checkIDValid(Map map, MapType type, int id);

/** check if a given character is a lower case alphabet letter */
bool checkLowerCase(char c);

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

/** change the vote count from state to state by a given amount */
EurovisionResult eurovisionChangeVote(Map states, int stateGiver, int stateTaker, int difference);

/** checks if a judge gave points to the given state */
bool judgeResultsContain(JudgeData judge, int stateId);

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
List convertToStringList(List finalResults, Map states);

/********************** CONTEST FUNCTIONS ***********************/
/** calculate the audience point (multiplied by audience percent)
 * of all states in given States map and returns CountData List */
List getAudiencePoints(Map states, int audiencePercent);

#endif //FUNCTIONS_H
