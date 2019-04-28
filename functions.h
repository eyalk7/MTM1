#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "map.h"
#include "eurovision.h"
#include "state.h"
#include "judge.h"

/*
 * These are included in judge.h:
 * #include <stdbool.h>
 * #include "list.h"
 */

/**
 * file containing helper Macros, Enumes, Structs & Functions for eurovision
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
 * @return
 *      positive number if the first integer is bigger,
 *      negative number if the second number is bigger,
 *      0 if the numbers are equal
 */
int compareInts(MapKeyElement integer1, MapKeyElement integer2);

/***
 * copy function for a strings list to use for states names list
 * @param str string to copy
 * @return pointer to the new string
 */
ListElement copyString(ListElement str);

/***
 * free function for a strings list to use for states names list
 * @param str string to free
 */
void freeString(ListElement str);

/***
 * Change the count of votes from stateGiver to stateTaker by a given difference
 * @param states states map that contains stateGiver & stateTaker
 * @param state_giver the state that gives the votes
 * @param state_taker the state that gets the votes
 * @param difference number of votes to give to stateTaker
 * @return
 *      EUROVISION_NULL_ARGUMENT if states is NULL
 *      EUROVISION_INVALID_ID if state_giver or state_taker less than 0
 *      EUROVISION_STATE_NOT_EXIST if one of the states not in states map
 *      EUROVISION_SAME_STATE if state_giver & state_taker is the same state
 *      EUROVISION_OUT_OF_MEMORY if memory allocation failed
 *      EUROVISION_SUCCESS if the votes was added
 */
EurovisionResult eurovisionChangeVote(Map states, int state_giver,
                                      int state_taker, int difference);

/********************** POINT LIST FUNCTIONS & STRUCTS ***********************
* point List is a List of StatePoints for saving the points/votes of each state */

/** struct for summing up the amount of points a state receives or gives */
typedef struct statePoints_t *StatePoints;

/***
 * copy function for StatePoints element
 * @param element pointer statePoints element to copy
 * @return pointer to the new statePoints element
 */
ListElement copyStatePoints(ListElement element);

/***
 * free function for StatePoints element
 * @param element statePoints element to free
 */
void freeStatePoints(ListElement element);

/***
 * compare function for StatePoints element
 * Note: compare function only returns positive or negative
 * (two StatePoints structs can't be equal because IDs are unique)
 * @param element1 pointer to the first statePoints element
 * @param element2 pointer to the second statePoints element
 * @return
 *   negative number if state in element1 ranks higher than state in element2
 *   positive number if state in element2 ranks higher than state in element1
 */
int compareStatePoints(ListElement element1, ListElement element2);

/***
 * Create StatePoints List. Initializes points to zero
 * @param states states/votes map to create the StatePoints list from
 * @return pointer to the new statePoints list
 */
List pointListCreate(Map states);

/***
 * Converts given votes map to a list of StatePoints,
 * fill the statePoints list with the votes counts from the votes map
 *  Sorts the list from most voted state to least voted state
 * @param votes votes map to create the statePoints list from
 * @return pointer to the new sorted statePoints list
 */
List convertVotesToList(Map votes);

/***
 * Converts given sorted list of statePoints to a sorted array
 * of 10 IDs (integers) of the first 10 states in the statePoints list
 * @param votes_list statePoints list
 * @return pointer to the new IDs array
 */
int *getStateResults(List votes_list, int *array_size);

/***
 * Converts final ranking of states (in StatePoints List)
 * to list of states names (strings List)
 * @param final_results the final sorted statePoints list
 * @param states states map that contains the states in the statePoints list
 * @return pointer to the new strings list of states names
 */
List convertToStringList(List final_results, Map states);

/********************** CONTEST FUNCTIONS ***********************/
/***
 * Gets the amount points for a certain ranking based on the Ranking enum
 * @param place the place of the state in the ranking from first to tenth
 * @return the number of points to give to that state
 */
Ranking getRanking(int place);

/***
 * Returns a statePoints list of each state's points given by the audience
 * @param states states map that contains the needed states
 * @return pointer to the new statePoints list
 */
List getAudiencePoints(Map states);

/***
 * Returns a statePoints list of each state's points given by the judges
 * @param judges judges map that contains the needed states
 * @param states states map that contains the needed states
 * @return pointer to the new statePoints list
 */
List getJudgesPoints(Map judges, Map states);

/***
 *  Receives an array of up to 10 state IDs
 *  and gives each state points according to their order
 *  using the Ranking enum
 * @param points_list pointer to a statePoints list to fill with the given points
 * @param results sorted array of up to 10 state IDs that need to get the points
 */
void distributePoints(List points_list, const int *results, int results_size);

/***
 *  Divides each state's audience points by the number of states minus one
 *  and multiplies it by the audience percentage.
 *  Does the same things for each state's judge points (not minus one).
 *  Finally, it adds to each state's audience points its corresponding judge points.
 * @param audience_points pointer to a statePoints list containing the audience points to the states
 * @param judge_points pointer to a statePoints list containing the judges points to the states
 * @param num_of_states number of states in the eurovision
 * @param num_of_judges number of judges to the eurovision
 * @param audience_percent wanted percentage of the audience points in the final calculation
 */
void calculateFinalPoints(List audience_points, List judge_points,
                          int num_of_states, int num_of_judges,
                          int audience_percent);

#endif //FUNCTIONS_H
