#include <stdio.h>

void swap(int a, int b) {
	
	int temp;
	temp = a;
	a = b;
	b = temp;
	
}

void main() {
	int value = 1, list[5] = {2, 4, 6, 8, 10};
	swap(value, list[0]);
	swap(list[0], list[1]);
	swap(value, list[value]);
}


