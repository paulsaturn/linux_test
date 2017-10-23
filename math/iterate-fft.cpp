/*
 * iterate-fft.cxx
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

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>  
#include <cmath>  
#include <vector>  

using namespace std;  

#define pi 3.1415926535   

#define VERBOSE_REVERSE 0x01
#define VERBOSE_FFT 	0x02

struct Complex  
{  
    double imag;  
    double real;  
    Complex(double r=0,double i=0){imag=i,real=r;}  
};
  
vector<Complex> a;  

static int g_verbose_flags = 0;

static void output_array(vector<Complex> &A);

Complex operator* (Complex const & lhs, Complex const & rhs)  
{  
    Complex temp;  
    temp.real = lhs.real * rhs.real - lhs.imag * rhs.imag;  
    temp.imag= lhs.real * rhs.imag + lhs.imag * rhs.real;  
    return temp;  
}  

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

int rev(int k)  
{  
    int temp=0,i=0;  
    static int flag=0;  
    size_t n=a.size()-1;  
    if (!flag){while(n>>i)i++;flag=i;}//设置哨兵为了只调用一次这个循环  
    int j=flag;  
    vector<int>b(n+1,0);  
    while (j>0)  
    {  
        //b[--j]=(k-((k>>1)<<1));  
        b[--j]=k%2;  
        k>>=1;  
        temp+=b[j]<<j;  
    }  
    return temp;  
}  

vector<Complex> BIT_REVERSE_COPY(vector<Complex>&a,vector<Complex>&A)  
{  
    size_t n=a.size();  
    for (size_t k=0;k<n;k++)  
    {  
        A[rev(k)]=a[k];  
    }  
    return A;  
}  

int Log(int n)//由于n是2的幂，所以函数返回值一定是整数  
{  
     int temp=0;  
     while (n>>=1) temp++;  
     return temp;  
}
  
void ITERATIVE_FFT(vector<Complex>&a,vector<Complex>&A)  
{  
    BIT_REVERSE_COPY(a,A);  
    
    size_t n=a.size();  
    for (int s=1;s<=Log(n);s++)  
    {  
        int m=pow(2,s);  
        for (int k=0;k<=n-1;k+=m)  
        {  
            Complex w(1,0),wm(cos(2*pi/m),sin(2*pi/m)),t,u;  
            for (int j=0;j<=m/2-1;j++)  
            {  
                t=w*A[k+j+m/2];  
                u=A[k+j];  
                A[k+j]=u+t;  
                A[k+j+m/2]=u-t;  
                w=w*wm;  
            }  
        }  
        
        if (g_verbose_flags & VERBOSE_FFT) {
	        cout << "=============== " << s << " ===============" << endl;
	        output_array(A);
		}
    }  
    return;
}  

void ITERATIVE_FFT2(vector<Complex>&a,vector<Complex>&A)  
{  
    BIT_REVERSE_COPY(a,A);  
    
    size_t n=a.size();  
    for (int s=1;s<=Log(n);s++)  
    {  
        int m=pow(2,s);  
        for (int k=0;k<=n-1;k+=m)  
        {  
            Complex w(1,0),wm(cos(-2*pi/m),sin(-2*pi/m)),t,u;	
            for (int j=0;j<=m/2-1;j++)  
            {  
                t=w*A[k+j+m/2];  
                u=A[k+j];  
                A[k+j]=u+t;  
                A[k+j+m/2]=u-t;  
                w=w*wm;  
            }  
        }  
        
        if (g_verbose_flags & VERBOSE_FFT) {
	        cout << "=============== " << s << " ===============" << endl;
	        output_array(A);
		}
    }  
    return;
} 

void ITERATIVE_FFT_INV(vector<Complex>&a,vector<Complex>&A)  
{  
    BIT_REVERSE_COPY(a,A);  
    
    size_t n=a.size();  
    for (int s=1;s<=Log(n);s++)  
    {  
        int m=pow(2,s);  
        for (int k=0;k<=n-1;k+=m)  
        {  
            Complex w(1,0),wm(cos(-2*pi/m),sin(-2*pi/m)),t,u;  
            for (int j=0;j<=m/2-1;j++)  
            {  
                t=w*A[k+j+m/2];  
                u=A[k+j];  
                A[k+j]=u+t;  
                A[k+j+m/2]=u-t;  
                w=w*wm;  
            }  
        }  
        
        if (g_verbose_flags & VERBOSE_FFT) {
			cout << "=============== " << s << " ===============" << endl;
			output_array(A);
		}
    }  
    
    for (int i=0; i<n; i++) {
		A[i].real /= n;
		A[i].imag /= n;
	}
	
    return;
} 

void ITERATIVE_FFT_INV2(vector<Complex>&a,vector<Complex>&A)  
{  
	ITERATIVE_FFT(a, A);
	
	size_t n=a.size();  
    for (int i=0; i<n; i++) {
		A[i].real /= n;
		A[i].imag /= n;
	}	
}

static void output_array(vector<Complex> &A)
{
    for (int t = 0; t<A.size(); t++) {  
		if (fabs(A[t].real) < 0.001) {
	        printf("%g", 0.0);  
		} else {
	        printf("%g", A[t].real);  
		}
        if (fabs(A[t].imag) < 0.001) { //虚数小于0.001，那么虚数忽略不计  
            cout << endl;  
            continue;  
        } else { 
            if (A[t].imag < 0) 
				printf("%gi", A[t].imag);  
            else 
				printf("+%gi", A[t].imag);  
        }  
        cout << endl;  
    }  	
}

/* 0, 2, 3, -1, 4, 5, 7, 9 */
/*
 FFT和FFT_INV中对应的wm是共轭，才能恢复到原来序列.
 */
int main(int argc, char *argv[])  
{  
	Complex x,y;  
	int i = 0;  
	int j = 0;
	int num;
	
	cout << "argc = " << argc << endl;
	if (argc < 2) {
		cout << "cmd : iterate-fft num_coefficient" << endl;
		return -1;
	}
	
	num = atoi(argv[1]);
	if (argc > 2) {
		g_verbose_flags = atoi(argv[2]);
	}
	cout << "Input real coefficient:" << endl;
	//输入复系数
	while (cin>>x.real) {  
		x.imag = 0;
		a.push_back(x);  
		i++;  
		if (i >= num) {
			break;
		}
	}  
	vector<Complex>A(i,y);  
	vector<Complex>B(i,y);
	vector<Complex>C(i,y);
	vector<Complex>b(i,y);		
		
	BIT_REVERSE_COPY(a,A);  
	if (g_verbose_flags & VERBOSE_REVERSE) {
		cout << "Output bit reverse copy :" << endl;
		output_array(A);
	}
	
	ITERATIVE_FFT(a,A);  
	cout << endl;
	cout << "Output FFT coefficient:" << endl;
	output_array(A);
	ITERATIVE_FFT_INV(A, b);
	cout << endl;
	cout << "Output FFT_INV coefficient:" << endl;
	output_array(b);

	ITERATIVE_FFT2(a,C);  
	cout << endl;
	cout << "Output FFT2 coefficient:" << endl;
	output_array(C);
	ITERATIVE_FFT_INV2(C, b);
	cout << endl;
	cout << "Output FFT_INV2 coefficient:" << endl;
	output_array(b);

	//共轭 conjugate
	for (j=0; j<i; j++) {
		B[j].real = A[j].real;
		B[j].imag = -A[j].imag;
	}
			
	ITERATIVE_FFT(B, b);
	cout << endl;
	cout << "Output FFT_INV B coefficient:" << endl;
	//共轭 conjugate, 并除以n	
	for (j=0; j<i; j++) {
		b[j].real = b[j].real/i;
		b[j].imag = -b[j].imag/i;
	}	
	output_array(b);
	    
    return 0;
}  

