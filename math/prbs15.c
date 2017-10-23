/**
 * 	@filename :
 * 		prbs15.c
 * 
 * 	@note :
 * 		用 PRBS-15 产生伪乱数二进制数列的C语言程式 (伪乱数二进制数列（pseudorandomness binary sequence）简称PRBS)
 * 
 *		PRBS7 = x^7 + x^6 + 1
 *		PRBS15 = x^15 + x^14 + 1
 *		PRBS23 = x^23 + x^18 + 1
 *		PRBS31 = x^31 + x^28 + 1
 * 
 * 		占空比c = (m - 1)/(N - 1) = 0.5
 * 			(N - 1)/(m - 1) = 2
 * 
 * 		N = 2^k - 1 : 二进制数列的长度, PRBS-15的N = 2^15 - 1 = 32768 - 1 = 32767
 * 		m　: 二进制数列中出现1的次数
 * 		
 *      1). start = 0 : i = 1, j = 0, a = 0
 *      2). start = 0x01, 而不能满足a == start (此时的a == 0x8001), 如果改为 (uint16_t)(a & 0x7fff) == start 就可以成立
 * 
 **/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
    
int main(int argc, char* argv[]) {
    uint16_t start = 0x02;
    uint16_t a = start;
    int i, j;    
    
    if (argc > 1) {
		start = (uint16_t)atoi(argv[1]);
	}
    
    j = 0;
    for(i = 1;; i++) {
        uint16_t newbit = (((a >> 14) ^ (a >> 13)) & 1);
        if (newbit == 0x01) {
			j++;
		}
        a = ((a << 1) | newbit) & 0xffff;
        printf("%x\n", a);
    #if 0    
        if (i == 32767) {
            printf("start=0x%x, i=%d, j=%d, rate=%d\n", start, i, j, (i-1)/(j-1));
			getchar();
		}
	#endif	
        if (a == start) {
            printf("start = 0x%x, repetition period is %d; j=%d, rate=%d\n", start, i, j, (i-1)/(j-1));
            break;
        }
    }
}
