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
    bool accessed;
} page_mem;

typedef struct _main_mem {
    int *p_id;
    int *vir_page;
    page_mem *pages;
} main_mem;

typedef struct _swap_mem {
    int p_id;
    int vir_page;
} swap_mem;

typedef struct node {
    swap_mem data;
    struct node * next;
} node_sm;

typedef struct _page_table {
    int p_id; // process ID for page table
    int *translations; // array of translations
    bool *valid; // is this a valid translation (TRUE if Valid, FALSE if not)
    bool *present; // is the page swapped (TRUE if not swapped, FALSE if swapped)
} page_table;

void add_translation(page_table *, int vir_page, int phys_page);

void remove_translation(page_table *, int vir_page);

bool accessMemory(page_table *, main_mem *, int vir_page, char operation);

bool allocateMemory(page_table *, swap_mem *, main_mem *, int vir_page, char operation);

void main_to_swap(page_table *, swap_mem *, main_mem *, int index);

void swap_to_main(page_table *, swap_mem *, main_mem *, int index);

void clear_pages(swap_mem *, main_mem *, int page_amt);

int unmodified_pages(main_mem *);

//void run_processes(char * file_name, void(*swag_alg))
void FIFO_swap(char *file_name, main_mem *, swap_mem *swap);

void LRU_swap(char *file_name, main_mem *, swap_mem *swap);

void random_swap(char *file_name, main_mem *, swap_mem *swap);

int main() {

    main_mem *main_memory = (main_mem *) (malloc(sizeof(main_mem)));
    main_memory->p_id = ((int *) malloc(sizeof(int) * PAGE_AMT));
    main_memory->vir_page = ((int *) malloc(sizeof(int) * PAGE_AMT));
    main_memory->pages = ((page_mem *) malloc(sizeof(page_mem) * PAGE_AMT));

    swap_mem *swap_memory = (swap_mem *) (malloc(sizeof(swap_mem)));
    swap_memory->p_id = ((int *) malloc(sizeof(int) * SWAP_MEMORY_AMT));
    swap_memory->vir_page = ((int *) malloc(sizeof(int) * SWAP_MEMORY_AMT));
    swap_memory->pages = ((page_mem *) malloc(sizeof(page_mem) * SWAP_MEMORY_AMT));

    page_table *pt = (page_table *) (malloc(sizeof(page_table) * SWAP_MEMORY_AMT));
    
    for(int i = 0; i<SWAP_MEMORY_AMT; i++){
	    swap_memory->p_id[i] = -1;
	    pt[i].p_id = -1;
    }
    for(int i =0; i<PAGE_AMT; i++){
	main_memory->p_id[i] = -1;
    }
    

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
			
		}
		else{

		}
	}

    }
}

//only use for reading/writing
bool accessMemory(page_table *page_tb, main_mem *main_m, int vir_page, char operation) {
    if (!(page_tb->present[vir_page] && page_tb->valid[vir_page]))
        return false;
    if (operation == 'R') {
        //just read the data, no need to do anything with it
        int temp = main_m->pages[page_tb->translations[vir_page]].data;
	main_m->pages[page_tb->translations[vir_page]].accessed = true;
        return true;
    } else if (operation == 'W') {
        main_m->pages[page_tb->translations[vir_page]].data = 1;
        main_m->pages[page_tb->translations[vir_page]].dirty = true;
        return true;
    } else return false;
}

//only use for allocating and deallocate memory
//Allocation: TRUE if allocated, FALSE if no memory OR modified pages left (SWAP MUST BE USED)
//Allocation: SWAP occurs when all pages full but at least one page remains unmodified
//De-allocation: TRUE if freed, FALSE if access was not granted, or not found

//TODO: CHECK PAGE TABLE FOR VALID TRANSLATION

bool allocateMemory(page_table *page_tb, swap_mem *swap_m, main_mem *main_m, int vir_page, char operation) {
    int index = -1;
    if (operation == 'A') {
        for (int i = 0; i < PAGE_AMT; i++) {
            if (main_m->p_id[i] == -1) {
                main_m->p_id[i] = page_tb->p_id;
                main_m->vir_page[i] = vir_page;
                main_m->pages[i].dirty = false;
                main_m->pages[i].data = 0;
                add_translation(page_tb, vir_page, i);
                return true;
            } else if (!(main_m->pages[i].dirty)) {
                index = i;
            }
        }
        if (index > -1) {
            main_to_swap(page_tb, swap_m, main_m, index);
            main_m->p_id[index] = page_tb->p_id;
            main_m->vir_page[index] = vir_page;
            main_m->pages[index].dirty = false;
            main_m->pages[index].data = 0;
            add_translation(page_tb, vir_page, index);
        } else return false;
    } else if (operation == 'F') {
        if(!(page_tb->valid[vir_page]))
            return false;
        for (int i = 0; i < PAGE_AMT; i++) {
            if (main_m->p_id[i] == page_tb->p_id) {
                main_m->p_id[i] = -1;
                main_m->vir_page[i] = -1;
                main_m->pages[i].dirty = false;
                main_m->pages[i].data = 0;
                remove_translation(page_tb,vir_page);
                return true;
            }
        }
        for (int i = 0; i < SWAP_MEMORY_AMT; i++) {
            if (swap_m->p_id[i] == page_tb->p_id) {
                swap_m->p_id[i] = -1;
                swap_m->vir_page[i] = -1;
                swap_m->pages[i].dirty = false;
                swap_m->pages[i].data = 0;
                remove_translation(page_tb,vir_page);
                return true;
            }
        }
        return false;
    }

}

//assumes you checked that all pages are taken first
int unmodified_pages(main_mem *mem ){
	int i = 0;
	for(i; i<20; i++){
		//tests if page has been modified 
		if(mem->pages[i].dirty ==false && mem->pages[i].accessed == false){
			//return page number if page is unmodified 
			return i;
		}

		else{
			return -1;
		}
	}
	
}

void add_translation(page_table *pt,int vir_page, int phys_page){
	pt->translations[vir_page] = phys_page;
	return;
}

void remove_translation(page_table *pt, int vir_page){
	pt->translations[vir_page] = -1;
	return;
}

void main_to_swap(page_table *pt, swap_mem *swap, main_mem *main,int index){
	
	int swap_index;

	//find a free page in swap memory 
	for(int i=0; i<1000; i++){
		if(swap->p_id[i] == -1){
			swap_index = i;
			break;
		}

	}


	//copy contents to swap memory
	swap->p_id[swap_index] = main->p_id[index];
	swap->vir_page[swap_index] = main->vir_page[index];
	swap->pages[swap_index].data = main->pages[index].data;
	swap->pages[swap_index].dirty = main->pages[index].dirty;
	swap->pages[swap_index].accessed = main->pages[index].accessed;

}

void swap_to_main(page_table *pt, swap_mem *swap, main_mem *main, int index){
	int main_index;
	
	//find free page in main memory
	for(int i = 0; i<20; i++){
		if(main->p_id[i] == -1){
			main_index = i;
			break;
		}
	}
	
	//copy contents to main memory
	main->p_id[main_index] = swap->p_id[index];
	main->vir_page[main_index] = swap->vir_page[index];
	main->pages[main_index].data = swap->pages[index].data;
	main->pages[main_index].dirty = swap->pages[index].dirty;
	main->pages[main_index].accessed = swap->pages[index].accessed;

}
