#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "functions.h"

/*
 * These are included in functions.h:
 * #include "eurovision.h"
 * #include "map.h"
 * #include "list.h"
 * #include "judge.h"
 * #include "state.h"
*/

struct eurovision_t {
    Map States; // key = State ID, data = State's name, song name and votes it gives
    Map Judges; // key = Judge ID, data = Judge's name and results
};

Eurovision eurovisionCreate() {
    Eurovision eurovision = malloc(sizeof(*eurovision));    // allocate memory for the struct
    if (!eurovision) return NULL;       // allocation failed

    // create the States map using the appropriate functions (from state.h)
    eurovision->States = mapCreate(copyStateDataElement,
                                   copyStateKeyElement,
                                   freeStateDataElement,
                                   freeStateKeyElement,
                                   compareStateKeyElements);
    if (!eurovision->States) {
        free(eurovision);
        return NULL;                    // allocation failed
    }

    // create the Judges map using the appropriate functions (from judge.h)
    eurovision->Judges = mapCreate(copyJudgeDataElement,
                                   copyJudgeKeyElement,
                                   freeJudgeDataElement,
                                   freeJudgeKeyElement,
                                   compareJudgeKeyElements);

    //check mapCreate return value
    if (!eurovision->Judges) {
        mapDestroy(eurovision->States);
        free(eurovision);
        return NULL;                    // allocation failed
    }

    return eurovision;
}

void eurovisionDestroy(Eurovision eurovision) {
    if (eurovision) {
        // destroy the States and Judges maps:
        mapDestroy(eurovision->States);     // votes maps are destroyed in the freeStateDataElement function
        mapDestroy(eurovision->Judges);

        free(eurovision);                   // free the eurovision struct
    }
    // if NULL pointer was received, nothing is done
}

EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId,
                                    const char *stateName,
                                    const char *songName) {
    if (!eurovision || !stateName || !songName) return EUROVISION_NULL_ARGUMENT;    // NULL pointer received

    EurovisionResult result = isIDValid(eurovision->States, STATES_MAP, stateId);
    if (result != EUROVISION_STATE_NOT_EXIST) return result;    // ID not valid or state is already in Eurovision

    if (!isValidName(stateName) || !isValidName(songName)) {
        return EUROVISION_INVALID_NAME;                         // state name or song name not valid
    }

    // temporarily allocate memory for the state's data
    StateData state_data = createStateData(stateName, songName);
    if (!state_data) return EUROVISION_OUT_OF_MEMORY;   // state's data allocation failed

    // add the state to Eurovision's States
    MapResult put_result = mapPut(eurovision->States, &stateId, state_data);

    // deallocate the temporary state data
    freeStateDataElement(state_data);

    if (put_result == MAP_OUT_OF_MEMORY) return EUROVISION_OUT_OF_MEMORY;           // copy in mapPut failed

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId) {
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;       // NULL pointer received

    EurovisionResult result = isIDValid(eurovision->States, STATES_MAP, stateId);
    if (result != EUROVISION_STATE_ALREADY_EXIST) return result;    // ID not valid or state is not in Eurovision

    // for each state in Eurovision, remove the votes that state has for given stateId
    MAP_FOREACH(int *, id, eurovision->States) {
        StateData state_data = mapGet(eurovision->States, id);
        assert(state_data->votes != NULL);
        mapRemove(state_data->votes, &stateId);
    }

    // for each judge in Eurovision, remove it if it voted for the given stateId
    MAP_FOREACH(int *, judge_id, eurovision->Judges) {
        JudgeData judgeData = mapGet(eurovision->Judges, judge_id);
        if (judgeResultsContain(judgeData, stateId)) {
            eurovisionRemoveJudge(eurovision, *judge_id);
        }
    }

    // Remove the state from Eurovision's States
    mapRemove(eurovision->States, &stateId);

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults) {
    if (!eurovision || !judgeName || !judgeResults) return EUROVISION_NULL_ARGUMENT;    // NULL pointer received

    EurovisionResult result = isIDValid(eurovision->Judges, JUDGES_MAP, judgeId);
    if (result != EUROVISION_JUDGE_NOT_EXIST) return result;        // ID not valid or Judge already exists in Eurovision

    if (!isValidName(judgeName)) return EUROVISION_INVALID_NAME;    // judge name not valid

    // check valid judge results
    for (int i=0; i < NUMBER_OF_STATES_TO_RANK; i++) {
        result = isIDValid(eurovision->States, STATES_MAP, judgeResults[i]);
        if (result != EUROVISION_STATE_ALREADY_EXIST) return result;        // invalid state ID in judge's results
    }

    // temporarily allocate memory for the judge's data
    JudgeData judge_data = createJudgeData(judgeName, judgeResults);
    if (!judge_data) return EUROVISION_OUT_OF_MEMORY;   // judge's data allocation failed

    // add the judge to Eurovision's Judges
    MapResult put_result = mapPut(eurovision->Judges, &judgeId, judge_data);

    // deallocate the temporary judge data
    freeJudgeDataElement(judge_data);

    if (put_result == MAP_OUT_OF_MEMORY) return EUROVISION_OUT_OF_MEMORY;   // copy in mapPut failed

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId) {
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;       // NULL pointer received

    EurovisionResult result = isIDValid(eurovision->Judges,  JUDGES_MAP, judgeId);
   if (result != EUROVISION_JUDGE_ALREADY_EXIST) return result;    // ID not valid or judge is not in Eurovision

    // Remove the judge from Eurovision's Judges
    mapRemove(eurovision->Judges, &judgeId);

    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver,
                                   int stateTaker) {
    // add one vote to stateTaker in the stateGiver's votes map
    return eurovisionChangeVote(eurovision->States, stateGiver, stateTaker, 1);
}


EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker) {
    // remove one vote from stateTaker in the stateGiver's votes map
    return eurovisionChangeVote(eurovision->States, stateGiver, stateTaker, -1);
}

List eurovisionRunContest(Eurovision eurovision, int audiencePercent) {
    if (!eurovision || audiencePercent > 100 || audiencePercent < 0) return NULL;   // invalid parameter received

    // if state map is empty return empty string List
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
                    points_list_iterator->vote_count += (100-audiencePercent)* getRanking(i);
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
        if (areFriendlyStates(stateId, favState1, stateId2, favState2)) {
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

