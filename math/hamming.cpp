/**
 * 	@filename : hamming.c
 * 		汉明码具有一位纠错能力。
 * 		如设接收到的错误汉明码0100111
 *		P1=①位+③位+⑤位+⑦位，即P1=0+0+1+1=0
 *		P2=②位+③位+⑥位+⑦位，即P2=1+0+1+1=1
 *		P4=④位+⑤位+⑥位+⑦位，即P4=0+1+1+1=1
 *  	根据P4P2P1构成的二进制是110，将110转换成十进制为6，说明是第6位出错
 * 
 *  	汉明码属于分组奇偶校验，P4P2P1=000，说明接收方生成的校验位和收到的校验位相同，否则不同说明出错。
 *		由于分组时校验位只参加一组奇偶校验(1, 2, 4)，有效信息参加至少两组奇偶校验，
 *		若果校验位出错，P4P2P1的某一位将为1，刚好对应位号4、2、1；
 *		若果有效信息出错，将引起P4P2P1中至少两位为1，如B1(⑤)出错，将使P4P1均为1，P2=0, P4P2P1=101
 * 
 * 	@note :
 * 	  ./bin/hamming
 * 		输入01序列回车
		Source: 10101
		Dest: 001101011
		----input error index : 8
		Code 001101001
		Error index 8
		Corret source: 10101
		Corret dest: 001101011
		Source: 1001010101010101010111111001101
		Dest: 1111001101010100101010101111110101101
		----input error index : 20
		Code 1111001101010100101110101111110101101
		Error index 20
		Corret source: 1001010101010101010111111001101
		Corret dest: 1111001101010100101010101111110101101
		Source: 1
		Dest: 111
		----input error index : 0
		Code 111
		Source: 1
		Dest: 111
		^C
 * 
 **/
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

int cal(int sz)
{
    int k = 0;
    int cur = 1;
    while (cur - 1 < sz + k ) {
        cur <<= 1;
        k++;
    }   
    
    return k;
}

bool encode(const string &s, string &d)
{
    int k = cal(s.size());
    int i, j, p;

    d.clear();
    d.resize(s.size() + k);
    /* 创建一个汉明码编码的空间, 填入数据，并留出校验码位（校验位先设为0）*/
    for (i = 0, j = 0, p = 0; i != d.size(); i++) {
        if ((i + 1) == pow(2,p) && p < k) {
            d[i] = '0';
            p++;
        } else if (s[j] == '0' || s[j] == '1')
            d[i] = s[j++];
        else 
            return false;
    }
    /* 对相应位进行异或操作获得校验码位，并填充相应位置 */
    for (i = 0; i != k; i++) {
        int count = 0, index = 1 << i;
        /* j += index : 跳过2^(k-1)位*/
        for (j = index - 1; j < d.size(); j += index)
			/* k=0; k != index: 异或连续2^(k-1)位 */
            for (k = 0; k != index && j < d.size(); k++, j++)
                count ^= d[j] - '0';
        d[index - 1] = '0' + count;
    }
    
    return true;
}

int antiCal(int sz)
{
    int k = 0;
    int cur = 1;
    while (cur < sz) {
        cur <<= 1;
        k++;
    }   
    return k;
}

int decode(string &s, string &d)
{
    int k = antiCal(d.size());
    int i, j, p;
    
    s.clear();
    s.resize(d.size() - k);

    int sum = 0;
    for (p = 0; p != k; p++) {
        int pAnti = 0;
        int index = 1 << p;

        for (i = index - 1; i < d.size(); i += index) {
            for (j = 0; j < index && i < d.size(); i++, j++)
                pAnti ^= d[i] - '0';
        }
        sum += pAnti << p;
    }
    /* 不等于0, sum位有错, 取反sum位 */
    if (sum != 0)
        d[sum - 1] = (1- (int)(d[sum - 1] - '0')) + '0';

    for (i = 0, p = 0, j = 0; i != d.size(); i++) {
        if ((i + 1) == (1 << p) && p < k)
            p++;
        else
            s[j++] = d[i];
    }

    return sum;
}

int main()
{
    string source, dest;
    while (cin >> source) {
        if (encode(source, dest)) {
            cout << "Source: " <<source << endl;
            cout << "Dest: " << dest << endl;
        }
        size_t index;
        cout << "----input error index : ";
        cin >> index;
        int k = dest.size();
        //取反相应位
        if (index != 0 && index <= dest.size())
            dest[index - 1] = (1 - (int)(dest[index - 1] - '0')) + '0';
        cout << "Code " << dest <<endl;
        int ret = decode(source,dest);
        if (ret == 0) {
            cout << "Source: " <<source << endl;
            cout << "Dest: " <<dest << endl;
        } else {
            cout << "Error index "<< ret  << endl;
            cout << "Corret source: " <<source << endl;
            cout << "Corret dest: " <<dest << endl;
        }
        cout << endl;
    }
    return 0;
}
