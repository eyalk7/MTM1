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
        if (type == STATES_MAP) {
            return EUROVISION_STATE_ALREADY_EXIST;
        }
        // else type == JUDGES_MAP
        return EUROVISION_JUDGE_ALREADY_EXIST;
    }

    // else state not exist
    if (type == STATES_MAP) {
        return EUROVISION_STATE_NOT_EXIST;
    }
    // else type == JUDGES_MAP
    return EUROVISION_JUDGE_NOT_EXIST;
}

bool isLowerCase(char c) {
    return ('a' <= c && c <= 'z');
}

bool checkValidName(char* name) {
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

bool resultsContain (Eurovision eurovision, int judge_id, int state_id) {
    // assert valid arguments (checked already in the sending function)
    assert(eurovision != NULL && isIDValid(eurovision->States, STATES_MAP, state_id) == EUROVISION_STATE_ALREADY_EXIST);

    // get the judge's data & assert judge_id exist in the map
    JudgeDataElement tmp_judge = mapGet(eurovision->Judges, &judge_id);
    assert(tmp_judge != NULL);

    // iterate on the judge's results and check if state_id is there
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        if (tmp_judge->states[i] == state_id) {
            return true;
        }
    }

    // state_id not on judge's results
    return false;
}
