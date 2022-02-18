
#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
#include <time.h>
#include <sys/time.h>

int main(){

/*
	myinit(1);
	void*p1, *p2, *p3, *p4, *p5;
	p1 = mymalloc(100);
	printf("addr of malloc 1 %p\n",p1);

	p2 = mymalloc(50);
	printf("addr of malloc 2 %p\n",p2);
	p3 = mymalloc(150);
	printf("addr of malloc 3 %p\n",p3);
	p4 = mymalloc(10);
	printf("addr of malloc 4 %p\n",p4);
	p5 = mymalloc(60);
	printf("addr of malloc 5 %p\n",p5);
	myfree(p2);
//	myfree(p3);
	printf("malloc of p2 after free\n\n\n");
	printf("addr of %p\n", mymalloc(50));
	myfree(p1);
	printf("malloc after free %p\n",mymalloc(150));
	printf("malloc 6 %p\n",mymalloc(200));
	printf("malloc after free %p\n",mymalloc(150));
	myfree(p2);
	myfree(p4);
	myfree(p5);
	printf("malloc after 3 frees %p\n",mymalloc(70));
	myfree(mymalloc(200));
	printf("malloc after free %p\n",mymalloc(150));
	printf("malloc after free %p\n",mymalloc(150));
	printf("malloc after free %p\n",mymalloc(1000));
	printf("malloc after free %p\n",mymalloc(15000));
	printf("malloc after free %p\n",mymalloc(100050));
	printf("malloc after free %p\n",mymalloc(5000));
*/


	int toMalloc  = 1;
	int toRealloc = 1;
	int todo;
	double througput;
	void* p;
//	void* m;
	time_t before;
	time_t after;


	myinit(0);
	time(&before);
	p = mymalloc(100);
	for(int i = 0; i<1000000; i++){
		srand(time(0));
		todo = rand() % 10 +1;
		if(todo == 2){

//			srand(time(0));
//			toMalloc = rand() % 256 +1;
//			p = mymalloc(toMalloc);
			myfree(p);
//			i++;
		}
		if(todo == 3){
//			srand(time(0));
//			toMalloc = rand() % 256 +1;
//			p = mymalloc(toMalloc);


			srand(time(0));
			toRealloc = rand() % 200 + 100;
			myrealloc(p,toMalloc+10);
//			i++;
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
//	double f = 1;
	p = mymalloc(100);
	for(int i = 0; i<1000000; i++){
		srand(time(0));
		todo = rand() % 10 +1;
		if(todo == 2){

//			srand(time(0));
//			toMalloc = rand() % 256 +1;
//			p = mymalloc(toMalloc);
			myfree(p);
//			i++;
		}
		if(todo == 3){

//			srand(time(0));
//			toMalloc = rand() % 256 +1;
//			p = mymalloc(toMalloc);
			srand(time(0));
			toRealloc = rand() % 200 + 100;
			myrealloc(p,toRealloc);
//			i++;
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
//	double f = 1;
	p = mymalloc(100);
	for(int i = 0; i<1000000; i++){
		srand(time(0));
		todo = rand() % 10 +1;
		if(todo == 2){

//			srand(time(0));
//			toMalloc = rand() % 256 +1;
//			p = mymalloc(toMalloc);
			myfree(p);
//			i++;
		}
		if(todo == 3){

//			srand(time(0));
//			toMalloc = rand() % 256 +1;
//			p = mymalloc(toMalloc);
			srand(time(0));
			toRealloc = rand() % 200 + 100;
			myrealloc(p,toRealloc);
//			i++;
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
