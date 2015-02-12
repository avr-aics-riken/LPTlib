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

void utility::Factorize(int& N, std::vector<std::pair<int, int> >* Factor)
{
    //念のためFactorを空にする
    Factor->clear();
    //Nが0以下なら何もしない
    if(N <= 0)
    {
        return;
    }
    //Nが1の時は1x1をFactorに入れて終了
    if(N == 1)
    {
        Factor->push_back(std::pair<int, int>(1, 1));

        return;
    }
    //2で何回割れるか？
    int pow2 = 0;

    while(N%2 != 0)
    {
        N /= 2;
        pow2++;
    }
    Factor->push_back(std::pair<int, int>(2, pow2));

    if(N == 1)
    {
        return;
    }
    //3以上の奇数の因数を探す
    for(int i = 3; i <= std::sqrt((double)N); i += 2)
    {
        int pow = 0;

        while(N%i != 0)
        {
            N /= i;
            pow++;
        }
        Factor->push_back(std::pair<int, int>(i, pow));

        //因数分解が完了してたら終了
        if(N == 1)
        {
            return;
        }
    }
    //残りの素数があれば、Factorに追加して終了
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
    //MaxNumStartPointsを (2^Pow[0] * 3^Pow[1] * 5^Pow[2] * Rem) の形に因数分解
    int  Pow[4];
    Factorize235(MaxPoints, Pow);
    int& RemNumStartPoints = Pow[3];

    //分割後の小領域の初期値を決める
    NB = pow(2, (Pow[0]/3+Pow[0]%3))*pow(3, (Pow[1]/3))*pow(5, (Pow[2]/3))*RemNumStartPoints;
    MB = pow(2, (Pow[0]/3))*pow(3, (Pow[1]/3+Pow[1]%3))*pow(5, (Pow[2]/3));
    KB = pow(2, (Pow[0]/3))*pow(3, (Pow[1]/3))*pow(5, (Pow[2]/3+Pow[2]%3));

    LPT::LPT_LOG::GetInstance()->LOG("initial NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("initial MB = ", MB);
    LPT::LPT_LOG::GetInstance()->LOG("initial KB = ", KB);

    // NB,MB,KBがそれぞれN,M,K以下になるように値を調整する
    while(NB > N || MB > M || KB > K)
    {
        // NB>Nの時はなるべく小さい因数で割って、MBに同じ値を掛ける
        if(NB > N)
        {
            if(NB%2 == 0)
            {
                NB /= 2;
                MB *= 2;
            }else if(NB%3 == 0){
                NB /= 3;
                MB *= 3;
            }else if(NB%5 == 0){
                NB /= 5;
                MB *= 5;
            }else if(NB%RemNumStartPoints){
                NB /= RemNumStartPoints;
                MB *= RemNumStartPoints;
            }else{
                LPT::LPT_LOG::GetInstance()->ERROR("Illegal NB: ", NB);
            }
        }

        // MB>Mの時はなるべく小さい因数で割って、KBに同じ値を掛ける
        if(MB > M)
        {
            if(MB%2 == 0)
            {
                MB /= 2;
                KB *= 2;
            }else if(MB%3 == 0){
                MB /= 3;
                KB *= 3;
            }else if(MB%5 == 0){
                MB /= 5;
                KB *= 5;
            }else if(MB%RemNumStartPoints){
                MB /= RemNumStartPoints;
                KB *= RemNumStartPoints;
            }else{
                LPT::LPT_LOG::GetInstance()->ERROR("Illegal MB: ", MB);
            }
        }

        // KB>Kの時はなるべく小さい因数で割って、NBに同じ値を掛ける
        if(KB > K)
        {
            if(KB%2 == 0)
            {
                KB /= 2;
                NB *= 2;
            }else if(KB%3 == 0){
                KB /= 3;
                NB *= 3;
            }else if(KB%5 == 0){
                KB /= 5;
                NB *= 5;
            }else if(KB%RemNumStartPoints){
                KB /= RemNumStartPoints;
                NB *= RemNumStartPoints;
            }else{
                LPT::LPT_LOG::GetInstance()->ERROR("Illegal KB: ", KB);
            }
        }
    }
    LPT::LPT_LOG::GetInstance()->LOG("NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("MB = ", MB);
    LPT::LPT_LOG::GetInstance()->LOG("KB = ", KB);
}
} // namespace PPlib