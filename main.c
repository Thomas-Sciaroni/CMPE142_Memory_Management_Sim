//
// Created by ameen on 5/10/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PAGE_AMT 20
#define SWAP_MEMORY_AMT 1000

typedef struct _page_mem {
    int data;
    bool dirty;
} page_mem;

typedef struct _main_mem {
    int *p_id;
    int *vir_page;
    page_mem *pages;
} main_mem;

typedef struct _swap_mem {
    int *p_id;
    int *vir_page;
    page_mem *pages;
} swap_mem;

typedef struct _page_table {
    int p_id; // process ID for page table
    int *translations; // array of translations
    bool *valid; // is this a valid translation (TRUE if Valid, FALSE if not)
    bool *present; // is the page swapped (TRUE if not swapped, FALSE if swapped)
} page_table;

bool add_translation(page_table *, int vir_page);

bool remove_translation(main_mem *, int page_amnt, int vir_page);

bool accessMemory(page_table *, main_mem *, int vir_page, char operation);

bool allocateMemory(page_table *, swap_mem *, main_mem *, int vir_page, char operation);

void main_to_swap(page_table *, swap_mem *, main_mem *, int index);

void swap_to_main(page_table *, swap_mem *, main_mem *, int index);

void clear_pages(swap_mem *, main_mem *, int page_amt);

//void run_processes(char * file_name, void(*swag_alg))
void FIFO_swap(char *file_name, main_mem *, swap_mem *swap);

void LRU_swap(char *file_name, main_mem *, swap_mem *swap);

void random_swap(char *file_name, main_mem *, swap_mem *swap);

int main(){

    main_mem *main_memory = (main_mem *) (malloc(sizeof(main_mem)));
    main_memory->p_id = ((int *) malloc(sizeof(int) * PAGE_AMT));
    main_memory->vir_page = ((int *) malloc(sizeof(int) * PAGE_AMT));
    main_memory->pages = ((page_mem *) malloc(sizeof(page_mem) * PAGE_AMT));

    swap_mem *swap_memory = (swap_mem *) (malloc(sizeof(swap_mem)));
    swap_memory->p_id = ((int *) malloc(sizeof(int) * SWAP_MEMORY_AMT));
    swap_memory->vir_page = ((int *) malloc(sizeof(int) * SWAP_MEMORY_AMT));
    swap_memory->pages = ((page_mem *) malloc(sizeof(page_mem) * SWAP_MEMORY_AMT));


    char file_name[32] = "memory.dat";
    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL) {
        perror("Cannot open file. \n");
    }
    fclose(f);

    // random_swap(file_name, main_memory, swap_memory)
    return 0;

}

void random_swap(char *file_name, main_mem *main_memory, swap_mem *swap_memory) {
    char line[100];
    char *token;
    FILE *f;
    int incrementer = 0;
    f = fopen(file_name, "r");
    int p_id, v_id;
    char operation;
    while (fgets(line, 100, f)) {
        token = strtok(line, " \t\n");
        incrementer = 0;
        while (token != NULL) {
            if (incrementer == 0) {
                p_id = atoi(token);
                incrementer++;
            } else if (incrementer == 1) {
                operation = *token;
                incrementer++;
            } else {
                v_id = atoi(token);
            }
            if (incrementer < 2) {
                v_id = -1;
            };
            //DATA IS IN p_id


            token = strtok(NULL, " \t\n");
        }
    }
}

//only use for reading/writing
bool accessMemory(page_table * page_tb, main_mem * main_m, int vir_page, char operation){
    if(!(page_tb->present[vir_page] && page_tb->valid))
        return false;
    if(operation == 'R') {
        //just read the data, no need to do anything with it
        int temp = main_m->pages[page_tb->translations[vir_page]].data;
        return true;
    }
    else if(operation == 'W'){
        main_m->pages[page_tb->translations[vir_page]].data = 1;
        main_m->pages[page_tb->translations[vir_page]].dirty = true;
        return true;
    }
    else return false;
}

//only use for allocating and deallocate memory
//Allocation: TRUE if allocated, FALSE if no memory OR modified pages left (SWAP MUST BE USED)
//Allocation: SWAP occurs when all pages full but at least one page remains unmodified
//De-allocation: TRUE if freed, FALSE if access was not granted or page is in SWAP
/*
bool allocateMemory(page_table * page_tb, swap_mem * swap_m, main_mem * main_m, int vir_page, char operation){
    int index = -1;
    if(operation == 'A'){
        for(int i = 0; i < PAGE_AMT; i ++){
            if (main_m->p_id[i] == -1){
                main_m->p_id[i] = page_tb->p_id;
                main_m->vir_page[i] = vir_page;
                main_m->pages[i].dirty = false;
                main_m->pages[i].data = 0;
                return true;
            }
            else if(!(main_m->pages[i].dirty)){
                index = i;
            }
        }
        if(index > -1){

        }
        else return false;
    }
    else if(operation == 'F' || opeation == T)
}
 */