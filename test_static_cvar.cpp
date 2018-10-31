#include <stdio.h> 

class Test { 
private: 
	//variable of class
	// static int cCount; 
public: 
	//variable of class
	static int cCount; 

	Test() { cCount++; } 
	~Test() { --cCount; } 
	int getCount() { return cCount; } 
}; 

//must be initialized first for private or public static variable
int Test::cCount = 0; 

Test gTest; 

int main() { 
	Test t1; 
	Test t2; 
	
	printf("count = %d\n", gTest.getCount()); 
	printf("count = %d\n", t1.getCount()); 
	printf("count = %d\n", t2.getCount()); 
	printf("Test::cCount = %d\n", Test::cCount);

	Test* pt = new Test(); 
	printf("count = %d\n", pt->getCount()); 
	delete pt; 

	printf("count = %d\n", gTest.getCount()); 
	return 0; 
}