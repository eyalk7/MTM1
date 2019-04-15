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
        if (!isLowerCase(name[i]) && name[i] != SPACE) return false;
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

List audiencePoints(Map states) {
    // create a points list with all states & Ranking table
    List audience_points = countListCreate(states);
    Ranking ranking[10] = {FIRST_PLACE, SECOND_PLACE, THIRD_PLACE, FOURTH_PLACE, FIFTH_PLACE, SIXTH_PLACE, SEVENTH_PLACE, EIGHT_PLACE, NINTH_PLACE, TENTH_PLACE};

    // iterate on the States map:
    MAP_FOREACH(int*, iterator, states) {
        // get the state's data
        StateData state_data = mapGet(states, iterator);
        assert(state_data != NULL);

        // create the state's vote list:
        List state_vote = convertVotesToList(state_data->votes);

        //update the audience_points array by the ten most voted
        CountData state_vote_iterator = listGetFirst(state_vote);
        for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
            LIST_FOREACH(CountData, audience_points_iterator, audience_points){
                if (audience_points_iterator->id == state_vote_iterator->id){
                    audience_points_iterator->count += ranking[i];
                }
            }
            state_vote_iterator = listGetNext(state_vote);
        }
    }
}
/********************************************* COUNT LIST FUNCTIONS **********************************************************/

ListElement copyCountData(ListElement elem) {

}

void freeCountData(ListElement elem) {

}

int compareCountData(ListElement data1, ListElement data2) {

}

List countListCreate(Map map) {
    List list = listCreate(copyCountData, freeCountData);
    if (!list) return NULL;

    MAP_FOREACH(MapKeyElement, key, map) {
        int id = *(int*)key;
        CountData data = malloc(sizeof(*data));
        if (!data) {
            listDestroy(list);
            return NULL;
        }

        data->id = id;
        
    }

    return list;
}


List convertVotesToList(Map votes) {

}

void freeCountList(CountData* countTable) {

}

List convertToStringlist(List countList) {

}
