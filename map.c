#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"
#define NO_COMPARES -1

typedef struct node_map {
    MapKeyElement key;
    MapDataElement data;
    struct node_map *next;
} *NodeMap;

typedef struct Map_t {
    NodeMap head;
    NodeMap iterator;
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
} *Map;

//assistance functions
NodeMap nodeCreate ();

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements) {
    // NULL check for parameters if NULL return NULL
    //allocate memory for new map
    //check successfull allocate, if fail return NuLL
    //insert function pointers to the map srtuct
    //put NULL in first_pair and itertor
    //return map
}

void mapDestroy(Map map) {
    //mapClear
    //free the map
}

Map mapCopy(Map map) {
    //if map is NULL return
    //interate on the map and copy all pairs:
    //aloocate memory for each new node
    //check allocate if NULL return NULL
    //link the nodes (need to elaborate)
    //on each pair copy the key and data with the user's function on put on the new node
    //go to next until the next is NULL
    //in the last node put NULL in "next"
    //return the new Map
}

int mapGetSize(Map map){
    //return -1 if NULL
    //iterate on the map and count until NULL
    //return count
}

bool mapContains(Map map, MapKeyElement element) {
    //return false if map or element NULL
    //iterate on the map and compare each key with the element with the user's function
    //return true if the user's function return 0
    //return falsr otherwise
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    //if NULL return MAP_NULL_ARGUMENT
    if (map == NULL || keyElement == NULL || dataElement == NULL) return NULL;
    // copy data with the user's function
    MapDataElement new_data = map->copyDataElement(dataElement);
    //iterate on the map and compare with the user's function the first parameter is keyelemt and second paramenter is the next node's key
    NodeMap tmp_iterator = map->first;
    int compare_result = NO_COMPARES;
    do {
        if (tmp_iterator == NULL) break;
        compare_result = map->compareKeyElements(keyElement, tmp_iterator->key);
        tmp_iterator = tmp_iterator->next;
        //while the user's function return positive numbers and not in the end of the map
    } while (compare_result > 0);

    //if 0 free the current node data with user's function,
    //update the data in the node
    if (compare_result == 0) {
        map->freeDataElement(tmp_iterator->data);
        tmp_iterator->data = new_data;
    }

    //if negative or end of loop enter new node:
    assert (compare_result < 0);
    //allocate new node and check allocation if fail return MAP_OUT_OF_MEMORY
    NodeMap new_node = nodeCreate();
    if (new_node == NULL) return MAP_OUT_OF_MEMORY;
    //  copy key with user's function
    keyElement new_key = map->copyKeyElement(keyElement);
    // insert the data and key to the node
    new_node->data = new_data;
    new_node->key = new_key;
    //the new node next  = "pointer"'s next
    new_node->next = tmp_iterator->next;
    //"pointer"'s next = new node address
    tmp_iterator->next = new_node;
    //return MAP_SUCCESS
    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
    //return NULL if NULL
    //iterate on the map and compare the key for each with the user's function
    //return the mapDataElement if found
    //else return NULL
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    //if NULL return MAP_NULL_ARGUMENT
    //iterate on the map and compare with the user's function the first parameter is keyelemt and second paramenter is the next node's key
    //while the user's function return positive number
    // if negative or end of loop return MAP_ITEM_DOES_NOT_EXIST
    //if 0:
    //save the next node pointer
    //take the next node's next and put it in the current node next
    // free the saved pointer's key and data with the user's function
    //free the saved node
    //return MAP_SUCCESS
}

MapKeyElement mapGetFirst(Map map) {
    //if map is NULL return NULL
    // ???
}

MapKeyElement mapGetNext(Map map) {
    //if map is NULL return NULL
    // ???
}

MapResult mapClear(Map map) {
    //return MAP_NULL_ARGUMENT - if a NULL pointer was sent.
    //iterate on the map and free all pairs
    //on each pair free the key and data with the user's function
    //free the node and go to next until the next is NULL
    //return MAP_SUCCESS
}

NodeMap nodeCreate () {
    NodeMap new_node = malloc(sizeof(*NodeMap));
    if (new_node == NULL) return NULL;
    return new_node;
}