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

struct eurovision_t {
    Map States; // keys = State ID, data = StateData
    Map Judges; // keys = Judge ID, data = JudgeData
};

Eurovision eurovisionCreate() {
    // memory allocation for the struct and check
    Eurovision eurovision = malloc(sizeof(*eurovision));
    if (!eurovision) return NULL;

    // mapCreate for the states and judges with functions for the States and Judges maps
    //initialize the parameters of eurovision with the maps

    eurovision->States = mapCreate(copyStateDataElement,
                                   copyStateKeyElement,
                                   freeStateDataElement,
                                   freeStateKeyElement,
                                   compareStateKeyElements);
    if (!eurovision->States) {
        free(eurovision);
        return NULL;
    }

    eurovision->Judges = mapCreate(copyJudgeDataElement,
                                   copyJudgeKeyElement,
                                   freeJudgeDataElement,
                                   freeJudgeKeyElement,
                                   compareJudgeKeyElements);

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
        // (votes map is destroyed on freeStateDataElement function)
        mapDestroy(eurovision->States);
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
    if (!eurovision || !stateName || !songName) return EUROVISION_NULL_ARGUMENT;
    EurovisionResult id_validation = checkIDValid(eurovision->States,
                                                  STATES_MAP,
                                                  stateId);

    assert(id_validation == EUROVISION_STATE_ALREADY_EXIST ||
           id_validation == EUROVISION_INVALID_ID ||
           id_validation == EUROVISION_STATE_NOT_EXIST);

    if (id_validation != EUROVISION_STATE_NOT_EXIST) return id_validation;
    if (!checkValidName(stateName) || !checkValidName(songName)) return EUROVISION_INVALID_NAME;

    // memory alloctation for the tmp_state_data & for names and check
    StateData tmp_state_data = malloc(sizeof(*tmp_state_data));
    if (!tmp_state_data) return EUROVISION_OUT_OF_MEMORY;
    char *tmp_stateName = malloc(strlen(stateName) + 1);
    if (!tmp_stateName) {
        free (tmp_state_data);
        return EUROVISION_OUT_OF_MEMORY;
    }
    char *tmp_songName = malloc(strlen(songName) + 1);
    if (!tmp_songName) {
        free (tmp_state_data);
        free(tmp_stateName);
        return EUROVISION_OUT_OF_MEMORY;
    }

    // Create tmp_votes and add votes functions,
    Map tmp_votes = mapCreate(copyVoteDataElement,
                              copyVoteKeyElement,
                              freeVoteDataElement,
                              freeVoteKeyElement,
                              compareVoteKeyElements);
    if (!tmp_votes) {
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
    MapResult put_result = mapPut(eurovision->States, &stateId, tmp_state_data);

    // free tmp memory allocations
    free(tmp_state_data);
    free(tmp_stateName);
    free(tmp_songName);
    mapDestroy(tmp_votes);

    if (put_result == MAP_OUT_OF_MEMORY) return EUROVISION_OUT_OF_MEMORY;

    //else success
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId) {
    // check valid arguments
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;
    EurovisionResult id_validation = checkIDValid(eurovision->States,
                                                  STATES_MAP,
                                                  stateId);
    if (id_validation != EUROVISION_STATE_ALREADY_EXIST) return id_validation;

    //iterate on "States" map & remove from each state's "Votes" map the given stateId
    MAP_FOREACH(int *, iterator, eurovision->States) {
        StateData state_data = mapGet(eurovision->States, iterator);
        assert(state_data->votes != NULL);
        mapRemove(state_data->votes, &stateId);
    }

    //iterate on "Judges" map & remove judges that voted for the given stateId
    MAP_FOREACH(int *, iterator, eurovision->Judges) {
        JudgeData judgeData = mapGet(eurovision->Judges, iterator);
        if (judgeResultsContain(judgeData, stateId)) {
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
    if (!eurovision || !judgeName || !judgeResults) return EUROVISION_NULL_ARGUMENT;
    EurovisionResult id_validation = checkIDValid(eurovision->Judges,
                                                  JUDGES_MAP,
                                                  judgeId);

    assert(id_validation == EUROVISION_JUDGE_ALREADY_EXIST ||
           id_validation == EUROVISION_INVALID_ID ||
           id_validation == EUROVISION_JUDGE_NOT_EXIST);

    if (id_validation != EUROVISION_JUDGE_NOT_EXIST) return id_validation;
    if (!checkValidName(judgeName)) return EUROVISION_INVALID_NAME;
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        id_validation = checkIDValid(eurovision->States, STATES_MAP, judgeResults[i]);
        assert(id_validation == EUROVISION_STATE_ALREADY_EXIST || id_validation == EUROVISION_INVALID_ID || id_validation == EUROVISION_STATE_NOT_EXIST);
        if (id_validation != EUROVISION_STATE_ALREADY_EXIST) return id_validation;
    }

    // memory alloctation for the tmp_judge_data & for name and check
    JudgeData tmp_judge_data = malloc(sizeof(*tmp_judge_data));
    if (!tmp_judge_data) return EUROVISION_OUT_OF_MEMORY;
    char *tmp_judgeName = malloc(strlen(judgeName) + 1);
    if (!tmp_judgeName) {
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
    MapResult put_result = mapPut(eurovision->Judges, &judgeId, tmp_judge_data);

    // free tmp memory allocations
    free(tmp_judge_data);
    free(tmp_judgeName);

    if (put_result == MAP_OUT_OF_MEMORY) return EUROVISION_OUT_OF_MEMORY;

    // else, success
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId) {
    // check valid arguments
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;
    EurovisionResult id_validation = checkIDValid(eurovision->Judges,
                                                  JUDGES_MAP,
                                                  judgeId);

    assert(id_validation == EUROVISION_JUDGE_ALREADY_EXIST ||
           id_validation == EUROVISION_INVALID_ID ||
           id_validation == EUROVISION_JUDGE_NOT_EXIST);

    if (id_validation != EUROVISION_JUDGE_ALREADY_EXIST) return id_validation;

    // remove judge from "Judges" map
    mapRemove(eurovision->Judges, &judgeId);

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver,
                                   int stateTaker) {
    return eurovisionChangeVote(eurovision->States, stateGiver, stateTaker, 1);
}


EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker) {
    return eurovisionChangeVote(eurovision->States, stateGiver, stateTaker, -1);
}

List eurovisionRunContest(Eurovision eurovision, int audiencePercent) {
    // check valid arguments
    if (!eurovision || audiencePercent > 100 || audiencePercent < 0) return NULL;
    Ranking ranking[NUMBER_OF_STATES_TO_RANK] = {FIRST_PLACE, SECOND_PLACE,
                                                 THIRD_PLACE, FOURTH_PLACE,
                                                 FIFTH_PLACE, SIXTH_PLACE,
                                                 SEVENTH_PLACE, EIGHT_PLACE,
                                                 NINTH_PLACE, TENTH_PLACE};

    // if state map is empty return empty List
    if (mapGetFirst(eurovision->States) == NULL) return listCreate(copyString, freeString);

    // get the audience points
    List points_list = getAudiencePoints(eurovision->States, audiencePercent);
    if (!points_list) return NULL;

    // update the points list according to the judges's results
    MAP_FOREACH(JudgeKeyElement, iterator, eurovision->Judges) {
        // get the judge's data
        JudgeData judge_data = mapGet(eurovision->Judges, iterator);
        assert(judge_data != NULL);
        // get the judge's results
        int *judge_results = judge_data->results;
        // compare each judge's result with all of the states, if match add points
        for (int i = 0; i < NUMBER_OF_STATES_TO_RANK; i++) {
            LIST_FOREACH(CountData, points_list_iterator, points_list) {
                if (judge_results[i] == points_list_iterator->id) {
                    points_list_iterator->voteCount += (100-audiencePercent)*ranking[i];
                }
            }
        }
    }

    // sort the final list
    if (listSort(points_list, compareCountData) != LIST_SUCCESS){
        listDestroy(points_list);
        return NULL;
    }

    // convert to names list & destroy & return
    List winners_list = convertToStringList(points_list, eurovision->States);
    listDestroy(points_list);
    return winners_list;
}

List eurovisionRunAudienceFavorite(Eurovision eurovision) {
    // check valid arguments
    if (!eurovision) return NULL;

    // get audience Points
    List audience_points = getAudiencePoints(eurovision->States, ONE_HUNDREND_PRECENT);
    if (!audience_points) return NULL;

    // sort the final list
    if (listSort(audience_points, compareCountData) != LIST_SUCCESS) {
        listDestroy(audience_points);
        return NULL;
    }

    // convert to names list & destroy list & return
    List winners_list = convertToStringList(audience_points, eurovision->States);
    listDestroy(audience_points);
    return winners_list;
}

List eurovisionRunGetFriendlyStates(Eurovision eurovision) {
    //getSize of States map - num_of_states
    int num_of_states = mapGetSize(eurovision->States);

    List friendly_states = listCreate(copyString, freeString);
    if (!friendly_states) return NULL;  // allocation failed

    // if state map is empty return empty List
    if (num_of_states == 0) return friendly_states;

    //create map state_favorites - key = stateId, value = favStateId
    /// OUTSIDE FUNCTION ///
    Map state_favorites = getStateFavorites(eurovision->States);
    /// OUTSIDE FUNCTION ///

    if (!state_favorites) {
        listDestroy(friendly_states);
        return NULL;
    }

    //iterate on the array, in each row:
    MAP_FOREACH(int*, stateId, state_favorites) {
        // iterate on the entire array and check if "it's a match", skip on "-1"
        int *favState1 = mapGet(state_favorites, stateId);
        int *stateId2 = favState1;
        int *favState2 = mapGet(state_favorites, stateId2);

        // areFriendly also checks if the pointers are NULL ! :)
        if (checkFriendlyStates(stateId, favState1, stateId2, favState2)) {
            // if it is a match save the states pair names on the list - after lexicographical sort
            StateData state1 = mapGet(eurovision->States, stateId);
            StateData state2 = mapGet(eurovision->States, stateId2);

            // change both rows to "-1" => prevent the same pair from being chosen
            *favState1 = NO_FAVORITE_STATE;
            *favState2 = NO_FAVORITE_STATE;

            /// OUTSIDE FUNCTION ///
            char *statePair = getStatePair(state1, state2);
            /// OUTSIDE FUNCTION ///

            if (!statePair) {
                mapDestroy(state_favorites);
                listDestroy(friendly_states);
                return NULL;
            }

            ListResult result = listInsertLast(friendly_states, statePair);
            free(statePair);
            if (result != LIST_SUCCESS) {
                mapDestroy(state_favorites);
                listDestroy(friendly_states);
                return NULL;
            }

        }
    }

    mapDestroy(state_favorites);

    // sort the strings array lexicorgraphically
    if (listSort(friendly_states, stringCompare) != LIST_SUCCESS) {
        listDestroy(friendly_states);
        return NULL;
    }

    return friendly_states;
}

