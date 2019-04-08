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
    //check valid state ID >= 0 and numbers
    //check the state name and song name - only small letters and spaces
    //check with mapContain in States map if stateId already exist
    //memory aloctation for the tmp_stateData and check
    //mapCreate for the tmp_votes and add votes functions
    //check mapCreate return value
    //initalize the tmp_stateDate element with id, nams, song name and tmp_votes map
    //mapPut in the States map with stateId (send as pointer!) as key and tmp_stateData element as data
    //check mapPut return value
    //free alocation of tmp_stateDate element
    //mapDestroy for tmp_votes map
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId) {

}

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults) {

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
