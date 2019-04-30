#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "functions.h"

struct statePoints_t {
    int id;
    double points;
};

/*********************** EUROVISION HELP FUNCTIONS *******************************/
bool isLowerCase(char c) {
    return ('a' <= c && c <= 'z');
}

bool isValidName(const  char* name) {
    for (int i = 0; i < strlen(name); i++) {
        if (!isLowerCase(name[i]) && name[i] != ' ') return false;
    }
    return true;
}

void* copyInt(void* integer) {
    int* copy = malloc(sizeof(*copy));
    *copy = *(int*)integer;
    return copy;
}
void freeInt(void* integer) {
    free(integer);
}
int compareInts(MapKeyElement integer1, MapKeyElement integer2) {
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

EurovisionResult eurovisionChangeVote(Map states, int state_giver,
                                      int state_taker, int difference) {
    /// PARAMETER CHECKS ///
    if (states == NULL) return EUROVISION_NULL_ARGUMENT;
    if (state_giver < 0 || state_taker < 0) return EUROVISION_INVALID_ID;       // ID not valid
    if (!mapContains(states, &state_giver) || !mapContains(states, &state_taker)) {
        return EUROVISION_STATE_NOT_EXIST;          // one of the given states doesn't exist
    }
    if (state_giver == state_taker) return EUROVISION_SAME_STATE;       // same states given
    /// PARAMETER CHECKS ///

    // get current number of votes for state_taker in state_giver's votes map
    StateData giver_data = mapGet(states, &state_giver);
    assert(giver_data != NULL);
    int *current_votes_num = mapGet(getStateVotes(giver_data), &state_taker);

    if (current_votes_num == NULL) { // no votes
        if (difference > 0) {
            // add state_taker to state's vote map along with the number of votes
            MapResult put_result = mapPut(getStateVotes(giver_data), &state_taker, &difference);
            if (put_result == MAP_OUT_OF_MEMORY) return EUROVISION_OUT_OF_MEMORY;
        }
        // if difference <= 0 nothing is done (no votes added or removed)
    } else {
        // if there are votes for this state already:
        (*current_votes_num) += difference;             // update the number of votes
        if ((*current_votes_num) <= 0) {                // if, after the update, number of votes <= 0
            mapRemove(getStateVotes(giver_data), &state_taker); // remove the votes from the votes map
        }
    }

    return EUROVISION_SUCCESS;
}

/***************************** POINTS LIST FUNCTIONS *****************************/
ListElement copyStatePoints(ListElement element) {
    if (element == NULL) return NULL;

    StatePoints copy = malloc(sizeof(*copy));
    if (copy == NULL) return NULL;

    StatePoints data = element;
    copy->id = data->id;
    copy->points = data->points;

    return copy;
}
void freeStatePoints(ListElement element) {
    free(element);
}
int compareStatePoints(ListElement element1, ListElement element2) {
    // if state in element1 ranks higher than state in element2
    // returns negative
    // (therefore state1 < state2 in final list)
    // else return positive (NEVER returns 0)

    assert(element1 != NULL && element2 != NULL);

    StatePoints data1 = element1;
    StatePoints data2 = element2;

    if (data1->points == data2->points) {
        return data1->id - data2->id;       // IDs are always different
    }
    return (data2->points > data1->points) ? 1 : -1;
}

List pointListCreate(Map states) {
    assert(states != NULL);

    List list = listCreate(copyStatePoints, freeStatePoints);
    if (!list) return NULL;

    MAP_FOREACH(int*, id, states) {
        StatePoints data = malloc(sizeof(*data));
        if (!data) {
            listDestroy(list);
            return NULL;
        }

        data->id = *id;
        data->points = 0.0; // initialize to zero

        ListResult result = listInsertFirst(list, data);
        freeStatePoints(data);

        if (result != LIST_SUCCESS) {
            listDestroy(list);
            return NULL;
        }
    }

    return list;
}

List convertVotesToList(Map votes) {
    assert(votes != NULL);

    // Convert to points list first (using votes instead of points)
    List list = pointListCreate(votes);     // IDs are set here
    LIST_FOREACH(StatePoints, point_data, list) {
        int* vote_count = mapGet(votes, &(point_data->id));
        if (!vote_count) {
            listDestroy(list);
            return NULL;
        }
        point_data->points = *vote_count;      // number of votes is set here
    }

    // Use list sort to sort based on vote counts
    ListResult result = listSort(list, compareStatePoints);
    if (result != LIST_SUCCESS) {
        listDestroy(list);
        return NULL;
    }

    return list;
}

int *getStateResults(List votes_list) {
    assert(votes_list != NULL);

    int votes_size = listGetSize(votes_list);
    int len = (votes_size < NUMBER_OF_RANKINGS ? votes_size : NUMBER_OF_RANKINGS);

    int *state_results = malloc(sizeof(int) * NUMBER_OF_RANKINGS);
    if (!state_results) return NULL;        // allocation failed

    StatePoints ptr = listGetFirst(votes_list);
    for (int i = 0; i < len; i++) {
        state_results[i] = ptr->id;
        ptr = listGetNext(votes_list);
    }
    // if state voted for less than 10, fill the rest NO_STATE
    for (int i = len; i < NUMBER_OF_RANKINGS; i++) {
        state_results[i] = NO_STATE;
    }

    return state_results;
}

List convertToStringList(List final_results, Map states) {
    assert(final_results != NULL && states != NULL);

    // Create a list of names of the sates that are in given list
    List state_names = listCreate(copyString, freeString);
    LIST_FOREACH(StatePoints, point_data, final_results) {
        int stateId = point_data->id;
        StateData data = mapGet(states, &stateId);
        if (!data) {
            listDestroy(state_names);
            return NULL;
        }

        // Keep the same order they have in given list
        ListResult result = listInsertLast(state_names, getStateName(data));
        if (result != LIST_SUCCESS) {
            listDestroy(state_names);
            return NULL;
        }
    }

    return state_names;
}

/***************************** CONTEST FUNCTIONS ********************************/
Ranking getRanking(int place) {
    static const Ranking ranking[NUMBER_OF_RANKINGS] = {
            FIRST_PLACE, SECOND_PLACE, THIRD_PLACE, FOURTH_PLACE,
            FIFTH_PLACE, SIXTH_PLACE, SEVENTH_PLACE, EIGHT_PLACE,
            NINTH_PLACE, TENTH_PLACE
    };

    return ranking[place];
}

List getAudiencePoints(Map states) {
    // create an audience points list with all states
    List audience_points = pointListCreate(states);
    if (!audience_points) return NULL;

    // distribute each states votes accordingly in audience_points
    MAP_FOREACH(int*, state_giver, states) {
        StateData giver_data = mapGet(states, state_giver);     // get the state's data
        assert(giver_data != NULL);

        // get the state's sorted vote list
        List votes_list = convertVotesToList(getStateVotes(giver_data));
        if (!votes_list) {
            listDestroy(audience_points);
            return NULL;
        }

        int *state_results = getStateResults(votes_list);

        listDestroy(votes_list);        // deallocate votes_list

        if (!state_results) {
            listDestroy(audience_points);
            return NULL;
        }

        // distribute points to the states in state_results
        // (according to their order in the array)
        distributePoints(audience_points, state_results);

        free(state_results);   // deallocate the state_results array
    }

    return audience_points;
}

List getJudgesPoints(Map judges, Map states) {
    List judge_points = pointListCreate(states);
    if (!judges) return NULL;

    // for each judge
    MAP_FOREACH(int *, judge_id, judges) {
        // get the judge's data
        JudgeData judge_data = mapGet(judges, judge_id);
        assert(judge_data != NULL);

        // distribute points to the states in judge_results
        // (according to their order in the array)
        distributePoints(judge_points, getJudgeResults(judge_data));
    }

    return judge_points;
}

void distributePoints(List points_list, const int *results) {
    // for each state in results
    for (int i=0; i < NUMBER_OF_RANKINGS; i++) {
        int state_id = results[i];          // ID of the state to give points to
        int points = getRanking(i);         // get points to give

        // iterate on points list & find the state to give points to
        LIST_FOREACH(StatePoints, point_data, points_list){
            if (point_data->id == state_id) {
                point_data->points += points;      // update the state's points
            }
        }
    }
}

void calculateFinalPoints(List audience_points, List judge_points,
                          int num_of_states, int num_of_judges,
                          int audience_percent) {
    int judge_percent = 100 - audience_percent;

    // Add judge points to audience points
    StatePoints judge_points_ptr = listGetFirst(judge_points);
    LIST_FOREACH(StatePoints, point_data, audience_points) {
        assert(judge_points_ptr != NULL);

        // Divide each state's audience points by the number of states
        point_data->points /= num_of_states;
        // Multiply each state's audience points by audience percentage
        point_data->points *= audience_percent;
        // Divide each state's judge points by the number of judges
        judge_points_ptr->points /= num_of_judges;
        // Multiply each state's judge points by audience percentage
        judge_points_ptr->points *= judge_percent;

        // Add judge points to audience points
        point_data->points += judge_points_ptr->points;
        // increment judge_points pointer
        judge_points_ptr = listGetNext(judge_points);

    }
//
//    listSort(audience_points, compareStatePoints);
//
//    int place = 0;
//    LIST_FOREACH(StatePoints, points, audience_points) {
//        printf("%d) state: %d - points: %f\n", place, points->id, points->points);
//        place++;
//    }
}
