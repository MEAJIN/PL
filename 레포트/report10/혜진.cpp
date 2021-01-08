#include <iostream>

using namespace std;

int q(int a) {
	cout << "q" << endl;
	return a;
}

int w(int b) {
	cout << "wd" << endl;
	return b;
}

void main() {
	int a = 2, b = 3;
	int i = q(a) + w(b);

	cout << "result : " << i << endl;
}

