#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 100000

int *p;

void staticD() { //���� ����
	static int a[SIZE];
}

void stackD() { //���û� ����
	int a[SIZE];
}

void heapD() { //���� ����
	p = (int*)malloc(sizeof(int)*SIZE);
}

int main() {
	float gap = 0;
	time_t startTime = 0, endTime = 0;

	// ����  

	startTime = clock();

	for (int i = 0; i < SIZE; i++) {
		staticD();
	}
 
	endTime = clock();
	gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);

	printf("���� ���� ���� ��� > %f ��\n", gap);

	// ���� 

	startTime = clock();

	for (int i = 0; i < SIZE; i++) {
		stackD();
	}

	endTime = clock();
	gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);

	printf("���� ���� ���� ��� > %f ��\n", gap);

	// �� 

	startTime = clock();

	for (int i = 0; i < SIZE; i++) {
		heapD();
	}

	endTime = clock();
	gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);

	printf("�� ���� ���� ��� > %f ��\n", gap);
}

