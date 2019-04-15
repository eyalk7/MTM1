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
FreeListElement freeString(ListElement str);
EurovisionResult eurovisionChangeVote(Eurovision eurovision, int stateGiver, int stateTaker, int diff);

struct eurovision_t {
    Map States; // keys = StateId, data = StateData
    Map Judges; // keys = JudgeId, data = JudgeData
};

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
        StateData state_data = mapGet(eurovision->States, iterator);

        assert(state_data->votes != NULL);
        mapRemove(state_data->votes, &stateId);
    }

    //iterate on "Judges" map & remove judges that voted for the given stateId
    MAP_FOREACH(int *, iterator, eurovision->Judges) {
        if (resultsContain(eurovision->States, eurovision->Judges, *iterator, stateId)) {
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
        id_validation = isIDValid(eurovision->States, STATES_MAP, judgeResults[i]);
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
    // check valid arguments
    if (eurovision == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    EurovisionResult id_validation = isIDValid(eurovision->Judges, JUDGES_MAP, judgeId);
    assert(id_validation == EUROVISION_JUDGE_ALREADY_EXIST || id_validation == EUROVISION_INVALID_ID || id_validation == EUROVISION_JUDGE_NOT_EXIST);
    if (id_validation != EUROVISION_JUDGE_ALREADY_EXIST) {
        return id_validation;
    }

    // remove judge from "Judges" map
    mapRemove(eurovision->Judges, &judgeId);

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver,
                                   int stateTaker) {
    return eurovisionChangeVote(eurovision, stateGiver, stateTaker, 1);
}


EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker) {
    return eurovisionChangeVote(eurovision, stateGiver, stateTaker, -1);
}

List eurovisionRunContest(Eurovision eurovision, int audiencePercent) {
    // check valid arguments
    if (eurovision == NULL || audiencePercent > 100 || audiencePercent < 0) return NULL;

    List winners_list = listCreate(copyString, freeString);
    // if state map is empty return empty List
    if (mapGetFirst(eurovision->States) == NULL) return winners_list;

        //outsize function - audiencePoints(eurovision) - returns List with all of the below:

    //outside function - judges_points = countListCreate(states map) - initialize to zero & add stateId

    //iterate on Judges map and for each judge:
        //update the judges_grades List by the judge's results
        //enum {FIRST_PLACE, SECOND_PLACE....

    //outside function - final_points = countListCreate(states map) - initialize to zero & add stateId

        //run on the audience_points and judges_points and add the :
            //the calculated grade by the precentage

    //outside function - compare - from big to small - if same grade, sort by stateId - low before high (!!)

    //outside function - countListToWinnersList(final_grades) - in "functions"
        //ListCreate with copyString and freeString functions
        //insert all final_grade array names to the list by the new order (with mapGet)

    // free all Lists

}

List eurovisionRunAudienceFavorite(Eurovision eurovision) {
    return NULL;
    //eurovision check
    // get audiencePoints(eurovision)
    //outside function - countListToWinnersList(final_grades) - in "functions"

}

List eurovisionRunGetFriendlyStates(Eurovision eurovision) {
    return NULL;
    //getSize of States map - num_of_states
    // if state map is empty return empty List

    //create two dimnesinal int array - state_favorites[2][num_of_states]
    //iterate on the states map and fill on each state:
        //insert stateId first column
        //outside function - getFavoriteState(votes map) - second column

    //iterate on the array, in each row:
        // iterate on the entire array and check if "it's a match", skip on "-1"
        // if it is a match save the states pair names on the list - after lexicorgraphic sort
        // change to "-1" the both rows

   // sort the strings array lexicorgraphic
}

//Functions for List
ListElement copyString(ListElement str) {
    char* copy = malloc(strlen(str) + 1);
    if (!copy) return NULL;

    strcpy(copy, str);

    return copy;
}

FreeListElement freeString(ListElement str) {
    free(str);
}

EurovisionResult eurovisionChangeVote(Eurovision eurovision, int stateGiver, int stateTaker, int diff) {
    // check valid arguments
    if (eurovision == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    EurovisionResult id_validation1 = isIDValid(eurovision->States, STATES_MAP, stateGiver);
    assert(id_validation1 == EUROVISION_STATE_ALREADY_EXIST || id_validation1 == EUROVISION_INVALID_ID ||
           id_validation1 == EUROVISION_STATE_NOT_EXIST);
    if (id_validation1 != EUROVISION_STATE_ALREADY_EXIST) {
        return id_validation1;
    }
    EurovisionResult id_validation2 = isIDValid(eurovision->States, STATES_MAP, stateTaker);
    assert(id_validation2 == EUROVISION_STATE_ALREADY_EXIST || id_validation2 == EUROVISION_INVALID_ID ||
           id_validation2 == EUROVISION_STATE_NOT_EXIST);
    if (id_validation2 != EUROVISION_STATE_ALREADY_EXIST) {
        return id_validation2;
    }

    // check that stategiver != stateTaker
    if (stateGiver == stateTaker) {
        return EUROVISION_SAME_STATE;
    }

    // check current num of votes for stateTaker in stateGiver's votes map
    StateData giver_data = mapGet(eurovision->States, &stateGiver);
    assert(giver_data != NULL);
    int *cur_votes_num = mapGet(giver_data->votes, &stateTaker);

    // if no votes & diff <= 0 just return
    if (cur_votes_num == NULL && diff <= 0) {
        return EUROVISION_SUCCESS;
    }

    // sum up the current num of votes and wanted difference
    diff += (*cur_votes_num);

    // if the sum <= 0 delete the state from the votes map
    if (diff <= 0) {
        mapRemove(giver_data->votes, &stateTaker);
        return EUROVISION_SUCCESS;
    }

    // else update the votes map
    if (mapPut(giver_data->votes, &stateTaker, &diff) == MAP_OUT_OF_MEMORY) {
        return EUROVISION_OUT_OF_MEMORY;
    }

    return EUROVISION_SUCCESS;
}