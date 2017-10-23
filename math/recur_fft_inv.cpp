/*
 * recur_fft_inv.cpp
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


#include <iostream>
#include <stdio.h>  
#include <math.h>  
#include <vector>  

using namespace std;  

#define pi 3.1415926535   

struct  Complex  
{  
    double real;  
    double imag;  
    int num;  
    Complex(int i=0,int j=0){real=i,imag=j;}  
};  

Complex operator+ (Complex const & lhs, Complex const & rhs)  
{  
    Complex temp;  
    temp.real =lhs.real+rhs.real;  
    temp.imag = lhs.imag+ rhs.imag;  
    return temp;  
} 

Complex operator- (Complex const & lhs, Complex const & rhs)  
{  
    Complex temp;  
    temp.real = lhs.real - rhs.real;  
    temp.imag = lhs.imag - rhs.imag;  
    return temp;  
}
  
Complex operator* (Complex const & lhs, Complex const & rhs)  
{  
    Complex temp;  
    temp.real = lhs.real * rhs.real - lhs.imag * rhs.imag;  
    temp.imag= lhs.real * rhs.imag + lhs.imag * rhs.real;  
    return temp;  
} 
 
Complex operator* ( const int &lhs, Complex const & rhs)  
{  
    Complex temp;  
    temp.real=lhs*rhs.real;  
    temp.imag=lhs*rhs.imag;  
    return temp;  
}  

istream &operator>>(istream&is, Complex &item)  
{  
    is >> item.real>>item.imag;  
    return is;  
}  

vector<Complex> recursive_FFT_opposite(vector<Complex> const&a)  
{  
    size_t n = a.size();  
    if (n == 1) return a;  
    struct  Complex wn, w;  
    wn.real = cos(-2 * pi / n), wn.imag = sin(-2 * pi / n);  
    w.real = 1, w.imag = 0;  
    vector<Complex>a0, a1;  
    a0.reserve(n / 2);  
    a1.reserve(n / 2);  
    for (int i = 0; i<n / 2; i++)  
    {  
        a0.push_back(a[i * 2]);  
        a1.push_back(a[i * 2 + 1]);  
    }  
    vector<Complex>y0 = recursive_FFT_opposite(a0);  
    vector<Complex>y1 = recursive_FFT_opposite(a1);  
    vector<Complex>y;  
    y.resize(n);  
    for (int k = 0; k<n / 2; k++)  
    {  
        y[k] = y0[k] + w*y1[k];   
        y[k + n / 2] = y0[k] - w*y1[k];  
        w = w*wn;  
    }  
    return y;  
}  

int main()  
{  
    cout << "请输入需要计算的多项式最高次数，注意必须是2的幂" << endl;  
    int j = 0;  
    cin >> j;  
    const int n = j;  
    vector<Complex>a;  
    cout << "请输入多项式A各项系数(注意系数为0的项要补0):";  
    int  k = 0;  
    Complex i;  
    while (k != n&&cin >> i)  
    {  
        a.push_back(i);  
        k++;  
    }  
    vector<Complex>y = recursive_FFT_opposite(a);  
    for (int t = 0; t<y.size(); t++)  
    {  
		if (fabs(y[t].real) < 0.001) {
			printf("0 ");
		} else {
			printf("%g ", y[t].real/n);//求DFT逆需要把结果÷n  
		}
        if (fabs(y[t].imag) < 0.001)//虚数小于0.001，那么虚数忽略不计  
        {  
            cout << endl;  
            continue;  
        }  
        else  
        {  
            if (y[t].imag<0) printf("%gi", y[t].imag/n);  
            else printf("+%gi", y[t].imag/n);  
        }  
        cout << endl;  
    }  
    
    return 0;
}

