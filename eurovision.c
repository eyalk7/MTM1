#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "map.h"
#include "eurovision.h"
#include "list.h"
#include "functions.h"
#include "judge.h"
#include "state.h"


ListElement copyString(ListElement str);
void freeString(ListElement str);


struct eurovision_t {
    Map States; // keys = StateId, data = StateData
    Map Judges; // keys = JudgeId, data = JudgeData
};

int main () {

}

Eurovision eurovisionCreate() {
    // memory allocation for the struct and check
    Eurovision eurovision = malloc(sizeof(*eurovision));
    if (!eurovision) return NULL;

    // mapCreate for the states and judges with functions for the States and Judges maps
    //initialize the parameters of eurovision with the maps

    eurovision->States = mapCreate(copyStateDataElement, copyStateKeyElement, freeStateDataElement, freeStateKeyElement, compareStateKeyElements);
    if (!eurovision->States) {
        free(eurovision);
        return NULL;
    }

    eurovision->Judges = mapCreate(copyJudgeDataElement, copyJudgeKeyElement, freeJudgeDataElement, freeJudgeKeyElement, compareJudgeKeyElements);

    //check mapCreate return value
    if (!eurovision->Judges) {
        mapDestroy(eurovision->States);
        free(eurovision);
        return NULL;
    }

    // return pointer
    return eurovision;
}

void eurovisionDestroy(Eurovision eurovision) {
    if (eurovision) {
        //mapDestroy for the states votes map if exists
        //mapDestroy for the States and Judges maps
        mapDestroy(eurovision->States); // (votes map is destroyed on freeStateDataElement function)
        mapDestroy(eurovision->Judges);
        //free eurovision
        free(eurovision);
    }
    // else: do nothing
}

EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId,
                                    const char *stateName,
                                    const char *songName) {
    // check valid arguments
    if (eurovision == NULL || stateName == NULL || songName == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    EurovisionResult id_validation = isIDValid(eurovision->States, STATES_MAP, stateId);
    assert(id_validation == EUROVISION_STATE_ALREADY_EXIST || id_validation == EUROVISION_INVALID_ID || id_validation == EUROVISION_STATE_NOT_EXIST);
    if (id_validation != EUROVISION_STATE_NOT_EXIST) {
        return id_validation;
    }
    if (!checkValidName(stateName) || !checkValidName(songName)) {
        return EUROVISION_INVALID_NAME;
    }

    // memory alloctation for the tmp_state_data & for names and check
    StateData tmp_state_data = malloc(sizeof(*tmp_state_data));
    if (tmp_state_data == NULL) {
        return EUROVISION_OUT_OF_MEMORY;
    }
    char *tmp_stateName = malloc(strlen(stateName) + 1);
    if (tmp_stateName == NULL) {
        free (tmp_state_data);
        return EUROVISION_OUT_OF_MEMORY;
    }
    char *tmp_songName = malloc(strlen(songName) + 1);
    if (tmp_songName == NULL) {
        free (tmp_state_data);
        free(tmp_stateName);
        return EUROVISION_OUT_OF_MEMORY;
    }

    // Create tmp_votes and add votes functions,
    Map tmp_votes = mapCreate(copyVoteDataElement, copyVoteKeyElement, freeVoteDataElement, freeVoteKeyElement, compareVoteKeyElements);
    if (tmp_votes == NULL) {
        free (tmp_state_data);
        free(tmp_stateName);
        free(tmp_songName);
        return EUROVISION_OUT_OF_MEMORY;
    }

    // initalize the tmp_state_data element
    strcpy(tmp_stateName, stateName);
    strcpy(tmp_songName, songName);
    tmp_state_data->name = tmp_stateName;
    tmp_state_data->song_name = tmp_songName;
    tmp_state_data->votes = tmp_votes;

    // add the state to the "States" map in "eurovision"
    if (mapPut(eurovision->States, &stateId, tmp_state_data) == MAP_OUT_OF_MEMORY) {
        free(tmp_state_data);
        free(tmp_stateName);
        free(tmp_songName);
        mapDestroy(tmp_votes);
        return EUROVISION_OUT_OF_MEMORY;
    }

    // free tmp memory allocations
    free (tmp_state_data);
    free(tmp_stateName);
    free(tmp_songName);
    mapDestroy(tmp_votes);

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId) {
    // check valid arguments
    if (eurovision == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    EurovisionResult id_validation = isIDValid(eurovision->States, STATES_MAP, stateId);
    if (id_validation != EUROVISION_STATE_ALREADY_EXIST) {
        return id_validation;
    }

    //iterate on "States" map & remove from each state's "Votes" map the given stateId
    MAP_FOREACH(int *, iterator, eurovision->States) {
        assert(iterator->votes != NULL);
        mapRemove(iterator->votes, &stateId);
    }

    //iterate on "Judges" map & remove judges that voted for the given stateId
    MAP_FOREACH(int *, iterator, eurovision->Judges) {
        if (resultsContain(eurovision, *iterator, stateId)) {
            eurovisionRemoveJudge(eurovision, *iterator);
        }
    }

    // remove the state from "States" map
    mapRemove(eurovision->States, &stateId);

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults) {
    // check valid arguments
    if (eurovision == NULL || judgeName == NULL || judgeResults == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    EurovisionResult id_validation = isIDValid(eurovision->Judges, JUDGES_MAP, judgeId);
    assert(id_validation == EUROVISION_JUDGE_ALREADY_EXIST || id_validation == EUROVISION_INVALID_ID || id_validation == EUROVISION_JUDGE_NOT_EXIST);
    if (id_validation != EUROVISION_JUDGE_NOT_EXIST) {
        return id_validation;
    }
    if (!checkValidName(judgeName)) {
        return EUROVISION_INVALID_NAME;
    }
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        EurovisionResult id_validation = isIDValid(eurovision->States, STATES_MAP, judgeResults[i]);
        assert(id_validation == EUROVISION_STATE_ALREADY_EXIST || id_validation == EUROVISION_INVALID_ID || id_validation == EUROVISION_STATE_NOT_EXIST);
        if (id_validation != EUROVISION_STATE_ALREADY_EXIST) {
            return id_validation;
        }
    }

    // memory alloctation for the tmp_judge_data & for name and check
    JudgeData tmp_judge_data = malloc(sizeof(*tmp_judge_data));
    if (tmp_judge_data == NULL) {
        return EUROVISION_OUT_OF_MEMORY;
    }
    char *tmp_judgeName = malloc(strlen(judgeName) + 1);
    if (tmp_judgeName == NULL) {
        free(tmp_judge_data);
        return EUROVISION_OUT_OF_MEMORY;
    }

    // initalize the tmp_judge_data element
    strcpy(tmp_judgeName, judgeName);
    tmp_judge_data->name = tmp_judgeName;
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        tmp_judge_data->states[i] = judgeResults[i];
    }

    // add the judge to the "Judges" map in "eurovision"
    if (mapPut(eurovision->Judges, &judgeId, tmp_judge_data) == MAP_OUT_OF_MEMORY) {
        free(tmp_judge_data);
        free(tmp_judgeName);
        return EUROVISION_OUT_OF_MEMORY;
    }

    // free tmp memory allocations
    free(tmp_judge_data);
    free(tmp_judgeName);

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId) {
    //check if id < 0
    //check NULL
    //mapRemove with judgeId
    //check return value
}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver,
                                   int stateTaker) {
    //check NULL
    // outside function - isIDValid(Map map, int id) on both id's (giver and taker)
    //check if stategiver != stateTaker
    //in the State map send stateGiver to mapGet
    //check return value and return if don't exist
    //with the stateData go to the votes map
    //with mapGet check how much votes there is for the stateTaker (key) id (or if not at all)
    //check return value
    //with mapPut add/update the vote count for the stateTaker(key) current votes+1 (data)
    //check return value
}

EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker) {
    //check NULL
    //check if id's < 0
    //check if stategiver != stateTaker
    //in the State map send stateGiver to mapGet
    //check return value and return if don't exist
    //with the stateDate go to the votes map
    //with mapGet check how much votes there is for the stateTaker (key) id (or if not at all)
    //check return value if doesn't exist return success
    //with mapPut update the vote count for the stateTaker(key) current votes-1 (data)
    //check return value
}

List eurovisionRunContest(Eurovision eurovision, int audiencePercent) {
    // eurovision NULL check
    //check precent (1-100) else return NULL

    //getSize of States map - num_of_states
    // if state map is empty return empty List

    //create int array audience_grades[2][num_of_states], initialize to zero
    //iterate with MAP_FOREACH in the States map:
        // add stateId in int array
    //iterate with MAP_FOREACH in the States map:
        //iterate in the votes map of that State
            //create array of struct for id+votes state_votes[num_of_states] array initialize to 0
            //iterate on the vote map, save the id and the vote_count on the array
                //sort the array if same grade, sort by stateId
            //update the audience_grades array by the ten most voted
            //enum {FIRST_PLACE, SECOND_PLACE....
    //create int array judges_grades[2][num_of_states], initialize to zero
        //iterate with MAP_FOREACH in the States map:
        // add stateId in int array
    //iterate on Judges map and for each judge
        //update the judges_grades array by the judge's results
        //enum {FIRST_PLACE, SECOND_PLACE....
    //special struct array final_grades[num_of_states], intialize to 0
        //run on the audience_grades and judges grades and add the :
            //stateId, and the calculated grade by the precentage
        //sort the array if same grade, sort by stateId

    //ListCreate with copyString and freeString functions
    //insert the top ten of final_grade array names to the list (with mapGet)

}

List eurovisionRunAudienceFavorite(Eurovision eurovision) {
    //create based on the above function (runConstest)
}

List eurovisionRunGetFriendlyStates(Eurovision eurovision) {
    //create based on the above function
    //in each state after finished the "state_vote" array (sorted)
    //two dimensinal array left cullumn stateId, right column the no. 1 of that state
    //iterate on the array and check for each row if "it's a match"
    //if it is a match save the states pair on another array (check for duplications)
    //print it how they wanted
}

//Functions for List
ListElement copyString(ListElement str) {

}

void freeString(ListElement str) {

}