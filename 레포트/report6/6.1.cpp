#include<iostream>

using namespace std;

int main() {
	
	int *ptr;
	int gasi = 0;

	for (int i = 0; i < 3; i++)
	{
	
		int gasi = 3; 
		ptr = &gasi; 
		cout << "gasi : "<<gasi << "의 주소값 : " << ptr <<endl;
	}

	ptr = &gasi; 
	cout << "gasi : " << gasi << "의 주소값 : " << ptr << endl;

	return 0;
}


