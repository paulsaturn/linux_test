#include <stdio.h>
class Test { 
private: 
	//variable of class
	static int cCount; 
public: 
	Test() { cCount++; } 
	~Test() { --cCount; } 

	//method of class
	static int GetCount() { return cCount; } 
}; 

//must be initialized first for private or public static variable
int Test::cCount = 0; 

int main() {

	printf("count = %d\n", Test::GetCount()); 
	
	Test t1; 
	Test t2; 

	printf("count = %d\n", t1.GetCount()); 
	printf("count = %d\n", t2.GetCount()); 

	Test* pt = new Test(); 
	printf("count = %d\n", pt->GetCount()); 
	delete pt; 

	printf("count = %d\n", Test::GetCount()); 

	return 0; 
}
