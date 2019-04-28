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

/********************** EUROVISION HELP FUNCTIONS ***********************/
/***
 * Check if a given character is a lower case alphabet letter
 * @param c charecter to check
 * @return true if lower case, false if not
 */
bool isLowerCase(char c);

/***
 * Check that the given string contains only small letters and spaces
 * @param name string to check
 * @return true if the string contains only small letters and spaces, false else
 */
bool isValidName(const char* name);

/***
 * copy function for integer elements that used in:
 * Judge/State/state_favorites Maps keysElements
 * and in votes/state_favorites Maps dataElements
 * @param integer pointer for int element to copy
 * @return a pointer for the new int element
 */
void* copyInt(void* integer);

/***
 * free function for integer elements that used in:
 * Judge/State/state_favorites Maps keysElements
 * and in votes/state_favorites Maps dataElements
 * @param integer pointer for int element to free
 */
void freeInt(void* integer);

/***
 * compare function for integer elements that used in:
 * Judge/State/state_favorites Maps keysElements
 * and in votes/state_favorites Maps dataElements
 * @param integer1 first integer to compare
 * @param integer2 second integer to compare
 * @return positive number if the first integer is bigger,
 * negative number if the second number is bigger,
 * 0 if the numbers are equal
 */
int compareInts(MapKeyElement integer1, MapKeyElement integer2);

/** List ADT functions for using a string list of state names */
/***
 * copy function for a strings list to use for states names list
 * @param str
 * @return
 */
ListElement copyString(ListElement str);
void freeString(ListElement str);

/** Change stateGiver's vote point to stateTaker by a given difference */
EurovisionResult eurovisionChangeVote(Map states, int state_giver,
                                      int state_taker, int difference);

/********************** POINT LIST FUNCTIONS & STRUCTS ***********************
* point List is a List of StatePoints for saving the points/votes of each state */

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

/** Converts given votes map to a list of StatePoints struct
 *  Sorts the list from most voted state to least voted state */
List convertVotesToList(Map votes);

/** Converts given list of votes to an array of IDs
 *  The length of the returned array is not higher than the number of rankings (10) */
int *getStateResults(List votes_list, int *array_size);

/** Converts final ranking of states (in StatePoints List)
 * to list of state names (strings List) */
List convertToStringList(List final_results, Map states);

/********************** CONTEST FUNCTIONS ***********************/
/** Gets the amount points for a certain ranking based on the Ranking enum */
Ranking getRanking(int place);

/***
 * Returns a list of each state's points given by the audience
 * @param states
 * @return
 */
List getAudiencePoints(Map states);

/** Returns a list of each state's points given by the judges */
List getJudgesPoints(Map judges, Map states);

/** Receives an array of up to 10 state IDs (array size given too)
 *  and gives each state points according to their order
 *  using the Ranking enum
 *  */
void distributePoints(List points_list, const int *results, int results_size);

/** Divides each state's audience points by the number of states
 *  and multiplies it by the audience percentage.
 *  Does the same things for each state's judge points.
 *  Finally, it adds to each state's audience points its corresponding judge points. */
void calculateFinalPoints(List audience_points, List judge_points,
                          int num_of_states, int num_of_judges,
                          int audience_percent);

#endif //FUNCTIONS_H
