/*
 * whitenoise.cxx
 * 
 * Copyright 2016 Paul <wu_bao@163.com>
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

/**************************
用乘同余法产生均匀分布白噪声（一串随机序列）。算法及程序实现叙述如下。
1) 在函数之外设定随机种子，随机序列为sequence，长度 n
2)  取 M =2^35，x = (A*x)mod M;e[i]= x/M ; 循环n次，得到均匀分布白噪声序列e[].该伪随机数的循环周期为2^(35-2)。
3) 实际实现中，将 e[i]= x/M ; 替换为 取ei的 小数部分 再赋值给ei＋1，循环n次，就得到均匀分布白噪声序列；
*************************/
void whiteNoise(vector<double> &sequence,int n)
{
    int x = rand();
    int A = 3125;
    double M = pow(2.0f,35.0f);
    sequence.resize(n);
    double seed =double(x/M);
    sequence[0]= ( A * seed - int( A * seed ));//取小数部分
    double average = 0,serror=0; //统计均值、方差
    for (int i=1;i<n;++i)
    {
        sequence[i]=( A * sequence[i-1] - int( A * sequence[i-1] ));//取小数部分，获得均匀分布白噪声序列
        average+=sequence[i];
    }
    average/=n;//得到均值
    for(int i=0;i<n;++i)
    {
        serror +=(sequence[i]-average)*(sequence[i]-average);
    }
    serror/=n;//得到方差

    
    counter1.resize(pieceNum);//用vecotr<double> counter 进行概率统计，共分为 pieceNum 份 进行统计
    int max=0;
    for(int i=0;i<n;++i)
    {
        int index = sequence[i]/double(10.0f/pieceNum)+pieceNum/2;
        counter1[index]++;
        if(counter1[index] > max)
            max=counter1[index];
    }
    for(int i=0;i<counter1.size();++i)
    {
        counter1[i]/=(double)max;    //归一化
    }
}

/*************************************
 用迭代取中法产生均匀分布白噪声
***************************************/
void MakeWhiteNoise(vector<double> &sequence,int  n)
{        
        sequence.resize(n);
        for(int  j = 0; j < n;  j ++)
        {    
            int  r = rand();
            r = (  (r & 0xff) + ( (r & 0xff00) >> 8 )  ) & 0xff;//截取后8位
            sequence[j] = (unsigned char) r/(double)0xff;//归一化
        }

    counter1.resize(pieceNum);//用vecotr<double> counter 进行概率统计，共分为 pieceNum 份 进行统计
    int max=0;
    for(int i=0;i<n;++i)
    {
        int index = sequence[i]/double(10.0f/pieceNum)+pieceNum/2;
        counter1[index]++;
        if(counter1[index] > max)
            max=counter1[index];
    }
    for(int i=0;i<counter1.size();++i)
    {
        counter1[i]/=(double)max;    //归一化
    }
}

/*************************
    先生成两个（0,1）间随机白噪声序列 sq1,sq2，再利用公式: c[i]=serror*(–2*log sq1[i])^0.5*cos(2*pi*sq2[i]) ＋average 循环n次
   计算得到均值和方差可任意调整的白噪声序列。
*************************/
void guassWhiteNoise(vector<double> &vdGuassSequence,int n,double average,double serror)
{
        //根据均值和方差得到调整后的白噪声序列
    vector<double> sq1,sq2;
    whiteNoise(sq1,n); //使用乘同余法
    whiteNoise(sq2,n);
    /*MakeWhiteNoise(sq1,n); //或使用迭代取中法
    MakeWhiteNoise(sq2,n);*/

    vdGuassSequence.resize(n);
    double average2 = 0,serror2=0;//均值、方差
    for (int i=0;i<n;++i)
    {
        if(sq1[i]!=0)
            vdGuassSequence[i]= serror* sqrt((-2) *log(sq1[i])) * sin( 2*PI* sq2[i])  +average;
        else
            vdGuassSequence[i]= serror* sqrt((-2) *log(0.0000001)) * sin( 2*PI* sq2[i])  +average;
        average2+=vdGuassSequence[i];
    }
    average2/=n;//得到均值
    for(int i=0;i<n;++i)
    {
        serror2 +=(vdGuassSequence[i]-average2)*(vdGuassSequence[i]-average2);
    }
    serror2/=n;//得到方差

    counter2.resize(pieceNum+1);//用vecotr<double> counter2 进行概率统计，共分为 pieceNum 份 进行统计
    int max=0;
    for(int i=0;i<n;++i)
    {
        int index = vdGuassSequence[i]/double(20.0f/pieceNum)+pieceNum/2;
        counter2[index]++;
        if(counter2[index] > max)
            max=counter2[index];
    }
    for(int i=0;i<counter2.size();++i)
    {
        counter2[i]/=(double)max;    //归一化
    }
}

int main(int argc, char **argv)
{
	
	return 0;
}

