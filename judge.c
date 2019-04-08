#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct JudgeData_t {
    char *name;
    unsigned int results[10];
} *JudgeData;

// copyJudgeDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
// copyJudgeKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
// freeJudgeDataElement - Function pointer to be used for removing data elements from the map
// freeJudgeKeyElement - Function pointer to be used for removing key elements from the map
// compareJudgeKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
