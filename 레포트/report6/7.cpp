#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 100000

int *p;

void staticD() { //정적 선언
	static int a[SIZE];
}

void stackD() { //스택상에 선언
	int a[SIZE];
}

void heapD() { //힙상에 선언
	p = (int*)malloc(sizeof(int)*SIZE);
}

int main() {
	float gap = 0;
	time_t startTime = 0, endTime = 0;

	// 정적  

	startTime = clock();

	for (int i = 0; i < SIZE; i++) {
		staticD();
	}
 
	endTime = clock();
	gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);

	printf("정적 선언 측정 결과 > %f 초\n", gap);

	// 스택 

	startTime = clock();

	for (int i = 0; i < SIZE; i++) {
		stackD();
	}

	endTime = clock();
	gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);

	printf("스택 선언 측정 결과 > %f 초\n", gap);

	// 힙 

	startTime = clock();

	for (int i = 0; i < SIZE; i++) {
		heapD();
	}

	endTime = clock();
	gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);

	printf("힙 선언 측정 결과 > %f 초\n", gap);
}

