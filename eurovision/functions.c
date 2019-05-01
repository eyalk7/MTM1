#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "functions.h"

struct statePoints_t {
    int id;
    double points;
};

/*********************** STATIC FUNCTIONS *******************************/

/***
 * Check if a given character is a lower case alphabet letter
 * @param c charecter to check
 * @return true if lower case, false if not
 */
static bool isLowerCase(char c);

/***
 * Create StatePoints List. Initializes points to zero
 * @param states states/votes map to create the StatePoints list from
 * @return pointer to the new statePoints list
 */
static List pointListCreate(Map states);

/***
 * Converts given votes map to a list of StatePoints,
 * fill the statePoints list with the votes counts from the votes map
 *  Sorts the list from most voted state to least voted state
 * @param votes votes map to create the statePoints list from
 * @return pointer to the new sorted statePoints list
 */
static List convertVotesToList(Map votes);

/***
 * Converts given sorted list of statePoints to a sorted array
 * of 10 IDs (integers) of the first 10 states in the statePoints list
 * @param votes_list statePoints list
 * @return pointer to the new IDs array
 */
static int *getStateResults(List votes_list);

/***
 * Gets the amount points for a certain ranking based on the Ranking enum
 * @param place the place of the state in the ranking from first to tenth
 * @return the number of points to give to that state
 */
static Ranking getRanking(int place);

/***
 *  Receives an array of up to 10 state IDs
 *  and gives each state points according to their order
 *  using the Ranking enum
 * @param points_list pointer to a statePoints list to fill with the given points
 * @param results sorted array of up to 10 state IDs that need to get the points
 */
static void distributePoints(List points_list, const int *results);

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
static bool statesAreFriendly(const int *stateId1, const int *favState1,
                       const int *stateId2, const int *favState2);

/**
 * Returns a string of two states that are "friendly".
 * (the state names are ordered lexicographically)
 * @param state1 - The first state's data
 * @param state2 - The first state's data
 * @return Return a string of the two states' names
 *   in the format defined in the assigment:
 *   "{first state's name} - {second state's name}"
 */
static char *getStatePair(StateData state1, StateData state2);




/*********************** EUROVISION HELP FUNCTIONS *******************************/
/*** STATIC FUNCTIONS ***/
static bool isLowerCase(char c) {
    return ('a' <= c && c <= 'z');
}
/*** STATIC FUNCTIONS ***/

/*** NON-STATIC FUNCTIONS ***/
bool isValidName(const  char* name) {
    for (int i = 0; i < strlen(name); i++) {
        if (!isLowerCase(name[i]) && name[i] != ' ') return false;
    }
    return true;
}

void* copyInt(void* integer) {
    int* copy = malloc(sizeof(*copy));
    *copy = *(int*)integer;
    return copy;
}
void freeInt(void* integer) {
    free(integer);
}
int compareInts(MapKeyElement integer1, MapKeyElement integer2) {
    int a = *(int*)integer1;
    int b = *(int*)integer2;
    return a - b;
}

ListElement copyString(ListElement str) {
    char* copy = malloc(strlen(str) + 1);
    if (!copy) return NULL;

    strcpy(copy, str);

    return copy;
}
void freeString(ListElement str) {
    free(str);
}

EurovisionResult eurovisionChangeVote(Map states, int state_giver,
                                      int state_taker, int difference) {
    /// PARAMETER CHECKS ///
    if (states == NULL) return EUROVISION_NULL_ARGUMENT;
    if (state_giver < 0 || state_taker < 0) return EUROVISION_INVALID_ID;       // ID not valid
    if (!mapContains(states, &state_giver) || !mapContains(states, &state_taker)) {
        return EUROVISION_STATE_NOT_EXIST;          // one of the given states doesn't exist
    }
    if (state_giver == state_taker) return EUROVISION_SAME_STATE;       // same states given
    /// PARAMETER CHECKS ///

    // get current number of votes for state_taker in state_giver's votes map
    StateData giver_data = mapGet(states, &state_giver);
    assert(giver_data != NULL);
    int *current_votes_num = mapGet(stateGetVotes(giver_data), &state_taker);

    if (current_votes_num == NULL) { // no votes
        if (difference > 0) {
            // add state_taker to state's vote map along with the number of votes
            MapResult put_result = mapPut(stateGetVotes(giver_data), &state_taker, &difference);
            if (put_result == MAP_OUT_OF_MEMORY) return EUROVISION_OUT_OF_MEMORY;
        }
        // if difference <= 0 nothing is done (no votes added or removed)
    } else {
        // if there are votes for this state already:
        (*current_votes_num) += difference;             // update the number of votes
        if ((*current_votes_num) <= 0) {                // if, after the update, number of votes <= 0
            mapRemove(stateGetVotes(giver_data), &state_taker); // remove the votes from the votes map
        }
    }

    return EUROVISION_SUCCESS;
}
/*** NON-STATIC FUNCTIONS ***/

/***************************** POINTS LIST FUNCTIONS *****************************/
/*** STATIC FUNCTIONS ***/
static List pointListCreate(Map states) {
    assert(states != NULL);

    List list = listCreate(copyStatePoints, freeStatePoints);
    if (!list) return NULL;

    MAP_FOREACH(int*, id, states) {
        StatePoints data = malloc(sizeof(*data));
        if (!data) {
            listDestroy(list);
            return NULL;
        }

        data->id = *id;
        data->points = 0.0; // initialize to zero

        ListResult result = listInsertFirst(list, data);
        freeStatePoints(data);

        if (result != LIST_SUCCESS) {
            listDestroy(list);
            return NULL;
        }
    }

    return list;
}

static List convertVotesToList(Map votes) {
    assert(votes != NULL);

    // Convert to points list first (using votes instead of points)
    List list = pointListCreate(votes);     // IDs are set here
    LIST_FOREACH(StatePoints, point_data, list) {
        int* vote_count = mapGet(votes, &(point_data->id));
        if (!vote_count) {
            listDestroy(list);
            return NULL;
        }
        point_data->points = *vote_count;      // number of votes is set here
    }

    // Use list sort to sort based on vote counts
    ListResult result = listSort(list, compareStatePoints);
    if (result != LIST_SUCCESS) {
        listDestroy(list);
        return NULL;
    }

    return list;
}

static int *getStateResults(List votes_list) {
    assert(votes_list != NULL);

    int votes_size = listGetSize(votes_list);
    int len = (votes_size < NUMBER_OF_RANKINGS ? votes_size : NUMBER_OF_RANKINGS);

    int *state_results = malloc(sizeof(int) * NUMBER_OF_RANKINGS);
    if (!state_results) return NULL;        // allocation failed

    StatePoints ptr = listGetFirst(votes_list);
    for (int i = 0; i < len; i++) {
        state_results[i] = ptr->id;
        ptr = listGetNext(votes_list);
    }
    // if state voted for less than 10, fill the rest NO_STATE
    for (int i = len; i < NUMBER_OF_RANKINGS; i++) {
        state_results[i] = NO_STATE;
    }

    return state_results;
}
/*** STATIC FUNCTIONS ***/

/*** NON-STATIC FUNCTIONS ***/
ListElement copyStatePoints(ListElement element) {
    if (element == NULL) return NULL;

    StatePoints copy = malloc(sizeof(*copy));
    if (copy == NULL) return NULL;

    StatePoints data = element;
    copy->id = data->id;
    copy->points = data->points;

    return copy;
}
void freeStatePoints(ListElement element) {
    free(element);
}
int compareStatePoints(ListElement element1, ListElement element2) {
    // if state in element1 ranks higher than state in element2
    // returns negative
    // (therefore state1 < state2 in final list)
    // else return positive (NEVER returns 0)

    assert(element1 != NULL && element2 != NULL);

    StatePoints data1 = element1;
    StatePoints data2 = element2;

    if (data1->points == data2->points) {
        return data1->id - data2->id;       // IDs are always different
    }
    return (data2->points > data1->points) ? 1 : -1;
}

List convertToStringList(List final_results, Map states) {
    assert(final_results != NULL && states != NULL);

    // Create a list of names of the sates that are in given list
    List state_names = listCreate(copyString, freeString);
    LIST_FOREACH(StatePoints, point_data, final_results) {
        int stateId = point_data->id;
        StateData data = mapGet(states, &stateId);
        if (!data) {
            listDestroy(state_names);
            return NULL;
        }

        // Keep the same order they have in given list
        ListResult result = listInsertLast(state_names, stateGetName(data));
        if (result != LIST_SUCCESS) {
            listDestroy(state_names);
            return NULL;
        }
    }

    return state_names;
}
/*** NON-STATIC FUNCTIONS ***/

/***************************** CONTEST FUNCTIONS ********************************/
/*** STATIC FUNCTIONS ***/
static Ranking getRanking(int place) {
    static const Ranking ranking[NUMBER_OF_RANKINGS] = {
            FIRST_PLACE, SECOND_PLACE, THIRD_PLACE, FOURTH_PLACE,
            FIFTH_PLACE, SIXTH_PLACE, SEVENTH_PLACE, EIGHT_PLACE,
            NINTH_PLACE, TENTH_PLACE
    };

    return ranking[place];
}

static void distributePoints(List points_list, const int *results) {
    // for each state in results
    for (int i=0; i < NUMBER_OF_RANKINGS; i++) {
        int state_id = results[i];          // ID of the state to give points to
        int points = getRanking(i);         // get points to give

        // iterate on points list & find the state to give points to
        LIST_FOREACH(StatePoints, point_data, points_list){
            if (point_data->id == state_id) {
                point_data->points += points;      // update the state's points
            }
        }
    }
}
/*** STATIC FUNCTIONS ***/

/*** NON-STATIC FUNCTIONS ***/
List getAudiencePoints(Map states) {
    // create an audience points list with all states
    List audience_points = pointListCreate(states);
    if (!audience_points) return NULL;

    // distribute each states votes accordingly in audience_points
    MAP_FOREACH(int*, state_giver, states) {
        StateData giver_data = mapGet(states, state_giver);     // get the state's data
        assert(giver_data != NULL);

        // get the state's sorted vote list
        List votes_list = convertVotesToList(stateGetVotes(giver_data));
        if (!votes_list) {
            listDestroy(audience_points);
            return NULL;
        }

        int *state_results = getStateResults(votes_list);

        listDestroy(votes_list);        // deallocate votes_list

        if (!state_results) {
            listDestroy(audience_points);
            return NULL;
        }

        // distribute points to the states in state_results
        // (according to their order in the array)
        distributePoints(audience_points, state_results);

        free(state_results);   // deallocate the state_results array
    }

    return audience_points;
}

List getJudgesPoints(Map judges, Map states) {
    List judge_points = pointListCreate(states);
    if (!judges) return NULL;

    // for each judge
    MAP_FOREACH(int *, judge_id, judges) {
        // get the judge's data
        JudgeData judge_data = mapGet(judges, judge_id);
        assert(judge_data != NULL);

        // distribute points to the states in judge_results
        // (according to their order in the array)
        distributePoints(judge_points, judgeGetResults(judge_data));
    }

    return judge_points;
}

void calculateFinalPoints(List audience_points, List judge_points,
                          int num_of_states, int num_of_judges,
                          int audience_percent) {
    int judge_percent = 100 - audience_percent;

    // num_of_states is number of states that give points to a state
    // so it doesn't count the state itself
    num_of_states -= 1;

    // Make sure not to divide by 0 in case there is only 1 state
    if (num_of_states == 0) {
        num_of_states = 1;
    }

    // Iterate through the points each state got and calculate their final points
    // If there are no states the point lists will be empty so the loop won't iterate
    StatePoints judge_points_ptr = listGetFirst(judge_points);
    LIST_FOREACH(StatePoints, point_data, audience_points) {
        assert(judge_points_ptr != NULL);

        // Divide each state's audience points by the number of states
        point_data->points /= num_of_states;
        // Multiply each state's audience points by audience percentage
        point_data->points *= audience_percent;

        // if there are judges we have to take the judge points into account
        if (num_of_judges > 0) {
            // Divide each state's judge points by the number of judges
            judge_points_ptr->points /= num_of_judges;
            // Multiply each state's judge points by audience percentage
            judge_points_ptr->points *= judge_percent;

            // Add judge points to audience points
            point_data->points += judge_points_ptr->points;

            // increment judge_points pointer
            judge_points_ptr = listGetNext(judge_points);
        }
    }
}
/*** NON-STATIC FUNCTIONS ***/

/********************** FRIENDLY STATE FUNCTIONS ***********************/
/*** STATIC FUNCTIONS ***/
static bool statesAreFriendly(const int *stateId1, const int *favState1,
                              const int *stateId2, const int *favState2) {
    // if received NULL pointer return false
    if (!stateId1 || !favState1 || !stateId2 || !favState2) return false;

    // check if first state's most voted state is the second state and vice-versa
    return (*stateId1 == *favState2 && *stateId2 == *favState1);
}

static char *getStatePair(StateData state1, StateData state2) {
    // get the states' names
    char *name1 = stateGetName(state1);
    char *name2 = stateGetName(state2);

    // get the lengths of the states' names
    int len1 = strlen(name1);
    int len2 = strlen(name2);

    // allocate memory for the friendly states string
    char *statePair = malloc(len1 + len2 + NUM_OF_EXTRA_CHARS + 1);
    if (!statePair) return NULL;
    statePair[0] = '\0';    // initialize as empty string

    // order the states' names lexicographically
    char *min = name2, *max = name1;
    if (strcmp(name1, name2) < 0) {
        min = name1;
        max = name2;
    }

    // build the friendly states string
    // FORMAT = "{smaller state's name} - {bigger state's name}"
    strcat(statePair, min);
    strcat(statePair, EXTRA_CHARS);
    strcat(statePair, max);

    return statePair;
}
/*** STATIC FUNCTIONS ***/

/*** NON-STATIC FUNCTIONS ***/
int stringCompare(void* str1, void* str2) {
    return strcmp(str1, str2);  // lexicographical comparison
}

Map getStateFavorites(Map states) {
    // create a map that matches each state to its most voted state
    // (key = state ID, value = favorite state ID)
    Map state_favorites = mapCreate(copyInt, copyInt,
                                    freeInt, freeInt,
                                    compareInts);
    if (!state_favorites) return NULL;

    // initialize the favorite states map
    MAP_FOREACH(int*, stateId, states) {
        // for each state
        StateData state = mapGet(states, stateId);
        if (!state) {
            mapDestroy(state_favorites);
            return NULL;
        }

        // get the ID of the state's most voted state
        int favState = stateGetFavorite(state);

        // insert the state along with it's most voted state to the favorite states map
        MapResult result = mapPut(state_favorites, stateId, &favState);
        if (result != MAP_SUCCESS) {
            mapDestroy(state_favorites);
            return NULL;
        }
    }

    return state_favorites;
}

List getFriendlyStates(Map states) {
    // create empty string list
    List friendly_states = listCreate(copyString, freeString);
    if (!friendly_states) return NULL;  // allocation failed

    // get state favorites map - key = state's ID, value = favorite state's ID
    Map state_favorites = getStateFavorites(states);
    if (!state_favorites) {
        listDestroy(friendly_states);
        return NULL;                    // allocation failed
    }

    // for each state in state favorites
    MAP_FOREACH(int*, stateId, state_favorites) {
        // get its favorite state's ID
        int *favState1 = mapGet(state_favorites, stateId);
        // get the ID of the favorite state's favorite state
        int *stateId2 = favState1;
        int *favState2 = mapGet(state_favorites, stateId2);

        // check if the states are favorites of each other
        // (outside function also checks if pointer are NULL)
        if (statesAreFriendly(stateId, favState1, stateId2, favState2)) {
            // if states are a pair get their data
            StateData state1 = mapGet(states, stateId);
            StateData state2 = mapGet(states, stateId2);

            /// mark as if they have no favorite state **to prevent duplicates**
            *favState1 = NO_STATE;
            *favState2 = NO_STATE;

            // create the string that contains the state names (ordered lexicographically)
            char *statePair = getStatePair(state1, state2);
            if (!statePair) {
                mapDestroy(state_favorites);
                listDestroy(friendly_states);
                return NULL;            // allocation failed
            }

            // add the string to the list
            ListResult result = listInsertLast(friendly_states, statePair);

            free(statePair);        // deallocate the string with the state names

            if (result != LIST_SUCCESS) {
                mapDestroy(state_favorites);
                listDestroy(friendly_states);
                return NULL;            // list insert failed
            }
        }
    }

    mapDestroy(state_favorites);    // destroy state favorites map

    return friendly_states;
}
/*** NON-STATIC FUNCTIONS ***/
