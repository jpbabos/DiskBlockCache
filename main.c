#include <stdlib.h>
#include <stdio.h>

#define LOCKED 0
#define LRU 1

struct node {
    int id;
    int status;
    struct node* hash_next;
    struct node* list_prev;
    struct node* list_next;
};

typedef struct node block_type;

block_type* lru_list = NULL;
block_type* locked_list = NULL;

block_type** hash_table = NULL;

int num_indices;

void option1(){
    printf("Enter number of indices for hash table: ");
    scanf("%d", &num_indices);

    hash_table = (block_type**)malloc(num_indices * sizeof(block_type*));

    for(int i = 0; i < num_indices; i++){
        hash_table[i] = NULL;
    }
}

void printTable(){
    block_type* curr_block;

    for(int i = 0; i < num_indices; i++){
        curr_block = hash_table[i];
        while(curr_block != NULL){
            printf("%d\t", i);
            printf("%d\t", curr_block->id);
            if(curr_block->status == LOCKED){
                printf("Locked\t");
            }
            else{
                printf("LRU\t");
            }
            if(curr_block->list_prev != NULL){
                printf("%d\t", curr_block->list_prev->id);
            }
            else{
                printf("\t");
            }
            if(curr_block->list_next != NULL){
                printf("%d\n", curr_block->list_next->id);
            }
            else{
                printf("\n");
            }
            curr_block = curr_block->hash_next;

        }
    }
    printf("\n");
}

void option2(){
    int row;
    int found = 0;
    int id;
    int status;
    block_type *new_block, *curr_block, *found_block, *temp_prev, *temp_next;

    printf("Enter block id: ");
    scanf("%d", &id);
    printf("Enter status (0 = LOCKED, 1 = LRU): ");
    scanf("%d", &status);

    row = id / num_indices;

    if(hash_table[row] == NULL){ //if hash table row is empty, add a new block
        new_block = (block_type*)malloc(sizeof(block_type));
        new_block->id = id;
        new_block->status = status;
        new_block->hash_next = NULL;
        new_block->list_prev = NULL;
        new_block->list_next = NULL;
        hash_table[row] = new_block;
    }
    else { //search hash table row for entry
        curr_block = hash_table[row];

        if ((curr_block->hash_next == NULL) &&
            (curr_block->id == id)) {//if found in 1st and only block of hash table row
            if (curr_block->status != status) {
                printf("Block %d found with wrong status\n", curr_block->id);
                return;
            }
            found_block = curr_block;
            found = 1;
        } else { //traverse hash table until either block is found or reached last block of row
            while (curr_block->hash_next != NULL) {
                if (curr_block->id == id) {
                    if (curr_block->status != status) {
                        printf("Block %d found with wrong status\n", curr_block->id);
                        return;
                    }
                    found = 1;
                    found_block = curr_block;
                    break;
                }
                curr_block = curr_block->hash_next;
            }
            if ((curr_block->id == id) && (found == 0)) {//check last block in hash table for matching id
                if (curr_block->status != status) {
                    printf("Block %d found with wrong status\n", curr_block->id);
                    return;
                }
                found = 1;
                found_block = curr_block;
            }
            if (found == 0) {//if block not found, add to end of hash table row
                new_block = (block_type *) malloc(sizeof(block_type));
                new_block->id = id;
                new_block->status = status;
                new_block->hash_next = NULL;
                new_block->list_prev = NULL;
                new_block->list_next = NULL;
                curr_block->hash_next = new_block;
            }
        }
    }
    if(status == 0){
        if(found == 1){ //if already found in locked list
            printf("Already in locked list\n");
            return;
        }
        if(locked_list == NULL){ //if locked list is empty, add to head of locked list
            locked_list = new_block;
            printTable();
            return;
        }
        else{//else traverse to end of locked list and add block
            curr_block = locked_list;
            while(curr_block->list_next != NULL){
                curr_block = curr_block->list_next;
            }
            curr_block->list_next = new_block;
            printTable();
            return;
        }
    }
    else{
        if(lru_list == NULL){ //if lru list is empty, add to head of locked list
            lru_list = new_block;
            new_block->list_prev = NULL;
            printTable();
            return;
        }
        else{ //traverse to end of lru list and add block
            curr_block = lru_list;
            while(curr_block->list_next != NULL){
                curr_block = curr_block->list_next;
            }
            if(found == 0){ //if it was not found in hash table row, add to end of lru list
                curr_block->list_next = new_block;
                new_block->list_prev = curr_block;
            }
            else{ //if found = 1
                if(lru_list->id == id){
                    if(lru_list->list_next == NULL){ //if only one block in lru list
                        return;
                    }
                    lru_list = lru_list->list_next;
                    lru_list->list_prev = NULL;
                    found_block->list_next = NULL;
                    found_block->list_prev = curr_block;
                    curr_block->list_next = found_block;
                    return;
                }

                if(curr_block->id == id){
                    return;
                }
                else{
                    temp_prev = found_block->list_prev;
                    temp_next = found_block->list_next;
                    temp_prev->list_next = temp_next;
                    if(temp_next != NULL){
                        temp_next->list_prev = temp_prev;
                    }
                    found_block->list_next = NULL;
                    found_block->list_prev = curr_block;
                    curr_block->list_next = found_block;
                }
            }
        }
    }
    printTable();
}

void option3(){
    int id;
    int row;
    block_type *found_block = NULL;
    block_type *curr_block, *prev_block, *prev_hash, *temp_prev, *temp_next;

    printf("Enter block id: ");
    scanf("%d", &id);
    row = id / num_indices;

    if(hash_table[row] == NULL){ //if hash table row is empty
        return;
    }
    if(hash_table[row]->id == id){//if block is found at head of hash table, check status
        if(hash_table[row]->status == LOCKED){
            printf("Locked");
            return;
        }
        else{ //record found block and adjust hash table pointer to next block
            found_block = hash_table[row];
            hash_table[row] = found_block->hash_next;
        }
    }
    else{ //traverse hash table row until block found or last block of hash table row reached
        curr_block = hash_table[row];
        prev_block = hash_table[row];
        while(curr_block->hash_next != NULL){
            if(curr_block->id == id){
                if(curr_block->status == LOCKED){
                    return;
                }
                else{
                    found_block = curr_block;
                    prev_block->hash_next = curr_block->hash_next;
                }
            }
            prev_hash = curr_block;
            curr_block = curr_block->hash_next;
        }
        if(curr_block->id == id){ //if found in end of hash table row
            if(curr_block->status == LOCKED){
                return;
            }
            found_block = curr_block;
            prev_hash->hash_next = NULL;
        }
        if(found_block == NULL){ //if block was not found
            printf("Block not found\n");
            return;
        }
        if(found_block->id == lru_list->id) { //if block is found at head of lru list
            lru_list = lru_list->list_next;
            if (lru_list != NULL) {
                lru_list->list_prev = NULL;
                free(found_block);
                printTable();
                return;
            }
        }



    }
    //adjust previous and next pointers in lru list
    temp_prev = found_block->list_prev;
    temp_next = found_block->list_next;
    temp_prev->list_next = temp_next;
    if(temp_next != NULL){
        temp_next->list_prev = temp_prev;
    }
    free(found_block);
    printTable();
}

void option4(){
    for(int i = 0; i < num_indices; i++){
        if(hash_table[i] != NULL){
            free(hash_table[i]);
        }
    }
}

int main(){
    int choice = 0;

    while(choice != 4){
        printf("*********************************************************\n");
        printf("Disk Block Cache!\n\n");
        printf("Please choose one of the options below:\n");
        printf("1. Enter Parameters\n");
        printf("2. Insert/access disk block in cache\n");
        printf("3. Delete disk block from cache\n");
        printf("4. Quit\n");

        printf("Selection:");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                option1();
                break;
            case 2:
                option2();
                break;
            case 3:
                option3();
                break;
        }
    }
    option4();
    printf("Have a great day!");
}
