/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <algorithm>
#include <vector>
#include <utility>
#include <cmath>

#include "Utility.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
int utility::pow(const int& N, const int& M)
{
    int rt = 1;
    for(int i = 0; i < M; i++)
        rt *= N;
    return rt;
}

void utility::Factorize235(const int& N, int Pow[4])
{
    //Pow[]を初期化
    Pow[0] = 0;
    Pow[1] = 0;
    Pow[2] = 0;
    Pow[3] = N;

    //2で何回割れるか？
    while(Pow[3]%2 == 0)
    {
        Pow[3] /= 2;
        Pow[0] += 1;
    }

    //3で何回割れるか？
    while(Pow[3]%3 == 0)
    {
        Pow[3] /= 3;
        Pow[1] += 1;
    }

    //5で何回割れるか？
    while(Pow[3]%5 == 0)
    {
        Pow[3] /= 5;
        Pow[2] += 1;
    }

    //Pow[3]は余り
}

void utility::Factorize(const int& argN, std::vector<std::pair<int, int> >* Factor)
{
    //念のためFactorを空にする
    Factor->clear();

    //Nが0以下なら何もしない
    if(argN <= 0)
    {
        return;
    }

    //Nが正の数ならまず1を代入
    Factor->push_back(std::pair<int, int>(1, 1));

    //Nが1の時は終了
    if(argN == 1)
    {
        return;
    }

    int N = argN;

    //2で何回割れるか？
    int pow2 = 0;
    while(N%2 == 0)
    {
        N /= 2;
        pow2++;
    }
    if(pow2 > 0)
    {
        Factor->push_back(std::pair<int, int>(2, pow2));
    }
    if(N == 1)
    {
        return;
    }

    //3以上sqrt(N)以下の奇数の因数を探す
    for(int i = 3; i <= std::sqrt((double)N); i += 2)
    {
        int pow = 0;

        while(N%i == 0)
        {
            N /= i;
            pow++;
        }
        if(pow > 0)
        {
            Factor->push_back(std::pair<int, int>(i, pow));
        }
        if(N == 1)
        {
            return;
        }
    }

    //余りがあれば追加して終了
    Factor->push_back(std::pair<int, int>(N, 1));
}

void utility::DivideLine1D(std::vector<REAL_TYPE>* rt, const int& num_points, const REAL_TYPE& point1, const REAL_TYPE& point2)
{
    if(num_points == 1)
    {
        rt->push_back(point1);
    }else{
        for(int i = 0; i < num_points; i++)
        {
            if(point1 != point2)
            {
                rt->push_back(point1+(point2-point1)/(num_points-1)*i);
            }else{
                rt->push_back(point1);
            }
        }
    }
    std::sort(rt->begin(), rt->end());
}

void utility::DetermineBlockSize(int* arg_NB, int* arg_MB, int* arg_KB, const int& MaxPoints, const int& N, const int& M, const int& K)
{
    int& NB = *arg_NB;
    int& MB = *arg_MB;
    int& KB = *arg_KB;

    std::vector<std::pair<int, int> > Factor;
    Factorize(MaxPoints, &Factor);
    NB = 1;
    MB = 1;
    KB = 1;
    for(std::vector<std::pair<int, int> >::iterator it = Factor.begin(); it != Factor.end(); ++it)
    {
        NB *= pow(it->first, (it->second)/3);
        MB *= pow(it->first, (it->second)/3);
        KB *= pow(it->first, (it->second)/3);
        if((it->second)%3 != 0)
        {
            if(NB <= MB && NB <= KB)
            {
                NB *= pow(it->first, (it->second)%3);
            }else if(MB <= KB){
                MB *= pow(it->first, (it->second)%3);
            }else{
                KB *= pow(it->first, (it->second)%3);
            }
        }
    }

    LPT::LPT_LOG::GetInstance()->LOG("initial NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("initial MB = ", MB);
    LPT::LPT_LOG::GetInstance()->LOG("initial KB = ", KB);

    // NB,MB,KBがそれぞれN,M,Kを越えてたらN,M,Kに置き換える
    if(NB > N)NB = N;
    if(MB > M)MB = M;
    if(KB > K)KB = K;

    //もしNB, MB, KBを大きくしても条件を満たせるなら大きくする
    while((NB+1)*MB*KB <= MaxPoints && NB+1 <= N)
        ++NB;
    while(NB*(MB+1)*KB <= MaxPoints && MB+1 <= M)
        ++MB;
    while(NB*MB*(KB+1) <= MaxPoints && KB+1 <= K)
        ++KB;

    LPT::LPT_LOG::GetInstance()->LOG("NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("MB = ", MB);
    LPT::LPT_LOG::GetInstance()->LOG("KB = ", KB);
}
} // namespace PPlib