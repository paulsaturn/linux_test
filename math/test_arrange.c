/*
==========================================================
题目：将4个红球，3个白球，3个黄球排成一排，共有多少种排法？球无编号！
==========================================================
==========================================================
评：排列(permutation)组合(combination)题目，关键是要理解排列和组合的含义，知道其公式:
m(总数) > n(其中一部分)
A(m,n)=m!/(m-n)!
C(m,n)=m!/(m-n)!/n!
==========================================================
*/
#include <stdio.h>

int factorial(int n)
{
	if (n==0||n==1) {
		return 1;
	} else {
		return n*factorial(n-1);
	}
}

int assemble(int m,int n)
{
	return factorial(m)/factorial(m-n)/factorial(n);
}

main()
{
	int k,h;
	
	k = assemble(10,4)*assemble(6,3)*assemble(3,3);
	printf("共有%d种排法！\n",k);
}
