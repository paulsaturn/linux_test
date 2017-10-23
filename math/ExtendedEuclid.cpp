/*
 * @filename :
 * 		ExtendedEuclid.cpp
 * 
 */

/**
  定理1:假设d = gcd(a , m)，假设对于整数xx和yy有 d = a * xx + m * yy。若d|b（d能够整出b），
	那么方程 a * x ≡ b (mod m)有一个解X满足式子 X = xx * (b/d) % m ，其中xx可以用扩展欧几里得算法获得。 

  证明1：
	假设上述成立那么有 a * X ≡ a * xx * (b/d)(mod m),
	由于a * xx = d - m * yy, 则a * xx ≡ d (mod m),
	所以可得到 a * X ≡ d * (b/d)(mod m) ≡ b(mod m)。

  定理2：对于a * x ≡ b (mod m)，若有解则必有d = gcd( a, m)个解。其解为 res = (X + i * (m/d)) (mod m)，其中X为同余方程的一个解(通过定理1求出)，i的取值范围为 0 <= i < d。

  证明2：
	若定理2成立则有 a * res (mod m) = a * (X + i * (m/d)) (mod m) 
									= (a * X + a* i * (m/d)) (mod m)  (因为d|a, aim/d是m的倍数)
									= a * X (mod m) = b
	
**/
 

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;  

long long ExtendedEuclid(long long a,long long b,long long& d,long long& x,long long& y)
{
    long long tmp;
    if(b == 0)
    {
        x = 1;
        y = 0;
        d = a;
    }else
    {
        ExtendedEuclid(b,a%b,d,x,y);
        tmp = x;
        x = y;
        y = tmp - (a / b) * y;
    }
}

void RemainderEquation(long long a,long long b,long long m)
{
    long long X,Y,d;
    long long res;
    ExtendedEuclid(a,m,d,X,Y);

	printf("a = %lld, b = %lld, m = %lld, d = %lld, X = %lld\n", a, b, m, d, X);
    if(b%d == 0)
    {
        X = X * (b / d) % m;//得到同于方程一解
        for(int i = 0 ; i < d; i++)
        {
            res = (X + (i * (m/d))) % m;
            printf("%lld\n",res);
        }
    }else
    {
        printf("No Sulutions!\n");
    }
}

int main(int argc, char **argv)
{
	if (argc < 4) {
		printf("ExtendedEuclid a b m\n");
		return -1;
	}
	
	RemainderEquation(atoll(argv[1]), atoll(argv[2]), atoll(argv[3]));
	
	return 0;
}

