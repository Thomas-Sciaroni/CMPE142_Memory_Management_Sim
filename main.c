//
// Created by ameen on 5/10/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PAGE_AMT 20
#define PAGE_TABLE_DEFAULT_SIZE 1000

typedef struct _main_mem {
    int *p_id;
    int *vir_page;
    int *data;
    bool *dirty;
    bool *accessed;
    bool *inSwap;
} main_mem;

typedef struct _swap_mem {
    int p_id;
    int vir_page;
    int data;
} swap_mem;

typedef struct _node_sm {
    swap_mem data;
    struct _node_sm * next;
} node_sm;

typedef struct _translation {
    int virtualPage;
    int physicalPage;
    bool present;
} translation;

typedef struct _node_t {
    translation data;
    struct _node_t * next;
} node_t;

typedef struct _page_table {
    int p_id; // process ID for page table
    node_t* head;
} page_table;

typedef struct _process_list {
    page_table* pageTable;
    struct _process_list* next;
} process_list;

typedef struct _LRU_queue {
    int physicalPage;
    struct _LRU_queue * next;
} node_lq;

typedef struct _kill_list {
    int PID;
    struct _kill_list * next;
} node_kl;

void mainInit(main_mem *);

void swapInit(node_sm *);

void mainDealloc(main_mem *);

void swapDealloc(node_sm *);

void killInit(node_kl *);

void killDealloc(node_kl *);

void add_translation(process_list *pl, int PID,  int vir_page, int phys_page);

void remove_translation(process_list *pl, int PID, int vir_page);

bool readMemory(process_list *pl, main_mem *main_m, node_kl *kl, int PID, int vir_page, int* readData);

bool writeMemory(process_list *pl, main_mem *main_m, node_kl *kl, int PID, int vir_page, int writeData);

bool allocateMemory(process_list *pl, node_sm * swapHead, main_mem *main_m, int PID, int vir_page);

bool freeMemory(process_list *pl, node_sm * swapHead, main_mem *main_m, node_kl *kl, int PID, int vir_page);

void swapDirty(process_list *pl, node_sm * swapHead, main_mem *main, int PID1, int virPage1, int PID2, int virPage2, int main_index);

void swapClean(process_list *pl, node_sm * swapHead, main_mem *main, int PID, int vir_page, int main_index);

void kill(node_kl *kl, int PID);

int lowestCleanPage(main_mem *);


//void run_processes(char * file_name, void(*swag_alg))
void FIFO_swap(char *file_name, main_mem *, node_sm * headSM, node_kl *headKL);

void LRU_swap(char *file_name, main_mem *, node_sm * headSM, node_kl *headKL);

void random_swap(char *file_name, main_mem *, node_sm * headSM, node_kl *headKL);

int main() {
    main_mem *main_memory = NULL;
    node_sm *headSM = NULL;
    node_kl *headKL = NULL

    char file_name[32] = "memory.dat";
    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL) {
        perror("Cannot open file. \n");
    }
    fclose(f);

    mainInit(main_memory);
    swapInit(headSM);
    killInit(headKL);
    printf("Random Swap: \n");
    //random_swap(file_name, main_memory, headSM, headKL);
    killDealloc(headKL);
    mainDealloc(main_memory);
    swapDealloc(headSM);

    mainInit(main_memory);
    swapInit(headSM);
    killInit(headKL);
    printf("LRU Swap: \n");
    // LRU_swap(file_name, main_memory, headSM, headKL);
    killDealloc(headKL);
    mainDealloc(main_memory);
    swapDealloc(headSM);

    mainInit(main_memory);
    swapInit(headSM);
    killInit(headKL);
    printf("FIFO Swap: \n");
    // FIFO_swap(file_name, main_memory, headSM, headKL);
    killDealloc(headKL);
    mainDealloc(main_memory);
    swapDealloc(headSM);

    return 0;

}

void random_swap(char *file_name, main_mem *main_memory, node_sm * swapHead) {
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
        v_id = -1;
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

            token = strtok(NULL, " \t\n");
        }

        //process the line

        //check if the operation is process creation or termination
        if(v_id == -1){
            //check if operation is creation of process
            if(operation == 'C'){
                //create page table for process
                for(int i =0; i<SWAP_MEMORY_AMT; i++){
                    if(pt[i].p_id != -1){
                        pt[i].p_id = p_id;
                    }
                }
            }
            else{
                for(int i = 0; i<PAGE_AMT; i++){
                    if(main_memory->p_id[i] == p_id){
                        main_memory->p_id[i] = -1;
                        main_memory->pages[i].data =-1;
                        main_memory->pages[i].dirty =false;
                        main_memory->pages[i].accessed = false;

                    }
                }
                for(int i = 0; i<SWAP_MEMORY_AMT; i++){
                    if(pt[i].p_id = p_id){
                        pt[i].p_id = -1;
                        pt[i].valid = false;
                        pt[i].present = false;
                    }
                }
            }
        }
        else{
            if(operation == 'W' || operation == 'R'){
                bool access = accessMemory(pt,main_memory,v_id,operation);
                if (access){
                    continue;
                }
            }
            else if(operation == 'A' || operation == 'F'){
                bool success = allocateMemory(pt, swap_memory,main_memory,v_id, operation);
                if(success){
                    continue;
                }

                else{
                    //swap function
                    //find random page number in main memory
                    int num = (rand() % 20);
                    main_to_swap(pt,swap_memory,main_memory,num);
                    main_memory->p_id[num] = p_id;
                    main_memory->vir_page = v_id;

                }
            }

        }

    }
}

//only use for reading/writing
bool readMemory(process_list *pl, main_mem *main_m, node_kl *kl, int PID, int vir_page, int* readData) {
    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID){
        currentPL = currentPL->next;
    }
    node_t * current = currentPL->next->pageTable->head;
    while( (current->next != NULL) && (current->data.virtualPage != vir_page) ){
        current = current->next;
    }
    if( (current->next == NULL) && (current->data.virtualPage != vir_page) ){
        kill(kl, PID);
        return false;
    }
    if(current->data.present){ //in main mem
        *readData = main_m->data[current->data.physicalPage];
        main_m->accessed[current->data.physicalPage] = true;
        return true;
    }
    return false;
}

bool writeMemory(process_list *pl, main_mem *main_m, node_kl *kl, int PID, int vir_page, int writeData){
    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID){
        currentPL = currentPL->next;
    }
    node_t * current = currentPL->next->pageTable->head;
    while( (current->next != NULL) && (current->data.virtualPage != vir_page) ){
        current = current->next;
    }
    if( (current->next == NULL) && (current->data.virtualPage != vir_page) ){
        kill(kl, PID);
        return false;
    }
    if(current->data.present){ //in main mem
        main_m->data[current->data.physicalPage] = writeData;
        main_m->accessed[current->data.physicalPage] = true;
        main_m->dirty[current->data.physicalPage] = true;
        return true;
    }
    return false;
}

//only use for allocating and deallocate memory
//Allocation: TRUE if allocated, FALSE if no memory OR modified pages left (SWAP MUST BE USED)
//Allocation: SWAP occurs when all pages full but at least one page remains unmodified
//De-allocation: TRUE if freed, FALSE if access was not granted, or not found

//TODO: CHECK PAGE TABLE FOR VALID TRANSLATION

bool allocateMemory(process_list *pl, node_sm * swapHead, main_mem *main_m, int PID, int vir_page) {
    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID){
        currentPL = currentPL->next;
    }
    node_t * current = currentPL->next->pageTable->head;
    int index = -1;
    for(int i = 0; i != PAGE_AMT; ++i){
        if(main_m->p_id[i] == -1){
            index = i;
            break;
        }
    }
    if(index == -1){ //main mem full
        for(int i = 0; i != PAGE_AMT; ++i){
            if(main_m->dirty[i] == false){
                index = i;
                break;
            }
        }
        if(index != -1){
            swapClean(pl, swapHead, main_m, PID, vir_page, index);
            return true;
        }
        else{
            return false;
        }

    }
    else{
        main_m->p_id[index] = currentPL->next->pageTable->p_id;
        main_m->data[index] = 0;
        main_m->inSwap[index] = false;
        main_m->dirty[index] = false;
        main_m->accessed[index] = false;
    }
}

bool freeMemory(process_list *pl, node_sm * swapHead, main_mem *main_m, node_kl *kl, int PID, int vir_page){
    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID){
        currentPL = currentPL->next;
    }
    node_t * currentT = currentPL->next->pageTable->head;
    while( (currentT->next != NULL) && currentT->next->data.virtualPage != vir_page){
        currentT = currentT->next;
    }
    if(currentT->next == NULL){
        return false;
    }
    else{
        if(currentT->next->data.present){ //clear main mem
            int index = currentT->next->data.physicalPage;
            main_m->data[index] = 0;
            main_m->p_id[index] = -1;
            main_m->dirty[index] = false;
            main_m->accessed[index] = false;
            main_m->inSwap[index] = false;
        }
        else{ //clear swap mem
            node_sm * currentSM = swapHead;
            while(currentSM->next != NULL && !( (currentSM->data.p_id == PID) && (currentSM->data.vir_page == vir_page) ) ){
                currentSM = currentSM->next;
            }
            if(currentSM->next == NULL){
                kill(kl, PID);
            }
            else{
                node_sm *tmp = currentSM->next->next;
                free(currentSM->next);
                currentSM->next = tmp;
            }
        }
        node_t * tmp = currentT->next->next;
        free(currentT->next);
        currentT->next = tmp;
    }
}

void add_translation(process_list *pl, int PID,  int vir_page, int phys_page){
    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID){
        currentPL = currentPL->next;
    }
    node_t * current = currentPL->next->pageTable->head;
    while( (current->next != NULL) && current->next->data.virtualPage != vir_page){
        current = current->next;
    }
    if(current->next->data.virtualPage == vir_page){ //update old mapping
        current->next->data.physicalPage = phys_page;
        current->next->data.present = true;
    }
    else{//add new mapping
        current->next = malloc(sizeof(node_t));
        current->next->data.virtualPage = vir_page;
        current->next->data.physicalPage = phys_page;
        current->next->data.present = true;
        current->next->next = NULL;
        return;
    }
}

void remove_translation(process_list *pl, int PID,  int vir_page){
    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID){
        currentPL = currentPL->next;
    }
    node_t * current = currentPL->next->pageTable->head;
    while( (current->next->data.virtualPage != vir_page) ){
        current = current->next;
    }
    current->next->data.present = false;
    current->next->data.physicalPage = -1;
}

//puts virPage1 into main mem, virPage2 into swap
void swapDirty(process_list *pl, node_sm * swapHead, main_mem *main, int PID1, int virPage1, int PID2, int virPage2, int main_index){
    int tmpData = main->data[main_index];
    bool tmpInSwap = main->inSwap[main_index];
    remove_translation(pl, PID1, virPage1);
    add_translation(pl, PID1, virPage1, main_index);
    remove_translation(pl, PID2, virPage2);

    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID1){
        currentPL = currentPL->next;
    }
    node_t * pt1 = currentPL->next->pageTable->head;

    currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID2){
        currentPL = currentPL->next;
    }
    node_t * pt2 = currentPL->next->pageTable->head;

    //get the values out of swap and into main mem
    node_sm * current = swapHead;
    while( (current->next != NULL) && !( (current->data.p_id == PID1) && (current->data.vir_page == virPage1) ) ){
        current = current->next;
    }
    main->data[main_index] = current->data.data;
    main->dirty[main_index] = false;
    main->accessed[main_index] = false;
    main->inSwap[main_index] = true;

    if(tmpInSwap){ //There is an old page
        node_sm * oldpage = swapHead;
        while ((oldpage->next != NULL)) {
            oldpage = oldpage->next;
        }
        oldpage->data.data = tmpData;
    }
    else { //No old page, just create a new entry in swap linked list
        node_sm * cur = swapHead;
        while ((cur->next != NULL)) {
            cur = cur->next;
        }
        cur->next = malloc(sizeof(node_sm));
        cur->next->data.data = tmpData;
        cur->next->next = NULL;
    }
}

void swapClean(process_list *pl, node_sm * swapHead, main_mem *main, int PID, int vir_page, int main_index){
    int tmpData = main->data[main_index];
    int tmpPID = main->p_id[main_index];
    int tmpVP = main->vir_page[main_index];
    bool tmpInSwap = main->inSwap[main_index];

    remove_translation(pl, PID, vir_page);
    add_translation(pl, PID, vir_page, main_index);
    remove_translation(pl, tmpPID, tmpVP);

    process_list * currentPL = pl;
    while(currentPL->next->pageTable->p_id != PID){
        currentPL = currentPL->next;
    }
    node_t * pt1 = currentPL->next->pageTable->head;

    node_sm * current = swapHead;
    while( (current->next != NULL) && !( (current->data.p_id == PID) && (current->data.vir_page == vir_page) ) ){
        current = current->next;
    }
    main->data[main_index] = current->data.data;
    main->dirty[main_index] = false;
    main->accessed[main_index] = false;
    main->inSwap[main_index] = true;

    if(!tmpInSwap){ //No old page, just create a new entry in swap linked list
        node_sm * cur = swapHead;
        while ((cur->next != NULL)) {
            cur = cur->next;
        }
        cur->next = malloc(sizeof(node_sm));
        cur->next->data.data = tmpData;
        cur->next->next = NULL;
    }
}

void mainInit(main_mem * main){
    main = (main_mem *) (malloc(sizeof(main_mem)));
    main->p_id = ((int *) malloc(sizeof(int) * PAGE_AMT));
    main->vir_page = ((int *) malloc(sizeof(int) * PAGE_AMT));
    main->data = ((int *) malloc(sizeof(int) * PAGE_AMT));
    main->dirty = ((bool *) malloc(sizeof(bool) * PAGE_AMT));
    main->accessed = ((bool *) malloc(sizeof(bool) * PAGE_AMT));
    main->inSwap = ((bool *) malloc(sizeof(bool) * PAGE_AMT));
    for(int i = 0; i != PAGE_AMT; ++i){
        main->p_id[i] = -1;
        main->vir_page[i] = -1;
        main->data[i] = 0;
        main->dirty[i] = false;
        main->accessed[i] = false;
        main->inSwap[i] = false;
    }
}

void swapInit(node_sm *head){
    head = malloc(sizeof(node_sm));
    head->data.p_id = -1;
    head->next = NULL;
}

void mainDealloc(main_mem* main){
    free(main->p_id);
    free(main->vir_page);
    free(main->data);
    free(main->dirty);
    free(main->accessed);
    free(main->inSwap);
    free(main);
}

void swapDealloc(node_sm* head){
    node_sm * current = head;
    node_sm * delete;
    while(current->next != NULL){
        delete = current;
        current = current->next;
        free(delete);
    }
    free(current);
}

void killInit(node_kl *head){
    head = malloc(sizeof(node_sm));
    head->PID = -1;
    head->next = NULL;
}

void killDealloc(node_kl* head){
    node_kl * current = head;
    node_kl * delete;
    while(current->next != NULL){
        delete = current;
        current = current->next;
        free(delete);
    }
    free(current);
}

void kill(node_kl *kl, int PID){
    kl->next = malloc(sizeof(node_kl));
    kl->next->PID = PID;
    kl->next->next = NULL;
}

int lowestCleanPage(main_mem * main){
    int index = -1;
    for(int i = 0; i != PAGE_AMT; ++i){
        if(main->dirty[i] == -1){
            index = i;
            break;
        }
    }
    return index;
}