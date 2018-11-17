/**
 * 	@filename :
 * 		prbs7.c
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
 * 		自相关autocorrelation: 
 * 				当gap != 0 (mode N)，等于mc, 如本例子为32
 * 				当gap == 0 (mode N), 等于m，如本例子为64
 * 		
 *      1). start = 0 : i = 1, j = 0, a = 0
 *      2). start = 0x01, 而不能满足a == start (此时的a == 0x8001), 如果改为 (uint16_t)(a & 0x7fff) == start 就可以成立
 * 
 **/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PRBS_SHIFT0		6
#define PRBS_SHIFT1 	5
#define PRBS_TYPE  		uint8_t
#define PRBS_MAX		0xff
#define PRBS_CYCLE 		127
#define PRBS_D			2
#define PRBS_BITS_MAX	(PRBS_CYCLE * PRBS_D)

/*
 * 
 */
uint32_t autocorrelation(PRBS_TYPE *bits, int max, int gap)
{
	uint32_t ac = 0;
	int i;
	
	for (i = 0; i < max; i++) {
		//ac += bits[i] * bits[(i + gap) % mod];
		ac += bits[i] * bits[i + gap];
	}
	
	return ac;
}

/*
 * 
 */
int main(int argc, char* argv[]) {
    PRBS_TYPE start = 2;
    PRBS_TYPE a = start;
    PRBS_TYPE bits[PRBS_BITS_MAX];
    int i, j;
    uint32_t last_ac = 0;
    
    j = 0;
    for(i = 1;; i++) {
        PRBS_TYPE newbit = (((a >> PRBS_SHIFT0) ^ (a >> PRBS_SHIFT1)) & 1);
        if (newbit == 1) {
			j++;
		}
		bits[i - 1] = newbit;
        a = ((a << 1) | newbit) & PRBS_MAX;
        //printf("%x\n", a);
        //if (a == start) {
        if (i == PRBS_BITS_MAX) {
            printf("start = 0x%x, repetition period is %d; j=%d, rate=%d\n", start, i/PRBS_D, j/PRBS_D, (i/PRBS_D-1)/(j/PRBS_D-1));
            break;
        }
    }

 #if 1
    for (i = 0; i < PRBS_CYCLE; i++) {
		printf("%d, ", bits[i]);
		if (i % 16 == 15) {
			printf("\n");
		}
	}
    printf("\n");
    
    printf("\n");
    for (; i < PRBS_BITS_MAX; i++) {
		printf("%d, ", bits[i]);
		if ((i + 1) % 16 == 15) {
			printf("\n");
		}
	}
    printf("\n");    
#endif    

    //autocorrelation
    for (i = 1; i < PRBS_CYCLE + 1; i++) {
		uint32_t ac = autocorrelation(bits, PRBS_CYCLE, i);
		if (ac != last_ac) {
			last_ac = ac;
			printf("[0x%02x]ac = %d\n", i, last_ac);
		}
	}
}
