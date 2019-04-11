#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
    // memory alocation for the struct and check
    Eurovision eurovision = malloc(sizeof(*eurovision));
    if (!eurovision) return NULL;

    // mapCreate for the states and judges with functions for the States and Judges maps
    eurovision->States = mapCreate(copyJudgeDataElement, copyJudgeKeyElement, freeJudgeDataElement, freeJudgeKeyElement, compareJudgeKeyElements);


    //check mapCreate return value
    //initialize the parameretes of eurovision with the maps
    // return pointer
}

void eurovisionDestroy(Eurovision eurovision) {
    //mapDestroy for the states votes map if exists
    //mapDestroy for the States and Judges maps
    //free eurovision
}

EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId,
                                    const char *stateName,
                                    const char *songName) {
    //outside function - isIDValid(Map map, int id)
    //outside function - checkValidName(char* name)

    //memory aloctation for the tmp_stateData and check
    //mapCreate for the tmp_votes and add votes functions
    //check mapCreate return value
    //initalize the tmp_stateData element with name(by pointer), song name (by pointer) and tmp_votes map (by pointer)
    //mapPut in the States map with stateId (send as pointer!) as key and tmp_stateData element as data
    //check mapPut return value
    //free alocation of tmp_stateDate element
    //mapDestroy for tmp_votes map
    //return success
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId) {
    //check NULL eurovision
    //outside function - isIDValid(Map map, int id)
    //iterate on the States map of eurovision with MAP_FOREACH:
    // in each State mapRemove from Votes map the given stateId
    //check return value
    //iterate on the Judges map of eurovision with MAP_FOREACH:
    // in each judge outside function - resultsContain(eurovision, judge id, state id
    // if true - call eurovisionRemoveJudge for this judgeId
    // check return value
    // mapRemove from States map the stateId
    //check return value
}

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults) {
    //outside functions:
    //outside function - isIDValid(Map map, int id)
    //check judges results- existing stateIds with mapContain
    //outside function - checkValidName(char* name)

    //check with mapContain in Judges map if judgeId already exist
    //memory aloctation for the tmp_judgeData and check
    //initalize the tmp_judgeDate element with name, and results (by value)
    //mapPut in the Judges map with judgeId (send as pointer!) as key and tmp_judgeData element as data
    //check mapPut return value
    //free alocation of tmp_judgeDate element
    //return success
}

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId) {
    //check if < 0
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