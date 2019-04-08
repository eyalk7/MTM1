#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct JudgeData_t {
    char *name;
    unsigned int results[10];
} *JudgeData;

// copyJudgeDataElement - Function pointer to be used for copying data elements into	the map or when copying the map.
    //memory allocation for the JudgeData and check
    //memory allocation for name string and check
    //initialize results
    //return address
// copyJudgeKeyElement - Function pointer to be used for copying key elements into	the map or when copying the map.
    //outside function
        //allocation for int
        //intialize the int
        //return address
// freeJudgeDataElement - Function pointer to be used for removing data elements from the map
    //free strings allocation
    //free judgeData allocatino
// freeJudgeKeyElement - Function pointer to be used for removing key elements from the map
    //free allocation
// compareJudgeKeyElements - Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
    //outside function
        //compare the ints
        //return 0 if equal
        //return bigger than 0 if first is bigger
        //else the small than 0