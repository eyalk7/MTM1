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
/** create a new node to add to the map (used in mapPut) */
static MapNode nodeCreate (Map map, MapKeyElement key, MapDataElement new_data);

/** function for deallocating a node */
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
    Map copy = mapCreate(map->copyDataElement, map->copyKeyElement, map->freeDataElement, map->freeKeyElement, map->compareKeyElements);
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
    CompareResult compareResult = mapIterateAndCompare(map, element, &ptr);

    if (compareResult == EQUAL || compareResult == EQUAL_TO_FIRST) return true;   // element found

    return false;   // element not found
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    // NULL check for parameters
    if (!map || !keyElement || !dataElement) return MAP_NULL_ARGUMENT;

    // copy data
    MapDataElement new_data = map->copyDataElement(dataElement);
    if (!new_data) return MAP_OUT_OF_MEMORY;

    // iterate on the map and compare
    MapNode ptr = map->head;
    CompareResult compareResult = mapIterateAndCompare(map, keyElement, &ptr);

    MapNode new_node;
    switch (compareResult) {
        case EQUAL:                  // if key exist, update the key's data
            map->freeDataElement(ptr->next->data);
            ptr->next->data = new_data;
            break;
        case EQUAL_TO_FIRST:         // different actions if it is equal to the first
            map->freeDataElement(ptr->data);
            ptr->data = new_data;
            break;
        default:
            new_node = nodeCreate(map, keyElement, new_data);   // if key doesn't exist, create new node
            if (!new_node) {
                map->freeDataElement(new_data);
                return MAP_OUT_OF_MEMORY;
            }
            switch (compareResult) {
                case START_OF_MAP:   // if the given key is the smallest key or map is empty, insert the new node to the head
                    new_node->next = map->head;
                    map->head = new_node;
                    break;
                case END_OF_MAP:    // if the given key is the largest key, insert in the end of map
                    ptr->next = new_node;
                    break;
                default:            // case MIDDLE_OF_MAP: insert the new node where it should be
                    new_node->next = ptr->next;
                    ptr->next = new_node;
                    break;
            }
            break;
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

MapResult mapRemove(Map map, MapKeyElement keyElement){
    if (!map || !keyElement) return MAP_NULL_ARGUMENT;  // NULL parameter received

    // iterate on the map and compare
    MapNode ptr = map->head;
    CompareResult compareResult = mapIterateAndCompare(map, keyElement, &ptr);

    if (compareResult != EQUAL && compareResult != EQUAL_TO_FIRST) {
        return MAP_ITEM_DOES_NOT_EXIST;     // map doesn't contain given key
    }

    // if key exists, delete the node & connect the previous and next node
    MapNode node = map->iterator;
    if (compareResult == EQUAL) {
        MapNode next_node = node->next->next;
        map->freeDataElement(node->next->data);
        map->freeKeyElement(node->next->key);
        nodeDestroy(node->next);
        node->next = next_node;
    } else {    // compareResult == EQUAL_TO_FIRST
        // different actions if it is equal to the first
        MapNode next_node = node->next;
        map->freeDataElement(node->data);
        map->freeKeyElement(node->key);
        nodeDestroy(node);
        map->head = next_node;
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

        MapNode nodeToDestroy = ptr;
        ptr = ptr->next;                // increment the pointer
        nodeDestroy(nodeToDestroy);     // free the current node
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
