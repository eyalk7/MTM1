#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "functions.h"



/*********************** EUROVISION HELP FUNCTIONS *******************************/
EurovisionResult isIDValid(Map map, MapType type, int id) {
    // assert valid arguments (checked already in the sending function)
    assert(map != NULL);
    assert(type == STATES_MAP || type == JUDGES_MAP);

    if (id < 0) return EUROVISION_INVALID_ID;   //check ID >= 0

    // check if Id already exist in the given map
    if (mapContains(map, &id)) {
        if (type == STATES_MAP) return EUROVISION_STATE_ALREADY_EXIST;
        return EUROVISION_JUDGE_ALREADY_EXIST;  // else type == JUDGES_MAP
    }

    if (type == STATES_MAP) return EUROVISION_STATE_NOT_EXIST;
    return EUROVISION_JUDGE_NOT_EXIST;  // else type == JUDGES_MAP
}

bool isLowerCase(char c) {
    return ('a' <= c && c <= 'z');
}

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
    // check valid arguments
    if (states == NULL) return EUROVISION_NULL_ARGUMENT;

    EurovisionResult result = isIDValid(states, STATES_MAP, state_giver);
    if (result != EUROVISION_STATE_ALREADY_EXIST) return result;

    result = isIDValid(states, STATES_MAP, state_taker);
    if (result != EUROVISION_STATE_ALREADY_EXIST) return result;

    // check that it's the same state
    if (state_giver == state_taker) return EUROVISION_SAME_STATE;

    // get current number of votes for state_taker in state_giver's votes map
    StateData giver_data = mapGet(states, &state_giver);
    assert(giver_data != NULL);
    int *current_votes_num = mapGet(giver_data->votes, &state_taker);

    if (current_votes_num == NULL) { // no votes
        if (difference > 0) {
            // add state_taker to state's vote map along with the number of votes
            MapResult put_result = mapPut(giver_data->votes, &state_taker, &difference);
            if (put_result == MAP_OUT_OF_MEMORY) return EUROVISION_OUT_OF_MEMORY;
        }
        // if difference <= 0 nothing is done (no votes added or removed)
    } else {
        assert(current_votes_num != NULL);              // there are votes for this state already

        (*current_votes_num) += difference;             // update the number of votes
        if ((*current_votes_num) <= 0) {                // if, after the update, number of votes <= 0
            mapRemove(giver_data->votes, &state_taker); // remove the votes from the votes map
        }
    }

    return EUROVISION_SUCCESS;
}

bool judgeResultsContain(JudgeData judge, int state_id) {
    for (int i=0; i < NUMBER_OF_RANKINGS; i++) {
        if (judge->results[i] == state_id) return true;
    }
    return false;
}

/***************************** COUNT LIST FUNCTIONS *****************************/
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
    return (int)(data2->points - data1->points);
}

List pointListCreate(Map states) {
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

int *convertVotesToIDArray(Map votes, int *array_size) {
    assert(votes != NULL);

    *array_size = 0;    // return 0 if error occurs

    // Convert to points list first (using votes instead of points)
    List list = pointListCreate(votes);     // IDs are set here
    LIST_FOREACH(StatePoints, element, list) {
        int* vote_count = mapGet(votes, &(element->id));
        if (!vote_count) {
            listDestroy(list);
            return NULL;
        }
        element->points = *vote_count;      // number of votes is set here
    }

    // Use list sort to sort based on vote counts
    ListResult result = listSort(list, compareStatePoints);
    if (result != LIST_SUCCESS) {
        listDestroy(list);
        return NULL;
    }

    int num_of_votes = mapGetSize(votes);
    int len = (num_of_votes < NUMBER_OF_RANKINGS ? num_of_votes : NUMBER_OF_RANKINGS);

    int *state_results = malloc(sizeof(int) * len);
    if (!state_results) {
        listDestroy(list);
        return NULL;        // allocation failed
    }

    StatePoints ptr = listGetFirst(list);
    for (int i = 0; i < len; i++) {
        state_results[i] = ptr->id;
        ptr = listGetNext(list);
    }

    *array_size = len;      // return array size
    return state_results;
}

List convertToStringList(List final_results, Map states) {
    assert(final_results != NULL && states != NULL);

    // Create a list of names of the sates that are in given list
    List state_names = listCreate(copyString, freeString);
    LIST_FOREACH(StatePoints, element, final_results) {
        int stateId = element->id;
        StateData data = mapGet(states, &stateId);
        if (!data) {
            listDestroy(state_names);
            return NULL;
        }

        // Keep the same order they have in given list
        ListResult result = listInsertLast(state_names, data->name);
        if (result != LIST_SUCCESS) {
            listDestroy(state_names);
            return NULL;
        }
    }

    return state_names;
}

/***************************** CONTEST FUNCTIONS ********************************/
Ranking getRanking(int place) {
    static const Ranking ranking[NUMBER_OF_RANKINGS] = {
            FIRST_PLACE, SECOND_PLACE, THIRD_PLACE, FOURTH_PLACE,
            FIFTH_PLACE, SIXTH_PLACE, SEVENTH_PLACE, EIGHT_PLACE,
            NINTH_PLACE, TENTH_PLACE
    };

    return ranking[place];
}

List getAudiencePoints(Map states) {
    // create an audience points list with all states
    List audience_points = pointListCreate(states);
    if (!audience_points) return NULL;

    // distribute each states votes accordingly in audience_points
    MAP_FOREACH(int*, state_giver, states) {
        StateData giver_data = mapGet(states, state_giver);     // get the state's data
        assert(giver_data != NULL);

        // get the state's sorted vote list
        int results_size = 0;
        int *state_results = convertVotesToIDArray(giver_data->votes, &results_size);
        if (!state_results) {
            listDestroy(audience_points);
            return NULL;
        }

        // distribute points to the states in state_results
        // (according to their order in the array)
        distributePoints(audience_points, state_results, results_size);

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
        distributePoints(judge_points, judge_data->results, NUMBER_OF_RANKINGS);
    }

    return judge_points;
}

void distributePoints(List points_list, const int *results, int results_size) {
    // for each state in results
    for (int i=0; i < results_size; i++) {
        int state_id = results[i];          // ID of the state to give points to
        int points = getRanking(i);         // get points to give

        // iterate on points list & find the state to give points to
        LIST_FOREACH(StatePoints, state_points, points_list){
            if (state_points->id == state_id) {
                state_points->points += points;      // update the state's points
            }
        }
    }
}
