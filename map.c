#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"

#define NO_COMPARES -1
#define END_OF_LOOP -1

typedef struct MapNode_t {
    MapKeyElement key;
    MapDataElement data;
    struct MapNode_t *next;
} *MapNode;

struct Map_t {
    MapNode head;
    MapNode iterator;
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
};

//assistance functions
MapNode nodeCreate ();
void nodeDestroy (MapNode node);
int mapIterateAndCompare (Map map, MapKeyElement keyElement, MapNode tmp_iterator);

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements) {
    // NULL check for parameters
    if (!copyDataElement || !copyKeyElement || !freeDataElement || !freeKeyElement || !compareKeyElements)
        return NULL;

    Map map = malloc(sizeof(*map));    // allocate memory for new map
    if (!map)
        return NULL;    // if allocate failed, return NULL

    // initialize empty map
    map->head = NULL;
    map->iterator = NULL;

    // set function pointers as given in parameters
    map->copyDataElement = copyDataElement;
    map->copyKeyElement = copyKeyElement;
    map->freeDataElement = freeDataElement;
    map->freeKeyElement = freeKeyElement;
    map->compareKeyElements = compareKeyElements;

    return map; // return map pointer
}

void mapDestroy(Map map) {
    // call mapClear function to deallocate all the nodes in the map
    MapResult result = mapClear(map);

    if (result == MAP_SUCCESS)  // if nodes deallocation was successful
        free(map);  // deallocate the map

    // else, if a null pointer was sent we do nothing
}

Map mapCopy(Map map) {
    if (!map)
        return NULL;    // return NULL if null pointer was sent

    // create map copy
    Map copy = mapCreate(map->copyDataElement, map->copyKeyElement, map->freeDataElement, map->freeKeyElement, map->compareKeyElements);
    if (!copy)
        return NULL;    // return null if memory allocation failed

    //iterate on the map and copy all pairs
    for (MapNode ptr = map->head; ptr ; ptr = ptr->next) {

        // add a copy of each node to the map copy
        // (uses the user's function to copy the key and data)
        MapResult result = mapPut(copy, ptr->key, ptr->data);

        /* assert(result != MAP_NULL_ARGUMENT) */

        if (result == MAP_OUT_OF_MEMORY)
            return NULL;    // if allocate failed return NULL
    }

    //in the last node put NULL in "next"
    // mapPut sets the "next"s correctly so the last node gets NULL

    return copy;    // return the copy pointer
}

int mapGetSize(Map map) {
    if (!map)
        return -1;  // return -1 if NULL

    //iterate on the map and count until NULL
    int count = 0;
    for (MapNode ptr = map->head; ptr ; ptr = ptr->next) {
        count++;
    }

    return count;    // return count
}

bool mapContains(Map map, MapKeyElement element) {
    if (!map || !element)
        return false;   // NULL pointer was sent

    //iterate on the map and compare each key with the element using the user's function
    MapNode ptr;
    for (ptr = map->head; ptr && map->compareKeyElements(element, ptr->key) > 0; ptr = ptr->next) {
    }
    if (!ptr)
        return false;   // end of map, element not found

    if (map->compareKeyElements(element, ptr->key) == 0)
        return true;    // element found


    return false;   // element not found
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    //if NULL return MAP_NULL_ARGUMENT
    if (map == NULL || keyElement == NULL || dataElement == NULL) return MAP_NULL_ARGUMENT;
    // copy data with the user's function
    MapDataElement new_data = map->copyDataElement(dataElement);
    //iterate on the map and compare with the user's function the head parameter is keyelemt and second paramenter is the next node's key
    MapNode tmp_iterator = map->head;
    int compare_result = mapIterateAndCompare(map, keyElement, tmp_iterator);

    //if 0 free the current node data with user's function,
    //update the data in the node
    if (compare_result == 0) {
        map->freeDataElement(tmp_iterator->data);
        tmp_iterator->data = new_data;
    } else {
        //if negative or end of loop enter new node:
        //allocate new node and check allocation if fail return MAP_OUT_OF_MEMORY
        MapNode new_node = nodeCreate();
        if (new_node == NULL) return MAP_OUT_OF_MEMORY;
        //  copy key with user's function
        MapKeyElement new_key = map->copyKeyElement(keyElement);
        // insert the data and key to the node
        new_node->data = new_data;
        new_node->key = new_key;
        //the new node next  = "pointer"'s next
        if (tmp_iterator != NULL) {
            new_node->next = tmp_iterator->next;
            tmp_iterator->next = new_node;
        } else {
            //"pointer"'s next = new node address
            new_node->next = NULL;
            map->head = new_node;
            // printf("map->head: %d", map->head);
        }
    }
    //return MAP_SUCCESS
    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
    //return NULL if NULL
    if (map == NULL || keyElement == NULL) return NULL;
    //iterate on the map and compare the key for each with the user's function
    MapNode tmp_iterator = map->head;
    while (tmp_iterator != NULL) {
        //return the mapDataElement if found
        if (map->compareKeyElements(keyElement, tmp_iterator->key) == 0) {
            return tmp_iterator->data;
        }
    }
    //else return NULL
    return NULL;
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    //if NULL return MAP_NULL_ARGUMENT
    if (map == NULL || keyElement == NULL) return MAP_NULL_ARGUMENT;
    //iterate on the map and compare with the user's function the head parameter is keyelemt and second paramenter is the next node's key
    MapNode tmp_iterator = map->head;
    int compare_result = mapIterateAndCompare(map, keyElement, tmp_iterator);
    //if 0:
    if (compare_result == 0) {
        //save the next node pointer
        MapNode next_node = tmp_iterator->next;
        //take the next node's next and put it in the current node next
        tmp_iterator->next = next_node->next;
        // free the saved pointer's key and data with the user's function
        map->freeDataElement(tmp_iterator->data);
        map->freeKeyElement(tmp_iterator->key);
        //free the saved node
        nodeDestroy(tmp_iterator);
        //return MAP_SUCCESS
        return MAP_SUCCESS;
    }
    // if negative or end of loop return MAP_ITEM_DOES_NOT_EXIST
    assert(compare_result < 0);
    return MAP_ITEM_DOES_NOT_EXIST;
}

MapKeyElement mapGetFirst(Map map) {
    //if map is NULL return NULL
    if (map == NULL) return NULL;
    // set internal iterator to head
    map->iterator = map->head;
    // return map head
    return map->head;
}

MapKeyElement mapGetNext(Map map) {
    //if map is NULL return NULL
    if (map == NULL) return NULL;
    // set internal iterator to next node
    map->iterator = map->iterator->next;
    //return next node
    return map->iterator;
}

MapResult mapClear(Map map) {
    //return MAP_NULL_ARGUMENT - if a NULL pointer was sent.
    if (map == NULL) return MAP_NULL_ARGUMENT;
    //iterate on the map and free all pairs
    MapNode tmp_iterator = map->head;
    while (tmp_iterator != NULL) {
        //on each pair free the key and data with the user's function
        map->freeKeyElement(tmp_iterator->key);
        map->freeDataElement(tmp_iterator->data);
        //free the node and go to next until the next is NULL
        MapNode node_to_destroy = tmp_iterator;
        tmp_iterator = tmp_iterator->next;
        nodeDestroy(node_to_destroy);
    }
    //return MAP_SUCCESS
    return MAP_SUCCESS;
}

MapNode nodeCreate () {
    MapNode new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) return NULL;
    return new_node;
}

void nodeDestroy (MapNode node) {
    free(node);
}

int mapIterateAndCompare (Map map, MapKeyElement keyElement, MapNode tmp_iterator) {
    int compare_result = NO_COMPARES;
    do {
        if (tmp_iterator == NULL) {
            compare_result = END_OF_LOOP;
            break;
        }

        compare_result = map->compareKeyElements(keyElement, tmp_iterator->key);
        tmp_iterator = tmp_iterator->next;
        //while the user's function return positive numbers and not in the end of the map
    } while (compare_result > 0);
    return compare_result;
}
