#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct node_t {
    int x;
    struct node_t *next;
} *Node;

typedef enum {
    SUCCESS=0,
    MEMORY_ERROR,
    EMPTY_LIST,
    UNSORTED_LIST,
    NULL_ARGUMENT
} ErrorCode;

int getListLength(Node list);
bool isListSorted(Node list);
char* errorCodeToString(ErrorCode error);

/**
* mergeSortedLists: Merges two lists sorted in ascending order into one list sorted in ascending order.
* @param list1 - Pointer to list no. 1
* @param list2 - Pointer to list no. 2
* @param *mergedOut - Pointer used to save the merge list
* @return
*   MEMORY_ERROR error if an allocation failed (when creating the merged list)
*   EMPTY_LIST error if an empty list was sent
*   UNSORTED_LIST if one of the given lists is not sorted
* 	NULL_ARGUMENT error if NULL pointer was sent for mergedOut
*   SUCCESS if there was no memory error and the merged list was created successfully
*
*/
ErrorCode mergeSortedLists(Node list1, Node list2, Node *mergedOut);

/**
* Deallocates all the nodes in a list
* If NULL pointer is received, nothing is done
* @param lst - pointer to first node in list
*/
void freeList(Node list);

void printList(Node lst);

int main() {
    // creating list1 and printing
    Node list1 = malloc(sizeof(*list1));
    Node ptr = list1, temp = ptr;
    ptr->x = 1;
    ptr = malloc(sizeof(*ptr));
    temp->next = ptr;
    ptr->x = 4;
    temp = ptr;
    ptr = malloc(sizeof(*ptr));
    temp->next = ptr;
    ptr->x = 9;
    ptr->next = NULL;
    printList(list1);

    // creating list2 and printing
    Node list2 = malloc(sizeof(*list1));
    ptr = list2, temp = ptr;
    ptr->x = 2;
    ptr = malloc(sizeof(*ptr));
    temp->next = ptr;
    ptr->x = 4;
    temp = ptr;
    ptr = malloc(sizeof(*ptr));
    temp->next = ptr;
    ptr->x = 8;
    ptr->next = NULL;
    printList(list2);

    Node mergedOut = NULL; // initializing merged list pointer to NULL
    ErrorCode result = mergeSortedLists(list1, list2, &mergedOut);
    if (result == SUCCESS) {
        printf("Success\n");
        printList(mergedOut);   // print merged list
    }
    else
        printf("Error Code = %s\n", errorCodeToString(result));

    // deallocating all list nodes
    freeList(list1);
    freeList(list2);
    freeList(mergedOut);
    return 0;
}

ErrorCode mergeSortedLists(Node list1, Node list2, Node *mergedOut) {
    if (!mergedOut)
        return NULL_ARGUMENT;       // NULL pointer received for mergedOut

    if (getListLength(list1) == 0 || getListLength(list2) == 0)
        return EMPTY_LIST;          // empty list received

    if (!isListSorted(list1) || !isListSorted(list2))
        return UNSORTED_LIST;       // unsorted list received

    // make pointers for iterating through given lists and the merged list
    Node node = NULL, prevNode = NULL, ptr1 = list1, ptr2 = list2;
    bool firstIteration = true;
    while (ptr1 && ptr2) {
        node = malloc(sizeof(*node));   // allocate new node
        if (!node) {
            // allocation failed
            if (!firstIteration) {
                freeList(*mergedOut);   // free all nodes created so far
            }
            return MEMORY_ERROR;
        }

        if (firstIteration) {
            *mergedOut = node;      // save the pointer to the merged list
            firstIteration = false;
        } else {
            prevNode->next = node;      // set previous node's next
        }

        // set node data to be smaller data
        // increment pointer of list with smaller data
        if (ptr1->x < ptr2->x) {
            node->x = ptr1->x;
            ptr1 = ptr1->next;
        } else {
            node->x = ptr2->x;
            ptr2 = ptr2->next;
        }

        prevNode = node;        // save current node for next iteration
    }

    // if one list is longer than the other
    // finish the merged list with the pointer that isn't NULL
    Node ptr = ptr1;
    if (ptr2) {
        ptr = ptr2;
    }
    while (ptr) {
        node = malloc(sizeof(*node));
        if (!node) {
            // allocation failed
            freeList(*mergedOut);       // free all nodes created so far
            return MEMORY_ERROR;
        }

        node->x = ptr->x;          // set node data
        prevNode->next = node;     // set previous node's next
        prevNode = node;           // save current node for next iteration
        ptr = ptr->next;           // increment pointer
    }

    node->next = NULL;             // set last node to point to null
    return SUCCESS;
}

void freeList(Node list) {
    while (list) {
        Node temp = list;
        list = list->next;
        free(temp);
    }
    // if NULL pointer is received nothing is done
}

void printList(Node lst) {
    if (lst) {
        printf("Printing a list\n");
        Node ptr;
        for (ptr = lst; ptr->next; ptr = ptr->next) {
            printf("%d -> ", ptr->x);
        }
        printf("%d\n", ptr->x);
    }
    else
        printf("Given list is NULL\n");
}

char* errorCodeToString(ErrorCode error) {
    char * codes[] = {"SUCCESS", "MEMORY_ERROR", "EMPTY_LIST", "UNSORTED_LIST", "NULL_ARGUMENT"};

    return codes[error];
}

int getListLength(Node list) {
    if (!list) return 0; // NUll pointer received (empty list)

    int count = 0;
    for (Node ptr = list; ptr; ptr = ptr->next) {
        count++;
    }

    return count;
}

bool isListSorted(Node list) {
    if (!list) return true;  // NUll pointer received (empty list)

    for (Node ptr = list; ptr->next; ptr = ptr->next) {
        int current = ptr->x;
        int next = ptr->next->x;
        if (current > next)
            return false;   // is not sorted
    }

    return true;    // is sorted
}

