#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"
#include "eurovision.h"

struct eurovision_t {
    Map States; // keys = StateId, data = StateData
    Map Judges; // keys = JudgeId, data = JudgeData
};

main () {

}

Eurovision eurovisionCreate() {
    // memory alocation for the struct and check
    // mapCreate for the states and judges with functions for the States and Judges maps
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
    //outside functions - validState()
        //check valid state ID >= 0 and numbers
        //check with mapContain in States map if stateId already exist

    //outside function checkValidName()
        //check the state name and song name - only small letters and spaces

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

}

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults) {
    //outside functions:
        //check valid judge ID >= 0 and numbers
            //check judges results- existing stateIds with mapContain
        //outside function checkValidName():
            //check the judge name only small letters and spaces

    //check with mapContain in Judges map if judgeId already exist
    //memory aloctation for the tmp_judgeData and check
    //initalize the tmp_judgeDate element with name, and results (by value)
    //mapPut in the Judges map with judgeId (send as pointer!) as key and tmp_judgeData element as data
    //check mapPut return value
    //free alocation of tmp_judgeDate element
    //return success
}

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId) {

}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver,
                                   int stateTaker) {

}

EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker) {

}

List eurovisionRunContest(Eurovision eurovision, int audiencePercent) {

}

List eurovisionRunAudienceFavorite(Eurovision eurovision) {

}

List eurovisionRunGetFriendlyStates(Eurovision eurovision) {

}
