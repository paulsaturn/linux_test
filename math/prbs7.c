/**
 * 	@filename :
 * 		prbs7.c
 * 
 * 	@note :
 * 		用 PRBS-7 产生伪乱数二进制数列的C语言程式 (伪乱数二进制数列（pseudorandomness binary sequence）简称PRBS)
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
 * 		m序列和它的位移序列模2相加(xor)后所得序列仍是该m序列的某个位移序列		
 * 
 * 		自相关autocorrelation: 

 * 				当gap != 0 (mode N)，等于mc, 如本例子为32
 * 				当gap == 0 (mode N), 等于m，如本例子为64
 * 			其自相关函数类似于冲击函数delta(t - kp), p是周期2^n - 1, 如本例n = 7, p = 127
 * 			类似于正态分布的白噪声，delta的傅立叶F为常数，而功率谱等于||F||^2, 其功率谱也为常数
 * 		
 * 		R(j) = Σ　a(i)a(i+j), i = 1, ..., p 		
 * 			二进制中的取值为R(j) = (A - D) / (A + D) = (A - D) / p
 * 			A = [a(i) xor a(i + j) = 0 的数目]， D = [a(i) xor a(i + j) = 1 的数目]
 * 			A - D = -1 (j != 0) 或 p (j = 0)
 * 			当j = 0时， R(j) = 1; 其他时, R(j) = -1/p
 * 			周期函数 : R(j) = R(j + kp)
 * 			偶函数 : R(j) = R(-j)
 * 
 * 		反馈系数8进制，左边高位的0去掉， 对应c0, c1,...cn, 总位数为n + 1, 其中c0 = cn = 1(恒为1), 而c1, ..., cn对应寄存器an-1, ..., a0, 即是bit0, ..., bit(n-1)
 * 	    特征方程f(x) = c0 + c1*x + ... + cn*x^n
 * 
 * 	m序列的应用:	
 * 	  1.扩展频谱通信  C = B lg(1 + S/N)
 * 		信道容量C，信道传输带宽B, 信噪比S/N
 * 		在保持信息传输速率不变的条件下，信噪比和带宽之间具有互换关系，就是说可以用扩展信号的频谱
 * 		换取用更低信噪比来传送信号
 * 		1). 直接序列扩频方式
 * 		数据 -> 伪码 模2加法 -> 调相器 -> 发射    接收 -> 相关器 -> 中频 -> 解调 -> 数据
 * 														  <- 调相器 <- 伪码 模2加法 
 * 		伪码 : 频率更高的数据序列
 * 		如何对准数据 : 相关器, 对准时相关性最(大 or 小) ????
 * 
 * 		2).跳变频率扩频方式
 * 		d(u, t)伪噪声发生器 -> 频率合成器 -> 发射	接收 -> 混频器 -> 中频带痛 -> 解调
 * 														    <- d(u, t)伪噪声发生器 <- 频率合成器
 * 		3).跳变时间扩频方式
 * 		4).混合式扩频方式
 * 
 * 	 2.通信加密
 * 
 **/
/**
 Matlab 或 Octave 的m序列算法
 function [seq]=mseq(coef) 
 %*************************************************** 
 % 此函数用来生成m序列 
 % coef为反馈系数向量 
 % Author: FastestSnail 
 % Date: 2017-10-03 
 %*************************************************** 
 m=length(coef); 
 len=2^m-1; % 得到最终生成的m序列的长度     
 backQ=0; % 对应寄存器运算后的值，放在第一个寄存器 
 seq=zeros(1,len); % 给生成的m序列预分配 (1 x len的数组(矩阵)，索引从1开始)
 registers = [1 zeros(1, m-2) 1]; % 给寄存器分配初始结果 
 for i=1:len 
	seq(i)=registers(m); 
	backQ = mod(sum(coef.*registers) , 2); %特定寄存器的值进行异或运算，即相加后模2 
	registers(2:length(registers)) = registers(1:length(registers)-1); % 移位 
	registers(1)=backQ; % 把异或的值放在第一个寄存器的位置 
end
end
  **/
 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//#define PRBS4
//#define PRBS5
#define PRBS7
//#define PRBS15

#ifdef PRBS4
#define PRBS_BITS		4
#define PRBS_SHIFT0		3
#define PRBS_SHIFT1 	0
#define PRBS_TYPE  		uint8_t
#endif

#ifdef PRBS5
#define PRBS_BITS		5
#define PRBS_SHIFT0		4
#define PRBS_SHIFT1 	1
#define PRBS_TYPE  		uint8_t
#endif

#ifdef PRBS7
#define PRBS_BITS		7
#define PRBS_SHIFT0		6
#define PRBS_SHIFT1 	5
#define PRBS_TYPE  		uint8_t
#endif

#ifdef PRBS15
#define PRBS_BITS		15
#define PRBS_SHIFT0		14
#define PRBS_SHIFT1 	13
#define PRBS_TYPE  		uint16_t
#endif

#define PRBS_CYCLE 		((1<<PRBS_BITS) - 1)
#define PRBS_MASK		(PRBS_TYPE)((1<<(8 * sizeof(PRBS_TYPE))) - 1)		
#define PRBS_M			2
#define PRBS_BITS_MAX	(PRBS_CYCLE * PRBS_M)

#define RAND_IBITS_LEN	(8 * sizeof(PRBS_TYPE))
#define RAND_DATA_LEN	((PRBS_CYCLE + RAND_IBITS_LEN - 1) / RAND_IBITS_LEN)
#define RAND_BITS_LEN	(RAND_DATA_LEN * RAND_IBITS_LEN)

#define SHOW_DATA_MAX	128

#define min(a, b)		((a > b) ? b : a)
#define max(a, b)		((a > b) ? a : b)

/*
 *  autocorrelation
 */
float acf(int *datas, int max, int gap, float mean)
{
	float ac = 0;
	int i;
	
	for (i = 0; i < max; i++) {
		//ac += (datas[i] - mean) * (datas[(i + gap) % mod] - mean);
		ac += (datas[i] - mean) * (datas[i + gap] - mean);
		//ac += (datas[i]) * (datas[i + gap]);
	}
		
	return ac / (max - 1);
}

/*
 * 	cross correlation
 */
float PRBS_ccf(int *ins1, int *ins2, int max)
{
	float ac = 0;
	int i;
	
	for (i = 0; i < max; i++) {
		if ((ins1[i]) ^ (ins2[i])) {
			ac--;
		} else {
			ac++;
		}
	}
		
	return ac / max;
}

/*
 * 
 */
float PRBS_acf(int *datas, int max, int gap)
{
	float ac = 0;
	int i;
	
	for (i = 0; i < max; i++) {
		if (datas[i] ^ datas[i + gap]) {
			ac--;
		} else {
			ac++;
		}
	}
	
	return ac / max;
}

/*
 * 
 */
float PRBS_mean(int *datas, int max)
{
	float sum = 0;
	int i;
	
	for (i = 0; i < max; i++) {
		sum += datas[i];
	} 
	
	return sum / max;
}

/*
 * 
 */
int PRBS_sum(int *datas, int max)
{
	int sum = 0;
	int i;
	
	for (i = 0; i < max; i++) {
		sum += datas[i];
	}
	
	return sum;
}

/*
 * 
 */
void datas_printf(int *datas, int max)
{
	int i; 
    
    printf("\n");
    for (i = 0; i < max; i++) {
		printf("%d, ", datas[i]);
		if (i % 16 == 15) {
			printf("\n");
		}
	}
    printf("\n");	
}

/*
 * 
 */
int PRBS_xor(int *bits, int max, int delay)
{
	int new_bits[max];
	int i;
	
	for (i = 0; i < max; i++) {
		new_bits[i] = (bits[i] ^ bits[i + delay]);
	}

#if 1
	//check new_bits offset
	for (i = 0; i < max; i++) {
		int j;
		for (j = 0; j < max; j++) {
			if (bits[i + j] != new_bits[j]) {
				break;
			}
		}
		
		if (j == max) {
			printf("\nnew_bits offset = %d\n", i);
			#if 0
			datas_printf(new_bits, max);
			datas_printf(bits + i, max);
			#endif
			break;
		}
	}
#endif 
	
	return PRBS_sum(new_bits, max);
}

/*
 * 
 */
void PRBS_xor2(int *bits, int *ins, int *outs, int max, int delay)
{
	int i;
	
	for (i = 0; i < max; i++) {
		outs[i] = (ins[i] ^ bits[i + delay]);
	}
}

/*
 * 
 */
void check_random_data(int *datas)
{
	int i, j;
	int equal_cnt = 0;
	
	printf("\n=========== datas analyze ===========\n");
	//通过PBRS产生了相对随机数
	for (i = 0; i < PRBS_CYCLE; i++) {
		for (j = 0; j < PRBS_CYCLE; j++) {
			if (datas[i] == datas[j] && i != j) {
				printf("data equal [%d] = [%d], %d\n", i, j, datas[i]);
				equal_cnt++;
			}
		}
	}
	printf("equal_cnt = %d\n", equal_cnt);
	
}

/*
 * 
 */
void check_data_encode_decode(int *bits)
{
	int i, j;
	int rand_bits[RAND_BITS_LEN];
	int new_bits[RAND_BITS_LEN];
	int renew_bits[RAND_BITS_LEN];
	float min_cc = 1.0;
	int min_index = 0;
	int not_equal_cnt = 0;
			
	//random data bits, xor encode, xor decode
	printf("\n=========== rand bits encode and decode ===========\n");
	j = 0;
	//printf("sizeof(rand_bits) = %ld\n", sizeof(rand_bits));
	memset(rand_bits, 0, sizeof(rand_bits));
	for (i = 0; i < RAND_DATA_LEN; i++) {
		PRBS_TYPE rand_data = rand();
		int k;

		for (k = 0; k < RAND_IBITS_LEN; k++) {
			if (rand_data & 1) {
				rand_bits[j] = 1;
			} 
			j++;
			rand_data >>= 1;
		}
	}
			
	datas_printf(rand_bits, PRBS_CYCLE);
	
	//对rand_bits用bits模2加法
	for (i = 0; i < PRBS_CYCLE; i++) {
		new_bits[i] = rand_bits[i] ^ bits[i];
	}
	datas_printf(new_bits, PRBS_CYCLE);
	
	//相关性判断
	for (i = 1; i < (PRBS_CYCLE + 1); i++) {
		float cc = PRBS_ccf(bits + i, new_bits, PRBS_CYCLE);
		//printf("[%d]ccf = %f\n", i, cc);
		if (cc < min_cc) {
			min_cc = cc;
			min_index = i;
		}
	}
	printf("[%d] min_cc = %f\n", min_index, min_cc);
	//对new_bits用bits模2加法
	PRBS_xor2(bits, new_bits, renew_bits, PRBS_CYCLE, min_index);
	/* half are not same */ 
	//PRBS_xor2(bits, new_bits, renew_bits, PRBS_CYCLE, 2);
	for (i = 0; i < PRBS_CYCLE; i++) {
		if (rand_bits[i] != renew_bits[i]) {
			//printf("error : %d\n", i);
			not_equal_cnt++;
		}
	}
	printf("not equal cnt = %d\n", not_equal_cnt);
	
}

/*
 * 
 */
int main(int argc, char* argv[]) {
    PRBS_TYPE start = 1;	/* 不能全0 或 全1 */
    PRBS_TYPE a = start;
    int bits[PRBS_BITS_MAX];
    int datas[PRBS_BITS_MAX];
    float last_ac = 0;
    float mean, variance;
    int i, j;
    
    printf("PRBS_MASK = 0x%x, PRBS_BITS = %d, PRBS_M = %d\n", PRBS_MASK, PRBS_BITS, PRBS_M);
    
    j = 0;
    for(i = 1;; i++) {
        PRBS_TYPE newbit = (((a >> PRBS_SHIFT0) ^ (a >> PRBS_SHIFT1)) & 1);
        if (newbit == 1) {
			j++;
		}
		bits[i - 1] = newbit;
        a = ((a << 1) | newbit) & PRBS_MASK;
		datas[i - 1] = a;
        //printf("%x\n", a);
        //if (a == start) {
        if (i == PRBS_BITS_MAX) {
            printf("start = 0x%x, repetition period is %d; j=%d, rate=%d\n", start, i/PRBS_M, j/PRBS_M, (i/PRBS_M-1)/(j/PRBS_M-1));
            break;
        }
    }

	datas_printf(bits, min(SHOW_DATA_MAX, PRBS_CYCLE));
    datas_printf(bits + PRBS_CYCLE, min(SHOW_DATA_MAX, PRBS_CYCLE));

	printf("\n=========== bits analyze ===========\n");
	//only output 64 no matter what delay sets
	//half bits are same
	printf("delay xor = %d\n", PRBS_xor(bits, PRBS_CYCLE, 4));
	printf("delay xor = %d\n", PRBS_xor(bits, PRBS_CYCLE, 7));
	
	mean = PRBS_mean(bits, PRBS_CYCLE);
	variance = PRBS_acf(bits, PRBS_CYCLE, 0);
    printf("mean = %f, variance = %f\n", mean, variance);
	
    //autocorrelation
    for (i = 1; i < PRBS_CYCLE + 1; i++) {
		float ac = PRBS_acf(bits, PRBS_CYCLE, i);
		if (ac != last_ac) {
			last_ac = ac;
			printf("[0x%02x]PRBS_acf = %f\n", i, last_ac / variance);
		}
	}

#if 0	
	variance = acf(bits, PRBS_CYCLE, 0, mean);
	printf("mean = %f, normal variance = %f\n", mean, variance);
	last_ac = 0;
    for (i = 1; i < PRBS_CYCLE + 1; i++) {
		float ac = acf(bits, PRBS_CYCLE, i, mean);
		if (ac != last_ac) {
			last_ac = ac;
			printf("[0x%02x]normal_acf = %f\n", i, last_ac / variance);
		}
	}
#endif

#if 1
	check_data_encode_decode(bits);
#endif

#if 1
	check_random_data(datas);
#endif
}
