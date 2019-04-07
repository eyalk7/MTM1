#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BOOL int
#define FALSE 0
#define TRUE 1

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

ErrorCode mergeSortedLists(Node list1, Node list2, Node *mergedOut);
int getListLength(Node list);
BOOL isListSorted(Node list);

/*
 * Deallocates all the nodes in a list
 * If given pointer is NULL does nothing
 * @param lst - pointer to first node in list
 */
void freeList(Node lst);

/*
 * Prints the given list
 * If given pointer is NULL prints a message accordingly
 * @param lst - pointer to the first node of the list
 */
void printList(Node lst);

/*
 * Converts an ErrorCode to its string counterpart
 * @param error - the ErrorCode to convert
 * @return Returns string form of given error code
 */
char* errorCodeToString(ErrorCode error);


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
    //ptr = malloc(sizeof(*ptr));
    //temp->next = ptr;
    //ptr->x = 4;
    //temp = ptr;
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
    // Check if given lists are NULL
    if (!list1 || !list2)
        return NULL_ARGUMENT;

    // Check if given lists are empty
    if (getListLength(list1) == 0|| getListLength(list2) == 0)
        return EMPTY_LIST;

    // Check if given lists are sorted
    if (!isListSorted(list1) || !isListSorted(list2))
        return UNSORTED_LIST;

    BOOL firstIteration = TRUE;
    Node node = NULL, prevNode = NULL, ptr1 = list1, ptr2 = list2;
    while (ptr1 && ptr2) {
        // create node
        node = malloc(sizeof(*node));
        if (!node) {
            if (firstIteration)
                free(node);
            else
                freeList(*mergedOut); // free all nodes created so far (starting from first)
            return MEMORY_ERROR; // allocation failed
        }

        if (firstIteration) {
            *mergedOut = node;
            printf("Merged List has been set\n");
            firstIteration = FALSE;
        }
        else
            prevNode->next = node;  // set previous node's next

           // set node data
        if (ptr1->x < ptr2->x) {
            node->x = ptr1->x;
            ptr1 = ptr1->next;
        } else {
            node->x = ptr2->x;
            ptr2 = ptr2->next;
        }

        prevNode = node;    // save current node for next iteration
    }

    // if one list is longer than the other
    // finish the merged list with the pointer that isn't NULL
    Node ptr = ptr1;
    if (ptr2)
        ptr = ptr2;


    while (ptr) {
        node = malloc(sizeof(*node));
        if (!node) {
            // free all nodes created so far (starting from first)
            freeList(*mergedOut);
            return MEMORY_ERROR; // allocation failed
        }

        node->x = ptr->x;
        prevNode->next = node;
        prevNode = node;
        ptr = ptr->next;
    }

    // set last node to point to null
    node->next = NULL;

    printf("End of Merged Function\n");
    return SUCCESS;
}

void freeList(Node lst) {
    printf("Freeing a list\n");
    while (lst) {
        Node temp = lst;
        lst = lst->next;
        free(temp);
    }
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
    char * codes[] = {"SUCCESS", "MEMORY_ERROR", "EMPTY_LIST", "UNSORTED_LIST"};

    return codes[error];
}

int getListLength(Node list) {
    if (!list)
        return 0;

    int count = 0;
    for (Node ptr = list; ptr; ptr = ptr->next) {
        count++;
    }

    return count;
}

BOOL isListSorted(Node list) {
    if (!list)
        return TRUE;    // empty

    for (Node ptr = list; ptr->next; ptr = ptr->next) {
        int current = ptr->x;
        int next = ptr->next->x;
        if (current > next)
            return FALSE;   // not sorted
    }

    return TRUE;    // sorted
}

