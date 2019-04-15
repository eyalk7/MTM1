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

/********************************************* COUNT LIST FUNCTIONS **********************************************************/

ListElement copyCountData(ListElement elem) {

}

void freeCountData(ListElement elem) {

}

int compareCountData(ListElement data1, ListElement data2) {

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
        data->count = 0; // intialize to 0

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
        elem->count = *data;
    }

    ListResult result = listSort(list, compareCountData);
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

