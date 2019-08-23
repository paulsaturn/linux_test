/**
 * @filename : test_new_this.cpp
 * 
 * @nate :
 * 
 **/
#include <iostream>
#include <string>

using namespace std; 
 
class A {
public:
    A() : m_str("") {
		cout << "1" << __FUNCTION__ << "()" << endl;
    }
    
    A(string str) {
		cout << "2" << __FUNCTION__ << "()" << endl;
		m_str = str;
    }
        
    ~A() {
		cout << __FUNCTION__ << endl;
	}
    
    void recall(string str) {
	   this->~A();
	   /*
	   * It constructs an object of type T without allocating memory for it, in the address specified in the parentheses.
	   * So what you're looking at is a copy assignment operator which first destroys the object being copied to (without freeing the memory), 
	   * and then constructs a new one in the same memory address.
       */
       new (this) A(str);
    }
    
    string get(void) {
		return m_str;
	}
    
private :
	string m_str;
};

int main(void)
{
	A a;
	
	cout << "get1: " << a.get() << endl;
	a.recall("hello");
	cout << "get2: " << a.get() << endl;
	
	return 0;
}
