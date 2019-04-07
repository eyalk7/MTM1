#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"

enum {EQUAL, EQUAL_TO_FIRST, START_OF_MAP, END_OF_MAP, MIDDLE_OF_MAP};

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
    // NULL check for parameters
    if (map == NULL || keyElement == NULL || dataElement == NULL) return MAP_NULL_ARGUMENT;
    // copy data
    MapDataElement new_data = map->copyDataElement(dataElement);

    // iterate on the map and compare
    MapNode tmp_iterator = map->head;
    int compare_result = mapIterateAndCompare(map, keyElement, tmp_iterator);

    // if key exist, update the key's data
    if (compare_result == EQUAL) {
        map->freeDataElement(tmp_iterator->next->data);
        tmp_iterator->next->data = new_data;
        return MAP_SUCCESS;
    } else if (compare_result == EQUAL_TO_FIRST) {
        map->freeDataElement(tmp_iterator->data);
        tmp_iterator->data = new_data;
        return MAP_SUCCESS;
    }

    // if key dosn't exist, create new node
    MapNode new_node = nodeCreate();
    if (new_node == NULL) return MAP_OUT_OF_MEMORY;
    MapKeyElement new_key = map->copyKeyElement(keyElement);
    new_node->data = new_data;
    new_node->key = new_key;
    new_node->next = NULL;

    // if the given key is the smallest key insert the new node to the head
    if (compare_result == START_OF_MAP) {
        if (map->head != NULL) {
            new_node->next = map->head;
        }
        map->head = new_node;
        return MAP_SUCCESS;
    }

    // if the given key is the largest key, insert in the end of map
    if (compare_result == END_OF_MAP) {
        tmp_iterator->next = new_node;
        return MAP_SUCCESS;
    }

    // else insert the new node to its place
        new_node->next = tmp_iterator->next;
        tmp_iterator->next = new_node;
        return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
    //return NULL if NULL
    if (map == NULL || keyElement == NULL) return NULL;
    //iterate on the map and compare the key for each with the user's function

    for (MapNode tmp_iterator = map->head; tmp_iterator != NULL; tmp_iterator=tmp_iterator->next) {
        //return the mapDataElement if found
        if (map->compareKeyElements(keyElement, tmp_iterator->key) == 0) {
            return tmp_iterator->data;
        }
    }
    //else return NULL
    return NULL;
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    // NULL check for parameters
    if (map == NULL || keyElement == NULL) return MAP_NULL_ARGUMENT;

    // iterate on the map and compare
    MapNode tmp_iterator = map->head;
    int compare_result = mapIterateAndCompare(map, keyElement, tmp_iterator);

    //  if didn't find matching key
    if (compare_result != EQUAL && compare_result != EQUAL_TO_FIRST) {
        return MAP_ITEM_DOES_NOT_EXIST;
    }

    // if key exist, delete the node
    if (compare_result == EQUAL) {
        MapNode next_node = tmp_iterator->next->next;
        map->freeDataElement(tmp_iterator->next->data);
        map->freeKeyElement(tmp_iterator->next->key);
        nodeDestroy(tmp_iterator->next);
        tmp_iterator->next = next_node;
    } else {
        // equal to the first
        MapNode next_node = tmp_iterator->next;
        map->freeDataElement(tmp_iterator->data);
        map->freeKeyElement(tmp_iterator->key);
        nodeDestroy(tmp_iterator);
        map->head = next_node;
    }

    return MAP_SUCCESS;
    }
}

MapKeyElement mapGetFirst(Map map) {
    //if map is NULL or empty return NULL
    if (map == NULL || map->head == NULL) return NULL;
    // set internal iterator to head
    map->iterator = map->head;
    // return map head
    return map->head->key;
}

MapKeyElement mapGetNext(Map map) {
    //if map is NULL or empty return NULL
    if (map == NULL || map->head == NULL) return NULL;
    // set internal iterator to next node
    map->iterator = map->iterator->next;
    //return next node
    return map->iterator->key;
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
    // if smallest key return START_OF_MAP
    if (tmp_iterator == NULL || map->compareKeyElements(keyElement, tmp_iterator->key) < 0) {
        return START_OF_MAP;
    }

    // if equal to the first
    if (map->compareKeyElements(keyElement, tmp_iterator->key) == EQUAL) {
        return EQUAL_TO_FIRST;
    }

    // iterate and compare
    while (tmp_iterator->next != NULL && map->compareKeyElements(keyElement, tmp_iterator->next->key) > 0) {
        tmp_iterator = tmp_iterator->next;
    }

    // equality
    if (map->compareKeyElements(keyElement, tmp_iterator->next->key) == EQUAL) {
        return EQUAL;
    }

    // reached end of map
    if (tmp_iterator->next == NULL) {
        return END_OF_MAP;
    }

    return MIDDLE_OF_MAP;
}
