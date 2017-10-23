/**
 *  @filename :
 *      test_vector.h
 **/
#include <vector>
#include <iostream>

using namespace std;

int main(void)
{
    vector<int> v;
    
    cout <<"size = "<<v.size()<<endl;
    cout <<"max_size = "<<v.max_size()<<endl;
    cout <<"capacity = "<<v.capacity()<<endl;
    v.push_back(1);
    cout << "====" << endl;
    cout << v.capacity() << endl;  // 1
    cout << v.size() << endl;      // 1
    v.push_back(2);
    cout << "====" << endl;
    cout << v.capacity() << endl;  // 2
    cout << v.size() << endl;      // 2
    v.push_back(3);
    cout << "====" << endl;
    cout << v.capacity() << endl; // 4    
    cout << v.size() << endl;     // 3
}
