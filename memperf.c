
#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
#include <time.h>
#include <sys/time.h>

int main(){

	int toMalloc  = 1;
	int toRealloc = 1;
	int todo;
	double througput;
	void* p;
	time_t before;
	time_t after;


	myinit(0);
	time(&before);
	p = mymalloc(100);
	for(int i = 0; i<1000000; i++){
		srand(time(0));
		todo = rand() % 10 +1;
		if(todo == 2){
			myfree(p);
		}
		if(todo == 3){
			srand(time(0));
			toRealloc = rand() % 200 + 100;
			myrealloc(p,toMalloc+10);
		}
		else{

			srand(time(0));
			toMalloc = rand() % 256 +1;
			p = mymalloc(toMalloc);
		}
		
	}
	time(&after);

	througput = (1000000)/(difftime(after, before));
	printf("Throughput for first fit: %1.2f ops/sec\n",througput);
	printf("Utilization for first fit: %1.2f\n\n",utilization());

	mycleanup();


	//==========================================================================================

	myinit(1);
	time(&before);
	p = mymalloc(100);
	for(int i = 0; i<1000000; i++){
		srand(time(0));
		todo = rand() % 10 +1;
		if(todo == 2){
			myfree(p);
		}
		if(todo == 3){
			srand(time(0));
			toRealloc = rand() % 200 + 100;
			myrealloc(p,toRealloc);
		}
		else{

			srand(time(0));
			toMalloc = rand() % 256 +1;
			p = mymalloc(toMalloc);
		}
		
	}
	time(&after);

	througput = (1000000)/(difftime(after, before));
	printf("Throughput for next fit: %1.2f ops/sec\n",througput);
	printf("Utilization for next fit: %1.2f\n\n",utilization());

	mycleanup();

	//===============================================================================

	myinit(2);
	time(&before);
	p = mymalloc(100);
	for(int i = 0; i<1000000; i++){
		srand(time(0));
		todo = rand() % 10 +1;
		if(todo == 2){
			myfree(p);
		}
		if(todo == 3){
			srand(time(0));
			toRealloc = rand() % 200 + 100;
			myrealloc(p,toRealloc);
		}
		else{
			srand(time(0));
			toMalloc = rand() % 256 +1;
			p = mymalloc(toMalloc);
		}
	}
	time(&after);

	througput = (1000000)/(difftime(after, before));
	printf("Throughput for best fit: %1.2f ops/sec\n",througput);
	printf("Utilization for best fit: %1.2f\n\n",utilization());

	mycleanup();

}
