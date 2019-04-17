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
#define SPACECHAR ' '
#define SPACE " "
#define DASH "-"
#define NUM_OF_EXTRA_CHARS 3

/** macro for audience precent */
#define ONE_HUNDREND_PRECENT 1

/** macros for points compare and enums for points distribution */
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

/**  enums for isIDValid function */
typedef enum mapType_t {
    STATES_MAP,
    JUDGES_MAP
} MapType;

/********************** EUROVISION HELP FUNCTIONS ***********************/
/** check if state/judge ID is valid */
EurovisionResult isIDValid(Map map, MapType type, int id);

/** change the vote count from state to state by */
EurovisionResult eurovisionChangeVote(Eurovision eurovision, int stateGiver, int stateTaker, int difference);

/** check if all letters lower case */
bool isLowerCase(char c);

/** check valid judge/song/state name */
bool checkValidName(const char* name);

/** check if the given judge's results contain the given state */
bool resultsContain (Map states, Map judges, int judge_id, int state_id);

/** Map ADT functions for Judge/State/state_favorites keysElements
 * and for votes/state_favorites dataElements */
void* copyInt(void* integer);
void freeInt(void* integer);
int compareInts(void* integer1, void* integer2);

/** List ADT functions for strings states list */
ListElement copyString(ListElement str);
void freeString(ListElement str);

/********************** COUNT LIST FUNCTIONS & STRUCTS ***********************
* Count List is a List of CountData for saving stateID & points/votes pairs */

/** struct for votes/points Count Lists */
typedef struct countData_t {
    int id;
    unsigned int count;
} *CountData;

/** copy, free & compare functions for CountData element */
ListElement copyCountData(ListElement elem);
void freeCountData(ListElement elem);
int compareCountData(ListElement data1, ListElement data2);

/** create CountData List
 * Assumption: Given map has keys of type int* */
List countListCreate(Map map);

/** converts given votes map to CountData List */
List convertVotesToList(Map votes);

/** Converts final ranking of states (in CountData List)
 * to list of state names (strings List) */
List convertToStringlist(List finalResults, Map states);

/********************** CONTEST FUNCTIONS ***********************/
/** calculate the audience point (multiplied by audience precent)
 * of all states in given States map and returns CountData List */
List audiencePoints(Map states, int audiencePrecent);

/********************** FRIENDLY STATE FUNCTIONS ***********************/
int stringCompare(void* str1, void* str2);
bool statesAreFriendly(int *stateId1, int *favState1, int *stateId2, int *favState2);

#endif //FUNCTIONS_H
