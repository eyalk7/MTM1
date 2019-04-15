#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "functions.h"

EurovisionResult isIDValid(Map map, MapType type, int id) {
    // assert valid arguments (checked already in the sending function)
    assert(map != NULL);
    assert(type == STATES_MAP || type == JUDGES_MAP);
    //check ID >= 0 & check if Id already exist in the given map
    if (id < 0) {
        return EUROVISION_INVALID_ID;
    }

    if (mapContains(map, &id)) {
        if (type == STATES_MAP) return EUROVISION_STATE_ALREADY_EXIST;
        return EUROVISION_JUDGE_ALREADY_EXIST;  // else type == JUDGES_MAP
    }

    // else state not exist
    if (type == STATES_MAP) return EUROVISION_STATE_NOT_EXIST;
    return EUROVISION_JUDGE_NOT_EXIST;  // else type == JUDGES_MAP
}

bool isLowerCase(char c) {
    return ('a' <= c && c <= 'z');
}

bool checkValidName(const  char* name) {
    //check the given string only contains small letters and spaces
    for (int i = 0; i < strlen(name); i++) {
        if (!isLowerCase(name[i]) && name[i] != SPACECHAR) return false;
    }
    return true;
}

int* copyInteger(int num) {
    //allocation for int
    int* copy = malloc(sizeof(*copy));
    //initialize the int
    *copy = num;
    //return address
    return copy;
}

int compareIntegers(int a, int b) {
    //compare the ints
    //return 0 if equal
    //return bigger than 0 if first is bigger
    //else the small than 0
    return a - b;
}

bool resultsContain (Map states, Map judges, int judge_id, int state_id) {
    // assert valid arguments (checked already in the sending function)
    assert(states != NULL && judges != NULL && isIDValid(states, STATES_MAP, state_id) == EUROVISION_STATE_ALREADY_EXIST);

    // get the judge's data & assert judge_id exist in the map
    JudgeDataElement tmp_judge = mapGet(judges, &judge_id);
    assert(tmp_judge != NULL);

    // iterate on the judge's results and check if state_id is there
    return resultsContainState(tmp_judge, state_id);
}

List audiencePoints(Map states, int audiencePrecent) {
    // create an audience points list with all states & Ranking table
    List audience_points = countListCreate(states);
    Ranking ranking[NUMBER_OF_STATES_TO_RANK] = {FIRST_PLACE, SECOND_PLACE, THIRD_PLACE, FOURTH_PLACE, FIFTH_PLACE, SIXTH_PLACE, SEVENTH_PLACE, EIGHT_PLACE, NINTH_PLACE, TENTH_PLACE};

    // iterate on the States map, check each state's top 10 & update the points list
    MAP_FOREACH(int*, iterator, states) {
        // get the state's data
        StateData state_data = mapGet(states, iterator);
        assert(state_data != NULL);

        // create the state's sorted vote list:
        List state_vote = convertVotesToList(state_data->votes);

        // update the audience_points list according to the top 10
        CountData state_vote_iterator = listGetFirst(state_vote);
        for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
            // if end of state's votes list, break
            if (state_vote_iterator == NULL) break;
            // iterate on audience points list & find the state & add the points
            LIST_FOREACH(CountData, audience_points_iterator, audience_points){
                if (audience_points_iterator->id == state_vote_iterator->id){
                    audience_points_iterator->count += audiencePrecent*ranking[i];
                }
            }
            state_vote_iterator = listGetNext(state_vote);
        }
        // destroy the state_vote list
        listDestroy(state_vote);
    }

    return audience_points;
}

/********************************************* COUNT LIST FUNCTIONS **********************************************************/

ListElement copyIntPair(ListElement elem) {

}

void freeIntPair(ListElement elem) {

}

int compareIntPairs(ListElement data1, ListElement data2) {

}

List countListCreate(Map map) {
    // check parameter?

    List list = listCreate(copyIntPair, freeIntPair);
    if (!list) return NULL;

    MAP_FOREACH(int*, key, map) {
        CountData data = malloc(sizeof(*data));
        if (!data) {
            listDestroy(list);
            return NULL;
        }

        data->id = *key;
        data->count = 0; // intialize to 0

        ListResult result = listInsertFirst(list, data);
        freeIntPair(data);

        if (result != LIST_SUCCESS) {
            listDestroy(list);
            return NULL;
        }
    }

    return list;
}


List convertVotesToList(Map votes) {
    // check parameter?

    List list = countListCreate(votes);

    LIST_FOREACH(CountData, elem, list) {
        int* data = mapGet(votes, &(elem->id));
        if (!data) {
            listDestroy(list);
            return NULL;
        }
        elem->count = *data;
    }

    ListResult result = listSort(list, compareIntPairs);
    if (result != LIST_SUCCESS) {
        listDestroy(list);
        return NULL;
    }

    return list;
}

List convertToStringlist(List finalResults, Map states) {
    // check parameters?
    List state_names = listCreate(copyString, freeString);

    LIST_FOREACH(CountData, elem, finalResults) {
        int stateId = elem->id;
        StateData data = mapGet(states, &stateId);
        if (!data) {
            listDestroy(state_names);
            return NULL;
        }

        ListResult result = listInsertLast(state_names, data->name);
        if (result != LIST_SUCCESS) {
            listDestroy(state_names);
            return NULL;
        }
    }

    return state_names;
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

ListElement copyIntPair(ListElement elem) {
    if (elem == NULL) return NULL;

    CountData new_elem = malloc(sizeof(*elem));
    if (new_elem == NULL) return NULL;

    CountData elem_p = elem;
    new_elem->count = elem_p->count;
    new_elem->id = elem_p->id;

    return new_elem;
}

void freeIntPair(ListElement elem) {
    free(elem);
}

int compareIntPairs(ListElement data1, ListElement data2) {
    if (data1 == NULL || data2 == NULL) return NULL;

    CountData data1_p = data1;
    CountData data2_p = data2;

    // if data1 need to come before data2 return POSITIVE_NUM, else return NEGATIVE_NUM

    if (data1_p->count == data2_p->count) {
        if (data1_p->id < data2_p->id) return POSITIVE_NUM;
        //else
        return NEGATIVE_NUM;
    }

    if (data1_p->count > data2_p->count) return POSITIVE_NUM;
    //else
    return NEGATIVE_NUM;
}
