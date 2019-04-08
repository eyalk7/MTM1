#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"
#include "eurovision.h"

typedef struct JudgeData_t {
    char *name;
    unsigned int results[10];
} *JudgeData;

typedef struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = stateId, data = no. of votes this state received
} *StateData;

struct eurovision_t {
    Map States;
    Map Judges;
};

main () {

}

Eurovision eurovisionCreate() {

}

void eurovisionDestroy(Eurovision eurovision) {

}

EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId,
                                    const char *stateName,
                                    const char *songName) {

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
