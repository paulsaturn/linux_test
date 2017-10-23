/*
 * chinese_remainder.cpp
 * 
 * Copyright 2017 Paul <wu_bao@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/*
 *  56 mod 9 = 2 mod 9
 *  56 * x = 1 mod 9  => 2 * x = 1 mod 9 => x = 5
 *  63 mod 8 = 7 mod 8
 *  63 * x = 1 mod 8 => 7 * x = 1 mod 8 => x = 7
 *  72 mod 7 = 2 mod 7
 *  72 * x = 1 mod 7 => 2 * x = 1 mod 7 => x = 4
 */


#include <iostream>  

using namespace std;  

struct t {
    int d, x, y, z;  
}s;  


struct t extended_eucild(int a, int b)  
{  
    if (b == 0) {
        s.d = a;  
        s.x = 1;  
        s.y = 0;  
        
        return s;  
    } else {
        struct t ss = extended_eucild(b, a % b);  
        
        s.d = ss.d;  
        s.x = ss.y;  
        s.y = ss.x - (a / b) * ss.y;  
        
        return s;  
    }  
}  


// ax = b (mod n)
int  MODULAR_LINEAR_EQUATION_SOLVER(int a,int b,int n)  
{  
    struct t ss = extended_eucild(a,n);  
    if (b % ss.d == 0) {  
        int x = (ss.x * (b / ss.d)) % (n);//100k-105  
        if (x < 0) {
            x += n;  
        }  

        return x;  
    } else {
        cout<<"no solutions"<<endl;  
        return -1;  
    }  
}  


void Chinese_remainder_theorem(int m[],int a[], int num)  
{  
    int s = 1;  
    
    for (int i = 0; i < num; i++) {
      s *= m[i];  
    }  
    cout << "n = " << s << endl;
    
    int M[num] = {0},MM[num] = {0};  
    for (int j = 0; j < num; j++) {
       M[j] = s / m[j];  										//Mi
    }  
    cout << "Mi = ";
    for (int j = 0; j < num; j++) {
		cout << M[j] << ", ";
	}
	cout << endl;
	
    for (int k = 0; k < num; k++) {
       MM[k] = MODULAR_LINEAR_EQUATION_SOLVER(M[k], 1, m[k]);  	//x = Mi-1 (mod ni) => Mi * x = 1 (mod ni)
    }  
    cout << "Mi-1 mod ni = ";
    for (int j = 0; j < num; j++) {
		cout << MM[j] << ", ";
	}
	cout << endl;
	    
    cout << "x = ";  
    int sum = 0;  
    int i = 0;  
    while (i != num) {
        sum += MM[i] * M[i] * a[i];  
        i++;  
    }  

    cout << sum % s << "(mod" << s << ")" << endl;  
}


void demo_1(void)
{
    int a[] = {1, 2, 3};//输入余数  
    int m[] = {9, 8, 7};//输入除数 

    int num = sizeof(a) / sizeof(int);

    cout << "======= demo_1 ======" << endl;
    cout << "ni = ";
    for (int j = 0; j < num; j++) {
		cout << m[j] << ", ";
	}
	cout << endl;       
    cout << "ai = ";
    for (int j = 0; j < num; j++) {
		cout << a[j] << ", ";
	}
	cout << endl;
    Chinese_remainder_theorem(m, a, num);  	
}


void demo_2(void)
{
    int a[]={4, 5};  
    int m[]={5, 11};  

    int num = sizeof(a) / sizeof(int);

    cout << "======= demo_2 ======" << endl;
    cout << "ni = ";
    for (int j = 0; j < num; j++) {
		cout << m[j] << ", ";
	}
	cout << endl;       
    cout << "ai = ";
    for (int j = 0; j < num; j++) {
		cout << a[j] << ", ";
	}
	cout << endl;
    Chinese_remainder_theorem(m, a, num);  	
}


int main()  
{  

	demo_1();
	demo_2(); 
    
    return 0;
}

