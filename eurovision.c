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
        tmp_judge_data->results[i] = judgeResults[i];
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

    // if state map is empty return empty List
    if (mapGetFirst(eurovision->States) == NULL) {
        List winners_list = listCreate(copyString, freeString);
        return winners_list;
    }

    // get the audience points
    List points_list = audiencePoints(eurovision->States);
    if (points_list == NULL) return NULL;

    // get the judges's points:
    /*List judges_points = countListCreate(eurovision->States);
    if (judges_points == NULL) {
        listDestroy(audience_points);
        return NULL;
    }*/

    // if there are judges, update the points list according to the judges's results
    if (mapGetFirst(eurovision->Judges) != NULL) {
        MAP_FOREACH(JudgeData, iterator, eurovision->Judges) {
            int *judge_results = iterator->results;
            for (int i = 0; i < NUMBER_OF_STATES_TO_RANK; i++) {
                // iterate on judges points list & find the state & add the points
                LIST_FOREACH(CountData, judges_points_iterator, judges_points) {
                    if (judge_results[i] == judges_points_iterator->id) {
                        judges_points_iterator->count += ranking[i];
                    }
                }
            }
        }
    }

    // create final_points list and calculate the points according to the audiencePrecent
    List final_points = countListCreate(eurovision->States);
    if (final_points == NULL){
        listDestroy(audience_points);
        listDestroy(judges_points);
        return NULL;
    }

    //run on the audience_points and judges_points and add the calculated grade by the precentage
    CountData audience_points_iterator = listGetFirst(audience_points);
    CountData judges_points_iterator = listGetFirst(judges_points);
    CountData final_points_iterator = listGetFirst(final_points);
    while (final_points_iterator != NULL) {
        assert(audience_points_iterator->id == judges_points_iterator->id);
        assert(audience_points_iterator->id == final_points_iterator->id);
        final_points_iterator->count = audiencePercent*(audience_points_iterator->count) + (100-audiencePercent)*(judges_points_iterator->count);
        audience_points_iterator = listGetNext(audience_points);
        judges_points_iterator = listGetNext(judges_points);
        final_points_iterator = listGetNext(final_points);
    }

    // sort the final list
    if (listSort(final_points, compareCountData) != LIST_SUCCESS) return NULL;

    // convert to names list
    List winners_list = convertToStringlist(final_points, eurovision->States);
    if (winners_list == NULL) return NULL;

    // destroy all Lists
    listDestroy(audience_points);
    listDestroy(judges_points);
    listDestroy(final_points);

    return winners_list;
}

List eurovisionRunAudienceFavorite(Eurovision eurovision) {
    // check valid arguments
    if (eurovision == NULL) return NULL;

    // get audience Points
    List audience_points = audiencePoints(eurovision->States);
    if (audience_points == NULL) return NULL;

    // sort the final list
    if (listSort(audience_points, compareCountData) != LIST_SUCCESS) return NULL;

    // convert to names list
    List winners_list = convertToStringlist(audience_points, eurovision->States);
    if (winners_list == NULL) return NULL;

    // destroy all lists
    listDestroy(audience_points);

    return winners_list;
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