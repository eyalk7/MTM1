#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "map.h"
#include "eurovision.h"
#include "state.h"
#include "judge.h"
#include "list.h"

/*
 * These are included in judge.h:
 * #include <stdbool.h>
 */

/********************** MACROS & ENUMS ***********************/
/** macro for audience precent */
#define ONE_HUNDREND_PRECENT 1

/**  enum for number of points in each rank */
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

/** check if a given character is a lower case alphabet letter */
bool isLowerCase(char c);

/** check valid judge/song/state name */
bool isValidName(const char* name);

/** Map ADT functions for Judge/State/state_favorites keysElements
 * and for votes/state_favorites dataElements */
void* copyInt(void* integer);
void freeInt(void* integer);
int compareInts(MapKeyElement integer1, MapKeyElement integer2);

/** List ADT functions for using a string list of state names */
ListElement copyString(ListElement str);
void freeString(ListElement str);

/** change the vote count from state to state by a given amount */
EurovisionResult eurovisionChangeVote(Map states, int stateGiver,
                                      int stateTaker, int difference);

/** checks if a judge gave points to the given state */
bool judgeResultsContain(JudgeData judge, int stateId);

/********************** COUNT LIST FUNCTIONS & STRUCTS ***********************
* Count List is a List of CountData for saving the points/votes of each state/judge */

/** struct for summing up the amount of votes a state gives or receives */
typedef struct countData_t {
    int id;
    unsigned int vote_count;
} *CountData;

/** copy, free & compare functions for CountData element
 * Note: compare function only returns positive or negative
 * (two CountData structs can't be equal because IDs are unique)
 * */
ListElement copyCountData(ListElement element);
void freeCountData(ListElement element);
int compareCountData(ListElement element1, ListElement element2);

/** create CountData List. Initializes vote counts to zero
 * Assumption: Given map has keys of type int* */
List countListCreate(Map map);

/** converts given votes map to CountData List */
List convertVotesToList(Map votes);

/** Converts final ranking of states (in CountData List)
 * to list of state names (strings List) */
List convertToStringList(List finalResults, Map states);

/********************** CONTEST FUNCTIONS ***********************/
/** Gets the amount points for a certain ranking based on the Ranking enum */
Ranking getRanking(int place);

/** Calculates the audience points of all states in given States map
 *  Returns a list of each state's points given by the audience */
List getAudiencePoints(Map states, int audience_percent);

/** Receives a state's given votes in order from highest to lowest
 *  and calculates the points to give to each state based on said order.
 *  The function also takes into account the audience percentage
 *  */
void distributeStateVotes(List audience_points, List state_votes,
                          int audience_percent);

/** Adds given amount of points to the state with the given ID in the given list */
void addStatePoints(List audience_points, int state_taker, int points);

#endif //FUNCTIONS_H
