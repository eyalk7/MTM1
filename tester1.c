//library
//keys - char strings - names of books
//data - char strings - names of people

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "map.h"
#define BOOKS_NUM 10
#define PEOPLE_NUM 5

//Function for copying data elements into the map or when copying the map.
MapDataElement copyData(MapDataElement origin_data);
//Function for copying key elements into the map or when copying the map.
MapKeyElement copyKey(MapKeyElement origin_key);
//Function for removing data elements from the map
void freeData(MapDataElement to_delete);
//Function for removing key elements from the map
void freeKey(MapKeyElement to_delete);
//Function for comparing key elements inside the map. Used to check if new elements already exist in the map.
int compareKeys(MapKeyElement key_a, MapKeyElement key_b);

int main() {
    char *books[BOOKS_NUM] = { "Harry Potter",
                                "The Bible",
                                "C Programing",
                                "book4",
                                "book5",
                                "book6",
                                "book7",
                                "book8",
                                "book9",
                                "book10" };
    char *people[PEOPLE_NUM] = {"Avi",
                                 "Yosi",
                                 "Pnina",
                                 "Amos",
                                 "Hana" };

    printf("Creating map for book borrowing list...\n");
    Map book_borrowing_list = mapCreate(&copyData, &copyKey, &freeData, &freeKey, &compareKeys);
    if (book_borrowing_list == NULL) {
        printf("mapCreate Error: NULL\n");
        return -1;
    }
    printf("Map Successfully created\n\n");

    printf("Inserting data and keys to the map...\n");
    int put_results[5] = {0};
    put_results[0] = mapPut(book_borrowing_list, books[0], people[2]);
    put_results[1] = mapPut(book_borrowing_list, books[1], people[4]);
    put_results[2] = mapPut(book_borrowing_list, books[5], people[0]);
    put_results[3] = mapPut(book_borrowing_list, books[2], people[0]);
    put_results[4] = mapPut(NULL, books[2], people[0]);
    for (int i=0; i<5; i++) {
        switch (put_results[i]) {
            case MAP_NULL_ARGUMENT: printf("error: NULL was sent as map in pair #%d\n", i+1); break;
            case MAP_OUT_OF_MEMORY: printf("error: allocation failed in pair #%d\n", i+1); break;
            case MAP_SUCCESS: printf("#%d pair's elements had been inserted successfully", i+1); break;
            default: printf("mapPut Error\n");
        }
    }

    printf("\nCreating backup list...\n");
    Map backup_list = mapCopy(book_borrowing_list);
    if (backup_list == NULL) {
        printf("mapCopy Error: NULL\n");
        return -1;
    }
    printf("Map Successfully copied\n\n");

    printf("Removing one pair from the original list...\n");
    int remove_results[3] = {0};
    remove_results[0] = mapRemove(book_borrowing_list, books[0]);
    remove_results[1] = mapRemove(book_borrowing_list, "blabla");
    remove_results[2] = mapRemove(NULL, books[0]);
    for (int i=0; i<3; i++) {
        switch (remove_results[i]) {
            case MAP_NULL_ARGUMENT: printf("error: NULL was sent as map in pair #%d\n", i+1); break;
            case MAP_ITEM_DOES_NOT_EXIST: printf("error: item does not already exists in the map in key #%d\n", i+1); break;
            case MAP_SUCCESS: printf("#%d pair's elements had been inserted successfully", i+1); break;
            default: printf("mapRemove Error\n");
        }
    }

    printf("\nChecking number of book borrowed...\nborrowing list: %d\nbackup list: %d\n\n", mapGetSize(book_borrowing_list), mapGetSize(backup_list));

    printf("Full Book list:\n");
    for (int i=0; i < BOOKS_NUM; i++) {
        printf("%d) %s, ", i+1 , books[i]);
    }
    printf("\nChoose book to check if borrowed:\n...");
    char *to_check = 0;
    char answer = 0;
    do {
        scanf("%s", to_check);
        if (mapContains(book_borrowing_list, to_check)) {
            printf("%s is borrowed by %s\n", to_check, mapGet(book_borrowing_list,to_check));
        } else {
            printf("%s isn't borrowed\n", to_check);
        }
        printf("Want to check another book?\n y / n ...\n");
        scanf("%c", &answer);
    } while (answer == 'y' || answer == 'Y');

    printf("\nIterating on the whole borrowing list and print the keys...\n");
    int i=0;
    MAP_FOREACH(void*, my_iterator, book_borrowing_list){
        printf("%d) %s\n", i, my_iterator);
        i++;
    }

    printf("Clearing backup list...\n");
    mapClear(backup_list);

    printf("Deleting both maps...\n");
    mapDestroy(backup_list);
    mapDestroy(book_borrowing_list);

    printf("\nfinished test!");
}

MapDataElement copyData(MapDataElement origin_data){
    MapDataElement new_data = malloc(strlen(origin_data) + 1);
    if (new_data == NULL) return NULL;
    strcpy(new_data, origin_data);
    return new_data;
}

MapKeyElement copyKey(MapKeyElement origin_key){
    MapKeyElement new_key = malloc(strlen(origin_key) + 1);
    if (new_key == NULL) return NULL;
    strcpy(new_key, origin_key);
    return new_key;
}

void freeData(MapDataElement to_delete) {
    free(to_delete);
}

void freeKey(MapKeyElement to_delete){
    free(to_delete);
}

int compareKeys(MapKeyElement key_a, MapKeyElement key_b){
    return strcmp(key_a, key_b);
}
