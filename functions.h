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

/** Check if a given character is a lower case alphabet letter */
bool isLowerCase(char c);

/** Check the given string only contains small letters and spaces */
bool isValidName(const char* name);

/** Map ADT functions for Judge/State/state_favorites keysElements
 * and for votes/state_favorites dataElements */
void* copyInt(void* integer);
void freeInt(void* integer);
int compareInts(MapKeyElement integer1, MapKeyElement integer2);

/** List ADT functions for using a string list of state names */
ListElement copyString(ListElement str);
void freeString(ListElement str);

/** Change stateGiver's vote point to stateTaker by a given difference */
EurovisionResult eurovisionChangeVote(Map states, int stateGiver,
                                      int stateTaker, int difference);

/** checks if a judge gave points to the given state */
bool judgeResultsContain(JudgeData judge, int stateId);

/********************** point LIST FUNCTIONS & STRUCTS ***********************
* point List is a List of StatePoints for saving the points/votes of each state/judge */

/** struct for summing up the amount of points a state receives */
typedef struct statePoints_t {
    int id;
    double points;
} *StatePoints;

/** copy, free & compare functions for StatePoints element
 * Note: compare function only returns positive or negative
 * (two StatePoints structs can't be equal because IDs are unique)
 * */
ListElement copyStatePoints(ListElement element);
void freeStatePoints(ListElement element);
int compareStatePoints(ListElement element1, ListElement element2);

/** Create StatePoints List. Initializes points to zero
 * Assumption: Given map has keys of type int* (states' IDs) */
List pointListCreate(Map states);

/** Converts given votes map to an array of IDs
 *  of the top 10 most voted states */
int *convertVotesToIDArray(Map votes, int *array_size);

/** Converts final ranking of states (in StatePoints List)
 * to list of state names (strings List) */
List convertToStringList(List final_results, Map states);

/********************** CONTEST FUNCTIONS ***********************/
/** Gets the amount points for a certain ranking based on the Ranking enum */
Ranking getRanking(int place);

/** Returns a list of each state's points given by the audience */
List getAudiencePoints(Map states);

/** Returns a list of each state's points given by the judges */
List getJudgesPoints(Map judges, Map states);

/** Receives an array of up to 10 state IDs (array size given too)
 *  and gives each state points according to their order
 *  using the Ranking enum
 *  */
void distributePoints(List points_list, const int *results, int results_size);

#endif //FUNCTIONS_H
