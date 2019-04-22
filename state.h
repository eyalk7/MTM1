#ifndef STATES_H
#define STATES_H

/********************** MACROS & ENUMS ***********************/
/** macros for strings states list */
#define SPACE " "
#define DASH "-"
#define NUM_OF_EXTRA_CHARS 3

/** macro for states that dont have a favorite state (no votes) */
#define NO_FAVORITE_STATE (-1)

/********************** STATE MAP DEFINITIONS ***********************/
typedef struct StateData_t {
    char *name;
    char *song_name;
    Map votes; // key = State's ID, data = no. of votes this state *gives*
} *StateData;

typedef void* StateKeyElement;
typedef void* StateDataElement;

typedef void* VoteKeyElement;
typedef void* VoteDataElement;

/********************** STATE MAP FUNCTIONS ***********************/
/** returns a copy of the state's id */
StateKeyElement copyStateKeyElement(StateKeyElement key);

/** returns a copy of the state's data (name, song name and votes) */
StateDataElement copyStateDataElement(StateDataElement data);

/** frees the state's id */
void freeStateKeyElement(StateKeyElement key);

/** frees the state's data (name, song name and votes) */
void freeStateDataElement(StateDataElement data);

/** compare between two state's ids */
int compareStateKeyElements(StateKeyElement key1, StateKeyElement key2);

/********************** VOTE MAP FUNCTIONS ***********************/

/** returns a copy of the id of the state that receives the votes */
VoteKeyElement copyVoteKeyElement(VoteKeyElement key);

/** returns a copy of the amount of votes being given */
VoteDataElement copyVoteDataElement(VoteDataElement data);

/** frees the id of the state that receives the votes */
void freeVoteKeyElement(VoteKeyElement key);

/** frees the amount of votes being given */
void freeVoteDataElement(VoteDataElement data);

/** compares between the ids of two states that receive the votes */
int compareVoteKeyElements(VoteKeyElement key1, VoteKeyElement key2);

/********************** FRIENDLY STATE FUNCTIONS ***********************/
/** string compare function for string list lexicographical sort */
int stringCompare(void* str1, void* str2);

/** receives the votes a state gives
 *  returns the id of the state which got the most votes from that state */
int getFavoriteState(Map votes);

/** Check if states are friendly by the assigment definition */
bool checkFriendlyStates(const int *stateId1,const int *favState1,const int *stateId2,const int *favState2);

/** returns a map that shows each state's "favorite state"
 * (key = state's id, value = favorite state's id) */
Map getStateFavorites(Map states);

/** returns a string of the state names in the format defined in the assigment:
 * "{first state's name} - {second state's name}"
 * (the state names are ordered lexicographically) */
char *getStatePair(StateData state1, StateData state2);

#endif //STATES_H
