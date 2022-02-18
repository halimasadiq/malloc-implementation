#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//=============================================================================
#define MB 1048576
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
#define MIN_SIZE 32
void *bestfit(size_t size, size_t total);
//=============================================================================
struct block{

	uint32_t header;
	struct block *next;
	struct block *prev;
};

char* mem;
struct block *initialFull;
static char *heapstart;
static int algo;
static char *heapend;
int totalSizeRequested;
void* maxAddress;
struct block* nextFitPtr;
//=============================================================================
void myinit(int allocAlg){
	mem = malloc(MB);
	algo = allocAlg;

	heapstart = (void*)(mem);
	heapend = (void*)((void*)heapstart+MB - sizeof(struct block));
	maxAddress = (void*)heapstart;
	
	initialFull = (void*)mem;
	initialFull-> header = (MB - sizeof(struct block));
	initialFull -> header = initialFull->header & -2;
	initialFull -> next = (void*)heapend;
	initialFull -> prev = (void*)heapstart;
	nextFitPtr = initialFull;

}
//=============================================================================
void *split(struct block *toSplit, size_t size){


	size_t *alloc = (void*)toSplit;
	struct block *new = (void*)((void*)toSplit+size+sizeof(size_t*));//+sizeof(struct allocBlock));
	if((void*) new >= (void*)heapend){
		*(alloc) = (size_t)size|1;
	}
	else{
	new->header = toSplit->header - size;
	new -> header = new -> header & -2;
	new->next = toSplit->next;
	toSplit->next->prev = new;
	toSplit->prev->next = new;
	new->prev = toSplit->prev;
	if(algo == 1){
		nextFitPtr = new;
	}
	
	initialFull = new;
	*(alloc) = (size_t)size|1;
	}
	return alloc;
}

//=============================================================================
void* mymalloc(size_t size){

	size_t total = size;
	size = ALIGN(size + sizeof(size_t));
	size = (size < MIN_SIZE)? MIN_SIZE : size;

	if(algo == 0){
		size_t *allocated = NULL;
		size_t *footer = NULL;

		struct block *current = initialFull;
		while((void*)current <(void*)heapend || allocated == NULL){
			if((current->header&1)==0){
				if(current->header > size){
					allocated = split(current,size);
					if(((void*)allocated+size) <= (void*)heapend){
					footer = ((void*)allocated + size);
					*footer = (int)size|1;
					}
					if((void*)allocated > (void*)maxAddress){
						maxAddress = (void*)((void*)allocated+size);
					}
					totalSizeRequested += total;
					return (void*)((void*)allocated+sizeof(size_t));
				}
				else if(current->header == size){
					allocated = (void*)current;
					*allocated = size|1;
					footer = ((void*)allocated + size);
					*footer = (int)size|1;
					current->next->prev = current->prev;
					current->prev->next = current->next;
					initialFull = current->next;
					if((void*)allocated > (void*)maxAddress){
						maxAddress = (void*)((void*)allocated+size);
					}
					totalSizeRequested += total;
					return (void*)((void*)allocated+sizeof(size_t));
				}
				else{
					if(current->next != (void*)heapend && (void*)current->next < (void*)heapend){
					current = current->next;
					}
					else{
					return NULL;
					}
				}
			}
			else{
				return NULL;
			}
		}
		if(allocated == NULL){
			return NULL;
		}

	}
	
	//====================================================================================================
	if(algo == 1){
		size_t *allocated = NULL;
		size_t *footer = NULL;

		struct block *current = nextFitPtr;
		while((void*)current < ((void*)heapend)|| allocated == NULL){
			if((void*)current >= (void*)heapend){
					break;
			}
			if((current->header &1)==1){
				break;
			}
			if((current->header&1)==0){
				if(current->header > size){
					allocated = split(current,size);
					if(((void*)allocated+size) <= (void*)heapend){
					footer = ((void*)allocated + size);
					*footer = (int)size|1;
					}
					if((void*)allocated > (void*)maxAddress){
						maxAddress = (void*)((void*)allocated+size);
					}
					totalSizeRequested += total;
					return (void*)((void*)allocated+sizeof(size_t));
				}
				else if(current->header == size){
					allocated = (void*)current;
					*allocated = size|1;
					footer = ((void*)allocated + size);
					*footer = (int)size|1;
					current->next->prev = current->prev;
					current->prev->next = current->next;
					if((void*)allocated > (void*)maxAddress){
						maxAddress = (void*)((void*)allocated+size);
					}
					if((void*)current->next != (void*)heapend){
					nextFitPtr = current->next;
					}
					totalSizeRequested += total;
					return (void*)((void*)allocated+sizeof(size_t));
				}
				else{
					if((void*)current->next < (void*)heapend){
					current = current->next;
					}
					else{
					return NULL;
					}
				}
			}
		}
		if(allocated == NULL){
			struct block *current = initialFull;
			while((void*)current <(void*)heapend || allocated == NULL){
				if((current->header&1)==0){
					if(current->header > size){
						allocated = split(current,size);
						if(((void*)allocated+size) <= (void*)heapend){
						footer = ((void*)allocated + size);
						*footer = (int)size|1;
						}
						if((void*)allocated > (void*)maxAddress){
							maxAddress = (void*)((void*)allocated+size);
						}
						totalSizeRequested += total;
						return (void*)((void*)allocated+sizeof(size_t));
					}
					else if(current->header == size){
						allocated = (void*)current;
						*allocated = size|1;
						footer = ((void*)allocated + size);
						*footer = (int)size|1;
						current->next->prev = current->prev;
						current->prev->next = current->next;
						initialFull = current->next;
						if((void*)allocated > (void*)maxAddress){
							maxAddress = (void*)((void*)allocated+size);
						}
						totalSizeRequested += total;
						return (void*)((void*)allocated+sizeof(size_t));
					}
					else{
						if(current->next != (void*)heapend && (void*)current->next < (void*)heapend){
						current = current->next;
						}
						else{
						return NULL;
						}
					}
				}
				else{
					return NULL;
				}

			}

		}

	}
	//====================================================================================================
	if(algo == 2){	
	struct block *bestFound = NULL;
	size_t *allocated = NULL;
	size_t *footer = NULL;
	size_t bestHeapSize = MB - sizeof(struct block);
	struct block *current = initialFull;
	while((void*)current < (void*)((void*)heapend)){
			if((current->header > size) && (current->header <= bestHeapSize)){
				bestFound = current;
				bestHeapSize = current->header;
			}
			if(((void*)current->next < (void*)heapend)){
			current = current->next;}
			else{
				break;
			}
	}

	if(bestFound == NULL){
		return NULL;
	}
	else if( bestFound != NULL && bestFound->header > (2*size)){
		allocated = split(bestFound,size);
		if(((void*)allocated+size) <= (void*)heapend){
			footer = ((void*)allocated + size);
			*footer = (int)size|1;
		}
		if((void*)allocated > (void*)maxAddress){
			maxAddress = (void*)((void*)allocated+size);
		}
		totalSizeRequested += total;
		return (void*)((void*)allocated+sizeof(size_t));
	}
	else if(bestFound != NULL && (bestFound->header == size || bestFound->header <= (2*size))){
		allocated = (void*)bestFound;
		*allocated = bestFound->header|1;
		if(((void*)allocated+*allocated) <= (void*)heapend){
			footer = ((void*)allocated + size);
			*footer = (int)size|1;
		}
		if(bestFound->next != (void*)heapend){
		bestFound->next->prev = bestFound->prev;
		}
		if(bestFound->prev != (void*)heapstart){
		bestFound->prev->next = bestFound->next;
		}
		bestFound->header = bestFound->header|1;

		if((void*)allocated > (void*)maxAddress){
			maxAddress = (void*)((void*)allocated+*allocated);
		}
		totalSizeRequested += total;
		return (void*)((void*)allocated+sizeof(size_t));
	}
	else{
		return NULL;
	}
	}

	return NULL;
}
//=============================================================================

//=============================================================================
void myfree(void* ptr){

	if((void*)ptr == NULL){
		return;
	}
	if(ptr <= (void*)heapstart || ptr >= (void*)(heapend)){
		printf("error: not a heap pointer\n");
		return;
	}
	size_t *ptrHead= (void*)((void*)ptr - sizeof(size_t));
	if((*ptrHead & ~1L) == 0){
		printf("error:not a malloced address\n");
		return;
	}
	if((*ptrHead & 1) == 0){
		printf("error: double free\n");
		return;
	}


	else{

	size_t *prevAlloc = (void*)((void*)ptr - sizeof(size_t));
	struct block *newFree = (struct block*)((void*)ptr-sizeof(size_t));
	size_t newFreeSize;
	newFree->header = *prevAlloc-1;
	newFree->header = newFree->header & -2;
	struct block *prevBlock;
	struct block *nextBlock;
	if((void*)((void*)newFree + *ptrHead) < (void*)heapend){
	nextBlock = (void*)heapstart;

	while(nextBlock != NULL && nextBlock < newFree){
		prevBlock = nextBlock;
		nextBlock = nextBlock->next;
	}
	newFree->next = nextBlock;
	newFree->prev = prevBlock;
	
	if(newFree->next != (void*)heapend){
	newFree->next->prev = newFree;}
	
	newFree->prev->next = newFree;
	struct block* init = initialFull;
	if((void*)newFree < (void*)initialFull){
		initialFull = newFree;
		initialFull ->next = init;
	}
	if((void*) ((void*)newFree + newFree->header) < (void*)heapend){
	size_t *footer;
	footer = ((void*)newFree + newFree->header);
	*footer = (int)newFree->header & -2;
	}
	newFreeSize = newFree->header;
	struct block* nextC = (void*)((void*)newFree + newFree->header+sizeof(size_t));
	if((nextC->header & 1) == 0 && (void*)nextC < (void*)heapend){

		newFree->header = newFree->header + nextC->header - sizeof(struct block);
		newFree->next = nextC->next;
		newFree->header = newFree->header & -2;
	}
	
	size_t* prevF = NULL; 
	if((void*)((void*)newFree -sizeof(size_t)) > ((void*)heapstart +sizeof(struct block))){
	prevF= (void*)((void*)newFree -sizeof(size_t));
	if((*prevF & 1) == 0){
	size_t prevBlockSize = *prevF;
		newFree = (void*)((void*)prevF - prevBlockSize);
		newFree->header += newFreeSize - sizeof(size_t);
		newFree->header = newFree->header & -2;
	}
	}
	}
	totalSizeRequested -= (size_t)(*ptrHead-1);
		
	}
	return;

}

//=============================================================================
void *myrealloc(void* ptr, size_t size){
	size_t *header = (void*)((void*)ptr - sizeof(size_t));
	if(ptr == NULL){
		return mymalloc(size);
	}
	if(size == 0){
		myfree(ptr);
		return NULL;
	}
	if(ptr == NULL && size == 0){
		return NULL;
	}
	if(size <= *header){
		return NULL;
	}

	else{
	
		if((*header & 1) == 0){
			return NULL;
		}
		
		size_t *nextHeader = (void*)((void*)header + *header-1 + sizeof(size_t));
		size_t *prevFooter = (void*)((void*)header - sizeof(size_t));
		if(  ( (void*)((void*)header + *header-1 + sizeof(size_t)) >= (void*)heapend && (*prevFooter&1) ==1  )|| ( (void*)((void*)header) == (void*)heapstart && (*nextHeader&1)==1)){
			void* p = mymalloc(size);
			myfree(ptr);
			return p;
		}

		if((void*)nextHeader < (void*)((void*)heapend - *nextHeader) && (*header + *nextHeader) >= size && (*nextHeader &1) ==0){
			size_t *finalFooter;
			struct block *nextFree = (void*)nextHeader;
			size_t *footerH = (void*)((void*)ptr + *header-1 -sizeof(size_t));
			size_t *footerN = (void*)((void*)nextFree + nextFree->header);
			totalSizeRequested -= *header;
			*header = *header + nextFree->header;
			*header = *header|1;
			totalSizeRequested += size;
			finalFooter = (void*)((void*)header + *header);
			*finalFooter = *header;
			*finalFooter = *finalFooter |1;
			struct block *init = initialFull;
			
			*nextHeader = *nextHeader |1;
			*footerH = *footerH|1;
			*footerN = *footerN|1;
			nextHeader = NULL;
			footerH = NULL;
			footerN = NULL;
			nextFree->next->prev = nextFree->prev;
			nextFree->prev->next = nextFree->next;
			nextFree->header = nextFree->header|1;
			if((void*)initialFull < (void*)nextFree->next){
				initialFull = (struct block*)nextFree->next;
				initialFull->next = init;

			}
			nextFree = NULL;
			return (void*)header + sizeof(size_t);
		}
		if((void*)prevFooter >= (void*)((void*)heapstart+*prevFooter)  && (*header + *prevFooter) >= size  && (*prevFooter&1) == 0){
			totalSizeRequested -= *header;
			totalSizeRequested += size;
			size_t *finalFooter;
			struct block *prevFree = (void*)((void*)prevFooter - *prevFooter);
			size_t *footerH = (void*)((void*)ptr + *header-1 -sizeof(size_t));
			header = (void*)prevFree;
			*header = *header + prevFree->header;
			finalFooter = (void*)((void*)header + *header);
			*finalFooter = *header;
			*footerH = *footerH|1;
			footerH = NULL;
			prevFree->next->prev = prevFree->prev;
			prevFree->prev->next = prevFree->next;
			prevFree->header = prevFree->header |1;
					struct block *init = initialFull;
			if((void*)initialFull == (void*)prevFree){
				initialFull = (struct block*)prevFree->next;
				initialFull->next = init;

			}
			return (void*)header + sizeof(size_t);
			
			
		}
		else{
			void* p = mymalloc(size);
			myfree(ptr);
			return p;
		}
	}//end main else
	return NULL;
}//end myrealloc

//=============================================================================
double utilization(){

	double util;
	void *last = maxAddress;
	int used = (void*)last - (void*)heapstart;
	util = (double)totalSizeRequested / (double)used;
	return util;
}
//================================================================================
void mycleanup(){

	free(mem);
	algo = 0;
	maxAddress =NULL;
	heapstart = NULL;
	heapend = NULL;
	totalSizeRequested = 0;	
	initialFull = NULL;
	nextFitPtr = NULL;
}
//==================================================================================
