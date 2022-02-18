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
//	uint32_t *footer;
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
//	printf("heapstart %p\n",heapstart);
	heapend = (void*)((void*)heapstart+MB - sizeof(struct block));
//	printf("heapend %p\n",(void*)heapend);
	maxAddress = (void*)heapstart;
	
	initialFull = (void*)mem;
	initialFull-> header = (MB - sizeof(struct block));
	initialFull -> header = initialFull->header & -2;
//	initialFull -> footer = (void*)((void*)initialFull->header + MB - sizeof(uint32_t));
//	initialFull -> footer = (size_t)(initialFull->header);
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
//	size_t *footerFree;
	new->header = toSplit->header - size;
	new -> header = new -> header & -2;
	new->next = toSplit->next;
	toSplit->next->prev = new;
	toSplit->prev->next = new;
	new->prev = toSplit->prev;
//	footerFree = (void*)((void*)new + new->header);
//	*footerFree = (size_t)new->header;
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

//	printf("in malloc loop\n");
	size_t total = size;
	size = ALIGN(size + sizeof(size_t));
	size = (size < MIN_SIZE)? MIN_SIZE : size;

	if(algo == 0){
		size_t *allocated = NULL;
		size_t *footer = NULL;

		struct block *current = initialFull;
		while((void*)current <(void*)heapend || allocated == NULL){
//			printf("start while\n");
			if((current->header&1)==0){
//				printf("inside free\n");
				if(current->header > size){
					allocated = split(current,size);
					if(((void*)allocated+size) <= (void*)heapend){
					footer = ((void*)allocated + size);
					*footer = (int)size|1;
//					printf("addr of allocated in malloc %p\n",allocated);
//					printf("size alloc %d\n",size);
//					printf("addr of footer %p\n",footer);
//					printf("val of footer %d\n",*footer);
					}
					if((void*)allocated > (void*)maxAddress){
						maxAddress = (void*)((void*)allocated+size);
					}
					totalSizeRequested += total;
//					printf("total at end of one malloc %d\n",totalSizeRequested);
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
//						printf("allocated greater than max\n");
						maxAddress = (void*)((void*)allocated+size);
					}
					totalSizeRequested += total;
//					printf("total at end of one malloc %d\n",totalSizeRequested);
					return (void*)((void*)allocated+sizeof(size_t));
				}
				else{
					if(current->next != (void*)heapend && (void*)current->next < (void*)heapend){
					current = current->next;
					}
					else{
//					printf("return null afer 7\n");
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
	//		printf("start for nextFit while\n");
			if((void*)current >= (void*)heapend){
//					printf("break out\n");
					break;
			}
			if((current->header &1)==1){
	//			printf("no empty\n");
				break;
			}
			if((current->header&1)==0){
	//			printf("inside free\n");
				if(current->header > size){
					allocated = split(current,size);
					if(((void*)allocated+size) <= (void*)heapend){
					footer = ((void*)allocated + size);
					*footer = (int)size|1;
//					printf("addr of allocated in malloc %p\n",allocated);
//					printf("size alloc %d\n",size);
//					printf("addr of footer %p\n",footer);
//					printf("val of footer %d\n",*footer);
					}
					if((void*)allocated > (void*)maxAddress){
						maxAddress = (void*)((void*)allocated+size);
					}
			//		if((void*)current->next != (void*)heapend){
			//		nextFitPtr = current->next;
			//		}
					totalSizeRequested += total;
//					printf("total at end of one malloc %d\n",totalSizeRequested);
					return (void*)((void*)allocated+sizeof(size_t));
				}
				else if(current->header == size){
					allocated = (void*)current;
					*allocated = size|1;
					footer = ((void*)allocated + size);
					*footer = (int)size|1;
					current->next->prev = current->prev;
					current->prev->next = current->next;
	//				initialFull = current->next;
					if((void*)allocated > (void*)maxAddress){
//						printf("allocated greater than max\n");
						maxAddress = (void*)((void*)allocated+size);
					}
					if((void*)current->next != (void*)heapend){
					nextFitPtr = current->next;
					}
					totalSizeRequested += total;
//					printf("total at end of one malloc %d\n",totalSizeRequested);
					return (void*)((void*)allocated+sizeof(size_t));
				}
				else{
					if((void*)current->next < (void*)heapend){
					current = current->next;
					}
					else{
					//allocated = NULL;
//					printf("return null afer 7\n");
					return NULL;
					}
				}
			}
		}
		if(allocated == NULL){
	//	printf("in allocated = null\n");
			struct block *current = initialFull;
			while((void*)current <(void*)heapend || allocated == NULL){
	//			printf("start while\n");
				if((current->header&1)==0){
	//				printf("inside free\n");
					if(current->header > size){
						allocated = split(current,size);
						if(((void*)allocated+size) <= (void*)heapend){
						footer = ((void*)allocated + size);
						*footer = (int)size|1;
	//					printf("addr of allocated in malloc %p\n",allocated);
	//					printf("size alloc %d\n",size);
	//					printf("addr of footer %p\n",footer);
	//					printf("val of footer %d\n",*footer);
						}
						if((void*)allocated > (void*)maxAddress){
							maxAddress = (void*)((void*)allocated+size);
						}
						totalSizeRequested += total;
	//					printf("total at end of one malloc %d\n",totalSizeRequested);
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
	//						printf("allocated greater than max\n");
							maxAddress = (void*)((void*)allocated+size);
						}
						totalSizeRequested += total;
	//					printf("total at end of one malloc %d\n",totalSizeRequested);
						return (void*)((void*)allocated+sizeof(size_t));
					}
					else{
						if(current->next != (void*)heapend && (void*)current->next < (void*)heapend){
						current = current->next;
						}
						else{
	//					printf("return null afer 7\n");
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
//	printf("current addre before while %p\n",current);
//	current = (void*)initialFull;
//	printf("before best fit while\n");
//	if((current->header &1) == 0){
//	if((void*)current < (void*)heapstart || (void*)current > (void*)heapend){
//		printf("current before heap\n");
//		exit(1);
//	}
	while((void*)current < (void*)((void*)heapend)){
//		printf("inside while\n");
//	printf("current addr %p\n",current);
//	printf("heap end addr %p\n", heapend);
//	printf("heap start addr %p\n",heapstart);
//	printf("initialFull addr %p\n",initialFull);
			if((current->header > size) && (current->header <= bestHeapSize)){
//				printf("inside while if\n");
				bestFound = current;
				bestHeapSize = current->header;
			}
			if(((void*)current->next < (void*)heapend)){
//			printf("inside increment\n");
			current = current->next;}
			else{
//				printf("inside while else\n");
				break;
			}
//		printf("after stats\n");
	}
//	printf("outside while\n");

	if(bestFound == NULL){
//		printf("bestfound still null\n");
		return NULL;
	}
	else if( bestFound != NULL && bestFound->header > (2*size)){
//		printf("error 2 in split loop\n");
		allocated = split(bestFound,size);
		if(((void*)allocated+size) <= (void*)heapend){
			footer = ((void*)allocated + size);
			*footer = (int)size|1;
		}
		if((void*)allocated > (void*)maxAddress){
			maxAddress = (void*)((void*)allocated+size);
		}
		totalSizeRequested += total;
//			printf("total at end of one malloc %d\n",totalSizeRequested);
		return (void*)((void*)allocated+sizeof(size_t));
	}
	else if(bestFound != NULL && (bestFound->header == size || bestFound->header <= (2*size))){
//		printf("error 3 in no split loop\n");
		allocated = (void*)bestFound;
		*allocated = bestFound->header|1;
		if(((void*)allocated+*allocated) <= (void*)heapend){
			footer = ((void*)allocated + size);
			*footer = (int)size|1;
		}
//		printf("before next prev\n");
		if(bestFound->next != (void*)heapend){
		bestFound->next->prev = bestFound->prev;
		}
		if(bestFound->prev != (void*)heapstart){
		bestFound->prev->next = bestFound->next;
		}
//		printf("after next prev\n");
		bestFound->header = bestFound->header|1;
//		size_t *init = (void*)heapstart;
//		printf("error 3 beofre while\n");
	//	while((*init &1) == 1 && (void*)init < (void*)heapend){
	//		printf("error 3 in while above\n");
	//		init = (void*)((void*)init + *init + sizeof(size_t));
	//		printf("in while after\n");
	//	}
//init = (void*)((void*)init + *init + sizeof(size_t));
	//	initialFull = (struct block*)init;
//		printf("after assignment error 3\n");
//		printf("addr of initialFull %p\n",initialFull);

		if((void*)allocated > (void*)maxAddress){
		//		printf("allocated greater than max\n");
			maxAddress = (void*)((void*)allocated+*allocated);
		}
		totalSizeRequested += total;
//			printf("total at end of one malloc %d\n",totalSizeRequested);
		return (void*)((void*)allocated+sizeof(size_t));
	}
	else{
//		printf("in last else to return null\n");
		return NULL;
	}

	//	return bestfit(size,total);
	//	printf("best fit return %p\n",p);
	}

	return NULL;
}
//=============================================================================

//=============================================================================
void myfree(void* ptr){

//	printf("in free and ptr is %p\n",ptr);
	//ptr = (void*)(ptr) - sizeof(struct allocBlock);

	if((void*)ptr == NULL){
//		printf("in ptr == nULL\n");
		return;
	}
	if(ptr <= (void*)heapstart || ptr >= (void*)(heapend)){
		printf("error: not a heap pointer\n");
		return;
	}
		size_t *ptrHead= (void*)((void*)ptr - sizeof(size_t));
//	size_t ptrHead = (*(size_t*)(ptr - sizeof(size_t)));
//	printf("ptrHead val %d\n",*ptrHead);

	if((*ptrHead & ~1L) == 0){
		printf("error:not a malloced address\n");
		return;
	}
	if((*ptrHead & 1) == 0){
		printf("error: double free\n");
		return;
	}


	else{

//printf("error 1\n");
	size_t *prevAlloc = (void*)((void*)ptr - sizeof(size_t));
	struct block *newFree = (struct block*)((void*)ptr-sizeof(size_t));
	size_t newFreeSize;
//	printf("error 2\n");
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
//	printf("addr of newFree %p\n",newFree);
//	printf("size of new free %d\n",newFree->header);
	struct block* nextC = (void*)((void*)newFree + newFree->header+sizeof(size_t));
//	printf("addr of next block to newFree %p\n",nextC);
	if((nextC->header & 1) == 0 && (void*)nextC < (void*)heapend){

		newFree->header = newFree->header + nextC->header - sizeof(struct block);
		newFree->next = nextC->next;
		newFree->header = newFree->header & -2;
//	printf("addr of newFree after right coles %p\n",newFree);
//	printf("size of newFree after right %d\n",newFree->header);
	}
	
	size_t* prevF = NULL; 
	if((void*)((void*)newFree -sizeof(size_t)) > ((void*)heapstart +sizeof(struct block))){
	prevF= (void*)((void*)newFree -sizeof(size_t));
//	printf("addr of newFree prev block %p\n",prevF);
//	if(prevF == NULL){
	if((*prevF & 1) == 0){

//	printf("val of newFree prev %d\n", *prevF);
	size_t prevBlockSize = *prevF;
//	printf("prevBlockSize %d\n",prevBlockSize);
		newFree = (void*)((void*)prevF - prevBlockSize);
//		printf("addr of new free if prev free %p\n",newFree);
		newFree->header += newFreeSize - sizeof(size_t);
		//newFree->next = nextC->next;
		newFree->header = newFree->header & -2;

//	printf("addr of newFree after left coles %p\n",newFree);
//	printf("size of newFree after left %d\n",newFree->header);
	}
	}
}
//}
//	ptr = NULL;
	//	printf("out of myfree\n");
	totalSizeRequested -= (size_t)(*ptrHead-1);
	//	printf("ptr size %d\n",toFree->header);
	//	printf("free struct %p\n",toFree);
	//	printf("free struct next pointer %p\n",toFree->next);

	//	printf("free struct prev pointer %p\n",toFree->prev);
		
	}
//	ptr = NULL;
	//printf("returned from myfree last loop\n");
	return;

}

//=============================================================================
void *myrealloc(void* ptr, size_t size){
	size_t *header = (void*)((void*)ptr - sizeof(size_t));
	if(ptr == NULL){
//		printf("in ptr = nul \n");
		return mymalloc(size);
	}
	if(size == 0){
//		printf("in size = 0 \n");
		myfree(ptr);
		return NULL;
	}
	if(ptr == NULL && size == 0){
//		printf("in ptr = nul and size = 9\n");
		return NULL;
	}
	if(size <= *header){
		return NULL;
	}

	else{
	
		if((*header & 1) == 0){
//			printf("already free\n");
			return NULL;
		}
//		*header = *header-1;
//		printf("size of header of ptr %ld\n",*header);
		
		size_t *nextHeader = (void*)((void*)header + *header-1 + sizeof(size_t));
		size_t *prevFooter = (void*)((void*)header - sizeof(size_t));
//		printf("error 1\n");
		if(  ( (void*)((void*)header + *header-1 + sizeof(size_t)) >= (void*)heapend && (*prevFooter&1) ==1  )|| ( (void*)((void*)header) == (void*)heapstart && (*nextHeader&1)==1)){
			void* p = mymalloc(size);
			myfree(ptr);
			return p;
//					printf("error 2\n");
		}

//		printf("error 2.3\n");
//		 if(((*nextHeader &1) == 1 && (*prevFooter &1) == 1) || ((*header + *nextHeader - (sizeof(size_t))) < size && (*prevFooter&1)==1) || ((*header + *prevFooter) < size && (*nextHeader&1)==1)){
//			printf("next is allocated\n");
//			void* p = mymalloc(size);
//			myfree(ptr);
//			return p;
//			printf("error 3\n");
//		}
//		printf("error 2.5\n");		
		if((void*)nextHeader < (void*)((void*)heapend - *nextHeader) && (*header + *nextHeader) >= size && (*nextHeader &1) ==0){
//		printf("\n\n in first else\n");
///		printf("addr of next header in realloc %p\n",nextHeader);
//		printf("size of next header in realloc %ld\n",*nextHeader);
			size_t *finalFooter;
			struct block *nextFree = (void*)nextHeader;
//			printf("next block header %ld\n",nextFree->header);
			size_t *footerH = (void*)((void*)ptr + *header-1 -sizeof(size_t));
//			printf("size of footer %ld\n",*footerH);
			size_t *footerN = (void*)((void*)nextFree + nextFree->header);
//			printf("size of footer %ld\n",*footerN);
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
//			if(nextFree->next != (void*)heapend){
			nextFree->next->prev = nextFree->prev;//}
//			if(nextFree->prev != (void*)heapstart){
			nextFree->prev->next = nextFree->next;//}
			nextFree->header = nextFree->header|1;
		//	nextFree = nextFree->next;
			if((void*)initialFull < (void*)nextFree->next){
				initialFull = (struct block*)nextFree->next;
//				printf("initial full addr %p\n",initialFull);
				initialFull->next = init;

			}
			nextFree = NULL;
			return (void*)header + sizeof(size_t);
		}
//		printf("error 2.7\n");
		if((void*)prevFooter >= (void*)((void*)heapstart+*prevFooter)  && (*header + *prevFooter) >= size  && (*prevFooter&1) == 0){
//		printf("\n\n in second else \n");
//		printf("addr of next header in realloc %p\n",nextHeader);
//		printf("size of next header in realloc %d\n",*nextHeader);
			totalSizeRequested -= *header;
			totalSizeRequested += size;
			size_t *finalFooter;
			struct block *prevFree = (void*)((void*)prevFooter - *prevFooter);
//			printf("prev free addr %p\n",prevFree);			
//			printf("prev Block header %d\n",prevFree->header);
			size_t *footerH = (void*)((void*)ptr + *header-1 -sizeof(size_t));
	//		printf("size of footer %d\n",*footerH);
	//		size_t *footerN = (void*)((void*)nextFree + nextFree->header);
	//		printf("size of footer %d\n",*footerN);
			header = (void*)prevFree;
			*header = *header + prevFree->header;
			finalFooter = (void*)((void*)header + *header);
			*finalFooter = *header;
			*footerH = *footerH|1;
	//		*nextHeader = NULL;
			footerH = NULL;
	//		*footerN = NULL;
			prevFree->next->prev = prevFree->prev;
			prevFree->prev->next = prevFree->next;
			prevFree->header = prevFree->header |1;
					struct block *init = initialFull;
			if((void*)initialFull == (void*)prevFree){
				initialFull = (struct block*)prevFree->next;
//				printf("initial full addr %p\n",initialFull);
				initialFull->next = init;

			}

//			prevFree = NULL;
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
//	printf("last block allocated %p\n",last);
//	printf("space total used %d\n",used);
//	printf("totalSizeRequested %d\n",totalSizeRequested);
	if((double)totalSizeRequested > (double)used){
		util = 1;
	}
	else if(totalSizeRequested < 0){
		util = 0;
	}
	else{
	util = (double)totalSizeRequested / (double)used;}
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
