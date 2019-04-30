#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"

/********************** ENUMS & STRUCTS ***********************/
/** enums for mapIterateAndCompare return result */
typedef enum CompareResult_t {
    EQUAL,
    EQUAL_TO_FIRST,
    START_OF_MAP,
    END_OF_MAP,
    MIDDLE_OF_MAP
} CompareResult;

/** node struct for the map */
typedef struct MapNode_t {
    MapKeyElement key;
    MapDataElement data;
    struct MapNode_t *next;
} *MapNode;

/** map struct */
struct Map_t {
    MapNode head;
    MapNode iterator;
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
};

/*************** HELP FUNCTIONS DECLARATIONS ****************/
/** create a new node to add to the map (used in mapPut)
 * The new node's data is the given MapDataElement and it's key is a copy of the given MapKeyElement.
 * The one who uses this function is in charge of freeing new_data when there's an error.
 * */
static MapNode nodeCreate (Map map, MapKeyElement key, MapDataElement new_data);

/** Deallocates a node */
static void nodeDestroy (MapNode node);

/** Iterate on the map & compare each keyElement to given keyElement
 *  Given node pointer is used as the iterator and is, therefore, changed
 *  If the given element is in the map:
 *      - given iterator will point to the node before the one the given element is in (unless it's the first node)
 *      - the function will return EQUAL or EQUAL_TO_FIRST appropriately
 *  If the given element is not on the map:
 *      - given iterator will point to the the place before the one the given element should be in
 *      - the function will return START_OF_MAP, MIDDLE_OF_MAP or END_OF_MAP depending or where the node should go
 *        (MIDDLE_OF_MAP refers to any place that's not the beginning or the end)
 *
 * */
static CompareResult mapIterateAndCompare (Map map, MapKeyElement key, MapNode *iterator);

/** Used in mapPut for updating an existing node
 *  Updates the node which the map iterator is on with the new data that is received.
 *  Makes sure to deallocate the data that was in the node beforehand.
 */
static void mapUpdateExistingNode(Map map, MapDataElement new_data);

/** Used in mapPut for inserting a new node
 *  Inserts a new node based on the map iterator's position and the given compare result
 *  (START_OF_MAP / MIDDLE_OF_MAP / END_OF_MAP)
 */
static void mapInsertNewNode(Map map, MapNode new_node, CompareResult position);

/********************** MAP FUNCTIONS ***********************/

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements) {
    // NULL check for parameters
    if (!copyDataElement || !copyKeyElement || !freeDataElement || !freeKeyElement || !compareKeyElements)
        return NULL;

    Map map = malloc(sizeof(*map));    // allocate memory for new map
    if (!map) return NULL;    // allocation failed

    // initialize empty map
    map->head = NULL;
    map->iterator = NULL;

    // initialize function pointers as given in parameters
    map->copyDataElement = copyDataElement;
    map->copyKeyElement = copyKeyElement;
    map->freeDataElement = freeDataElement;
    map->freeKeyElement = freeKeyElement;
    map->compareKeyElements = compareKeyElements;

    return map; // return map pointer
}

void mapDestroy(Map map) {
    MapResult result = mapClear(map);   // deallocate all the nodes in the map

    if (result == MAP_SUCCESS)  // if nodes deallocation was successful
        free(map);              // deallocate the map

    // if a null pointer was sent do nothing
}

Map mapCopy(Map map) {
    if (!map) return NULL;  // NULL pointer was sent

    // create map copy
    Map copy = mapCreate(map->copyDataElement,
                         map->copyKeyElement,
                         map->freeDataElement,
                         map->freeKeyElement,
                         map->compareKeyElements);

    if (!copy) return NULL; // allocation failed

    //iterate on the map and copy all pairs
    for (MapNode ptr = map->head; ptr ; ptr = ptr->next) {
        MapResult result = mapPut(copy, ptr->key, ptr->data);   // add a copy of each node to the map copy

        if (result == MAP_OUT_OF_MEMORY) {
            mapDestroy(copy);   // free all nodes created so far
            return NULL;        // allocation failed
        }
    }

    return copy;    // return the copy pointer
}

int mapGetSize(Map map) {
    if (!map) return -1;  // NULL pointer received

    //iterate on the map and increment counter
    int count = 0;
    for (MapNode ptr = map->head; ptr ; ptr = ptr->next) {
        count++;
    }

    return count;
}

bool mapContains(Map map, MapKeyElement element) {
    if (!map || !element) return false; // NULL pointer was sent

    //iterate on the map and check if the given key element is contained
    MapNode ptr = map->head;
    CompareResult compare_result = mapIterateAndCompare(map, element, &ptr);

    if (compare_result == EQUAL || compare_result == EQUAL_TO_FIRST) return true;   // element found

    return false;   // element not found
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    // NULL check for parameters
    if (!map || !keyElement || !dataElement) return MAP_NULL_ARGUMENT;

    // copy data
    MapDataElement new_data = map->copyDataElement(dataElement);
    if (!new_data) return MAP_OUT_OF_MEMORY;

    // iterate on the map and compare, map iterator is changed
    mapGetFirst(map);
    CompareResult compare_result = mapIterateAndCompare(map, keyElement, &(map->iterator));

    // map iterator is used to help update an existing node or insert a new one
    if (compare_result == EQUAL || compare_result == EQUAL_TO_FIRST) {
        if (compare_result == EQUAL)            // if the node exists, but it's not the first in the map
            mapGetNext(map);                    // the iterator is pointing to one before, so we increment it
        mapUpdateExistingNode(map, new_data);   // update the data of the node the iterator is on
    } else {
        MapNode new_node = nodeCreate(map, keyElement, new_data);   // if the node doesn't exist, create a new one
        if (!new_node) {
            map->freeDataElement(new_data);
            return MAP_OUT_OF_MEMORY;
        }
        mapInsertNewNode(map, new_node, compare_result);   // insert a new node after the one the iterator is on
    }

    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
    if (!map || !keyElement) return NULL;   // NULL parameter received

    //iterate on the map and compare each node's key with the user's function
    MapNode ptr = map->head;
    CompareResult compare_result = mapIterateAndCompare(map, keyElement, &ptr);

    if (compare_result == EQUAL) return ptr->next->data;    // if found return the data
    if (compare_result == EQUAL_TO_FIRST) return ptr->data; // different actions if it is equal to the first

    return NULL;  // key element not found
}

MapResult mapRemove(Map map, MapKeyElement keyElement) {
    if (!map || !keyElement) return MAP_NULL_ARGUMENT;  // NULL parameter received

    // iterate on the map and compare
    mapGetFirst(map);
    CompareResult compare_result = mapIterateAndCompare(map, keyElement, &(map->iterator));

    if (compare_result != EQUAL && compare_result != EQUAL_TO_FIRST) {
        return MAP_ITEM_DOES_NOT_EXIST;         // map doesn't contain given key
    }

    MapNode prev_node = map->iterator;          // save the previous node
    if (compare_result == EQUAL) {
        mapGetNext(map);                        // if EQUAL_TO_FIRST, prev_node = node_to_remove = map->head
    }
    MapNode node_to_remove = map->iterator;     // get the node you have to remove
    MapNode next_node = node_to_remove->next;   // save the next node

    // deallocate the node we want to remove (with it's data and key)
    map->freeDataElement(node_to_remove->data);
    map->freeKeyElement(node_to_remove->key);
    nodeDestroy(node_to_remove);

    if (compare_result == EQUAL_TO_FIRST) {
        map->head = next_node;          // if it's equal to the first node, update the map's head
    } else {
        prev_node->next = next_node;    // otherwise, connect the previous and next node
    }

    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map) {
    if (!map || map->head == NULL) return NULL; // map is empty or NULL pointer received

    map->iterator = map->head;  // set iterator to first node

    return map->head->key;      // return the key of the first node
}

MapKeyElement mapGetNext(Map map) {
    if (!map || map->head == NULL) return NULL; // map is empty or NULL pointer received

    map->iterator = map->iterator->next;        // increment iterator

    if (map->iterator == NULL) return NULL;     // reached the end of the map

    return map->iterator->key;                  // return the current key
}

MapResult mapClear(Map map) {
    if (!map) return MAP_NULL_ARGUMENT;   // NULL pointer was sent.

    // iterate on the map and free all key-data pairs
    MapNode ptr = map->head;
    while (ptr != NULL) {
        // free the key and data with the user's function
        map->freeKeyElement(ptr->key);
        map->freeDataElement(ptr->data);

        MapNode node_to_destroy = ptr;
        ptr = ptr->next;                // increment the pointer
        nodeDestroy(node_to_destroy);     // free the current node
    }

    map->head = NULL;   // set map as empty

    return MAP_SUCCESS;
}

/****************** HELP FUNCTIONS IMPLEMENTATIONS *******************/
static MapNode nodeCreate (Map map, MapKeyElement key, MapDataElement new_data) {
    MapNode node = malloc(sizeof(*node));
    if (!node) return NULL;

    MapKeyElement new_key = map->copyKeyElement(key);
    if (!new_key) {
        nodeDestroy(node);
        return NULL;
    }

    node->data = new_data;
    node->key = new_key;
    node->next = NULL;

    return node;
}

static void nodeDestroy (MapNode node) {
    free(node);
}

static CompareResult mapIterateAndCompare (Map map, MapKeyElement key, MapNode *iterator) {
    MapNode ptr = *iterator;

    // if empty map or smallest key return START_OF_MAP
    if (ptr == NULL || map->compareKeyElements(key, ptr->key) < 0) {
        return START_OF_MAP;
    }

    // if equal to the first
    if (map->compareKeyElements(key, ptr->key) == EQUAL) {
        return EQUAL_TO_FIRST;
    }

    // iterate and compare
    while (ptr->next != NULL && map->compareKeyElements(key, ptr->next->key) > 0) {
        (*iterator) = ptr->next;
        ptr = *iterator;
    }

    // reached end of map
    if (ptr->next == NULL) return END_OF_MAP;

    // equality
    if (map->compareKeyElements(key, ptr->next->key) == EQUAL) return EQUAL;

    return MIDDLE_OF_MAP;
}

static void mapUpdateExistingNode(Map map, MapDataElement new_data) {
    MapNode ptr = map->iterator;        // get the node
    map->freeDataElement(ptr->data);    // free existing data
    ptr->data = new_data;               // set new data
}

static void mapInsertNewNode(Map map, MapNode new_node, CompareResult position) {
    MapNode ptr = map->iterator;
    switch (position) {
        case START_OF_MAP:              // if the given key is the smallest key or map is empty
            new_node->next = map->head;
            map->head = new_node;       // insert the new node to the head
            break;
        case END_OF_MAP:    // if the given key is the largest key, insert in the end of map
            ptr->next = new_node;
            // new_node->next = NULL was done in the initialization of new_node
            break;
        default:            // case MIDDLE_OF_MAP: insert the new node where it should be
            new_node->next = ptr->next;
            ptr->next = new_node;
            break;
    }
}
