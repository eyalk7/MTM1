#ifndef JUDGES_H
#define JUDGES_H

#include <stdbool.h>

/**
 *  File containing all macros, enums, structs and functions
 *  related to the Judges map and the data element in said map.
 */

/********************** MACROS & ENUMS ***********************/
/** number of states the judge has to rank */
#define NUMBER_OF_RANKINGS 10

/********************** JUDGE MAP DEFINITIONS ***********************/
typedef struct JudgeData_t *JudgeData;

typedef void* JudgeDataElement;
typedef void* JudgeKeyElement;

/********************** JUDGE MAP FUNCTIONS ***********************/
/***
 * Function for copying a key element in Judges map.
 * @param key - The judge's ID
 * @return A copy of the judge's ID
 */
JudgeKeyElement copyJudgeKeyElement(JudgeKeyElement key);

/***
 * Copy function for the data element in Judges map.
 * @param data - JudgeData struct with judge's name and results
 * @return A copy of the JudgeData struct
 */
JudgeDataElement copyJudgeDataElement(JudgeDataElement data);

/***
 * Function for deallocating a key element in Judges map.
 * @param key  - The judge's ID
 */
void freeJudgeKeyElement(JudgeKeyElement key);

/***
 * Function for deallocating a data element in Judges map.
 * @param data - JudgeData struct with judge's name and results
 */
void freeJudgeDataElement(JudgeDataElement data);

/** compare between two judge's ids */
/***
 * Compare function for two keys in Judges map.
 * @param key1 - A judge's ID
 * @param key2 - A judge's ID
 * @return
 *   Positive integer if first key is bigger than the second
 *   Negative integer if first key is smaller than the second
 *   0 if keys are equal
 */
int compareJudgeKeyElements(JudgeKeyElement key1, JudgeKeyElement key2);

/********************** JUDGE DATA FUNCTIONS ***********************/
/***
 * Get the judge's results
 * @param data - JudgeData struct with judge's results
 * @return Return an int array representing the judge's results
 */
int *getJudgeResults(JudgeData data);

 /***
  * Creates a JudgeData struct with given paremeters.
  * @param name - Judge's name
  * @param results  - Judge's results
  * @return
  *   A JudgeData struct which values are set to copies of the given parameters
  */
JudgeData judgeDataCreate(const char *name, const int *results);

/***
 * Checks if a judge gave points to the given state
 * @param judge - JudgeData struct with judge's results
 * @param state_id - ID of the state to check
 * @return
 *   true if judge's results contains given state ID
 *   false otherwise
 */
bool judgeResultsContain(JudgeData judge, int state_id);


#endif //JUDGES_H
