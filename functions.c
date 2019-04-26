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
    //check the given string only contains small letters and spaces
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
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        if (judge->results[i] == state_id) return true;
    }
    return false;
}

/***************************** COUNT LIST FUNCTIONS *****************************/
ListElement copyCountData(ListElement element) {
    if (element == NULL) return NULL;

    CountData copy = malloc(sizeof(*copy));
    if (copy == NULL) return NULL;

    CountData data = element;
    copy->id = data->id;
    copy->vote_count = data->vote_count;

    return copy;
}

void freeCountData(ListElement element) {
    free(element);
}

int compareCountData(ListElement element1, ListElement element2) {
    // if state in element 1 ranks higher than state in element2
    // returns negative
    // else return positive (NEVER returns 0)

    assert(element1 != NULL && element2 != NULL);

    CountData data1 = element1;
    CountData data2 = element2;

    if (data1->vote_count == data2->vote_count) {
        return data1->id - data2->id;       // IDs are always different
    }
    return data2->vote_count - data1->vote_count;
}

List countListCreate(Map map) {
    assert(map != NULL);

    List list = listCreate(copyCountData, freeCountData);
    if (!list) return NULL;

    MAP_FOREACH(int*, key, map) {
        CountData data = malloc(sizeof(*data));
        if (!data) {
            listDestroy(list);
            return NULL;
        }

        data->id = *key;
        data->vote_count = 0; // initialize to 0

        ListResult result = listInsertFirst(list, data);
        freeCountData(data);

        if (result != LIST_SUCCESS) {
            listDestroy(list);
            return NULL;
        }
    }

    return list;
}

List convertVotesToList(Map votes) {
    assert(votes != NULL);

    // create the list
    List list = countListCreate(votes);     // IDs are set here
    LIST_FOREACH(CountData, element, list) {
        int* vote_count = mapGet(votes, &(element->id));
        if (!vote_count) {
            listDestroy(list);
            return NULL;
        }
        element->vote_count = *vote_count;  // number of votes is set here
    }

    // sort the list based on vote counts
    ListResult result = listSort(list, compareCountData);
    if (result != LIST_SUCCESS) {
        listDestroy(list);
        return NULL;
    }

    return list;
}

List convertToStringList(List finalResults, Map states) {
    assert(finalResults != NULL && states != NULL);

    // Create a list of names of the sates that are in given list
    List state_names = listCreate(copyString, freeString);
    LIST_FOREACH(CountData, element, finalResults) {
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
    static const Ranking ranking[NUMBER_OF_STATES_TO_RANK] = {
            FIRST_PLACE, SECOND_PLACE, THIRD_PLACE, FOURTH_PLACE,
            FIFTH_PLACE, SIXTH_PLACE, SEVENTH_PLACE, EIGHT_PLACE,
            NINTH_PLACE, TENTH_PLACE
    };

    return ranking[place];
}

List getAudiencePoints(Map states, int audience_percent) {
    // create an audience points list with all states
    List audience_points = countListCreate(states);
    if (!audience_points) return NULL;

    // distribute each states votes accordingly in audience_points
    MAP_FOREACH(int*, state_giver, states) {
        StateData giver_data = mapGet(states, state_giver);     // get the state's data
        assert(giver_data != NULL);

        // get the state's sorted vote list
        List state_votes = convertVotesToList(giver_data->votes);
        if (!state_votes) {
            listDestroy(audience_points);
            return NULL;
        }

        distributeStateVotes(audience_points, state_votes, audience_percent);

        listDestroy(state_votes);   // deallocate the state_votes list
    }

    return audience_points;
}

void distributeStateVotes(List audience_points, List state_votes,
                          int audience_percent) {
    // iterate on the giver state's votes and update the audience points list accordingly
    CountData vote_data = listGetFirst(state_votes);
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK && vote_data != NULL; i++) {
        int state_taker = vote_data->id;                // the state being given the points
        int points = audience_percent * getRanking(i);      // amount of points to give

        addStatePoints(audience_points, state_taker, points);

        vote_data = listGetNext(state_votes);           // go to next "given votes" node
    }
}

void addStatePoints(List audience_points, int state_taker, int points) {
    // iterate on audience points list & find the state to give points to
    LIST_FOREACH(CountData, data, audience_points){
        if (data->id == state_taker) {
            data->vote_count += points;      // update the state_taker's points
        }
    }
}
