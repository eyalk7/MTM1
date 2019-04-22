#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "functions.h"

/*********************** EUROVISION HELP FUNCTIONS *******************************/
EurovisionResult checkIDValid(Map map, MapType type, int id) {
    // assert valid arguments (checked already in the sending function)
    assert(map != NULL);
    assert(type == STATES_MAP || type == JUDGES_MAP);

    //check ID >= 0
    if (id < 0) return EUROVISION_INVALID_ID;

    // check if Id already exist in the given map
    if (mapContains(map, &id)) {
        if (type == STATES_MAP) return EUROVISION_STATE_ALREADY_EXIST;
        return EUROVISION_JUDGE_ALREADY_EXIST;  // else type == JUDGES_MAP
    }


    if (type == STATES_MAP) return EUROVISION_STATE_NOT_EXIST;
    return EUROVISION_JUDGE_NOT_EXIST;  // else type == JUDGES_MAP
}

bool checkLowerCase(char c) {
    return ('a' <= c && c <= 'z');
}

bool checkValidName(const  char* name) {
    //check the given string only contains small letters and spaces
    for (int i = 0; i < strlen(name); i++) {
        if (!checkLowerCase(name[i]) && name[i] != ' ') return false;
    }
    return true;
}

void* copyInt(void* integer) {
    //allocation for int
    int* copy = malloc(sizeof(*copy));
    //initialize the int
    *copy = *(int*)integer;
    //return address
    return copy;
}
void freeInt(void* integer) {
    free(integer);
}
int compareInts(MapKeyElement integer1, MapKeyElement integer2) {
    //compare the ints
    //return 0 if equal
    //return bigger than 0 if first is bigger
    //else smaller than 0
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

EurovisionResult eurovisionChangeVote(Map states,
                                      int stateGiver,
                                      int stateTaker,
                                      int difference) {
    // check valid arguments
    if (states == NULL) return EUROVISION_NULL_ARGUMENT;
    EurovisionResult id_validation = checkIDValid(states, STATES_MAP, stateGiver);

    assert(id_validation == EUROVISION_STATE_ALREADY_EXIST ||
           id_validation == EUROVISION_INVALID_ID ||
           id_validation == EUROVISION_STATE_NOT_EXIST);

    if (id_validation != EUROVISION_STATE_ALREADY_EXIST) return id_validation;
    id_validation = checkIDValid(states, STATES_MAP, stateTaker);

    assert(id_validation == EUROVISION_STATE_ALREADY_EXIST ||
           id_validation == EUROVISION_INVALID_ID ||
           id_validation == EUROVISION_STATE_NOT_EXIST);

    if (id_validation != EUROVISION_STATE_ALREADY_EXIST) return id_validation;

    // check that stategiver != stateTaker
    if (stateGiver == stateTaker) return EUROVISION_SAME_STATE;

    // check current num of votes for stateTaker in stateGiver's votes map
    StateData giver_data = mapGet(states, &stateGiver);
    assert(giver_data != NULL);
    int *current_votes_num = mapGet(giver_data->votes, &stateTaker);

    // if no votes & difference <= 0 just return
    if (current_votes_num == NULL && difference <= 0) return EUROVISION_SUCCESS;

    // if there are votes sum up the current num of votes and wanted difference
    if (current_votes_num != NULL) {
        difference += (*current_votes_num);

        // if the sum <= 0 and there are votes - delete the state from the votes map
        if (difference <= 0) {
            mapRemove(giver_data->votes, &stateTaker);
            return EUROVISION_SUCCESS;
        }
    }

    // else update the votes map
    if (mapPut(giver_data->votes, &stateTaker, &difference) == MAP_OUT_OF_MEMORY) {
        return EUROVISION_OUT_OF_MEMORY;
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
ListElement copyCountData(ListElement elem) {
    if (elem == NULL) return NULL;

    CountData elem_p = elem;
    CountData copy = malloc(sizeof(*elem_p));
    if (copy == NULL) return NULL;

    copy->voteCount = elem_p->voteCount;
    copy->id = elem_p->id;

    return copy;
}

void freeCountData(ListElement elem) {
    free(elem);
}

int compareCountData(ListElement data1, ListElement data2) {
    assert(data1 != NULL && data2 != NULL);

    CountData data1_p = data1;
    CountData data2_p = data2;

    // if data1 need to come before data2 return FIRST_BEFORE_SECOND,
    // else return SECOND_BEFORE_FIRST

    if (data1_p->voteCount == data2_p->voteCount) {
        if (data1_p->id < data2_p->id) return FIRST_BEFORE_SECOND;
        //else
        return SECOND_BEFORE_FIRST;
    }

    if (data1_p->voteCount > data2_p->voteCount) return FIRST_BEFORE_SECOND;
    //else
    return SECOND_BEFORE_FIRST;
}

List countListCreate(Map map) {
    // check parameter?

    List list = listCreate(copyCountData, freeCountData);
    if (!list) return NULL;

    MAP_FOREACH(int*, key, map) {
        CountData data = malloc(sizeof(*data));
        if (!data) {
            listDestroy(list);
            return NULL;
        }

        data->id = *key;
        data->voteCount = 0; // initialize to 0

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
    // check parameter?

    List list = countListCreate(votes);

    LIST_FOREACH(CountData, elem, list) {
        int* data = mapGet(votes, &(elem->id));
        if (!data) {
            listDestroy(list);
            return NULL;
        }
        elem->voteCount = *data;
    }

    ListResult result = listSort(list, compareCountData);
    if (result != LIST_SUCCESS) {
        listDestroy(list);
        return NULL;
    }

    return list;
}

List convertToStringList(List finalResults, Map states) {
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

/***************************** CONTEST FUNCTIONS ********************************/
List getAudiencePoints(Map states, int audiencePercent) {
    // create an audience points list with all states
    List audience_points = countListCreate(states);
    if (!audience_points) return NULL;

    Ranking ranking[NUMBER_OF_STATES_TO_RANK] = {FIRST_PLACE, SECOND_PLACE,
                                                 THIRD_PLACE, FOURTH_PLACE,
                                                 FIFTH_PLACE, SIXTH_PLACE,
                                                 SEVENTH_PLACE, EIGHT_PLACE,
                                                 NINTH_PLACE, TENTH_PLACE};

    // iterate on the States map, check each state's top 10 & update the points list
    MAP_FOREACH(int*, iterator, states) {
        // get the state's data
        StateData state_data = mapGet(states, iterator);
        assert(state_data != NULL);

        // create the state's sorted vote list:
        List state_vote = convertVotesToList(state_data->votes);
        if (!state_vote) {
            listDestroy(audience_points);
            return NULL;
        }

        // update the audience_points list according to the top 10
        CountData state_vote_iterator = listGetFirst(state_vote);
        for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
            // if end of state's votes list, break
            if (state_vote_iterator == NULL) break;
            // iterate on audience points list & find the state & add the points
            LIST_FOREACH(CountData, audience_points_iterator, audience_points){
                if (audience_points_iterator->id == state_vote_iterator->id){
                    audience_points_iterator->voteCount += audiencePercent*ranking[i];
                }
            }
            state_vote_iterator = listGetNext(state_vote);
        }
        // destroy the state_vote list
        listDestroy(state_vote);
    }

    return audience_points;
}
