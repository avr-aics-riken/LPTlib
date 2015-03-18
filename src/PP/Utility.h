/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_UTILITY_H
#define PPLIB_UTILITY_H

namespace PPlib
{
class utility
{
    //! @brief 与えられたNを素因数分解して、結果をFactorに収める
    //! @param N      [in]  素因数分解を行なう数
    //! @param Factor [out] 結果を格納するコンテナ
    static void Factorize(const int& N, std::vector<std::pair<int, int> >* Factor);

    //! @brief N^Mを計算して返す
    static int pow(const int& N, const int& M);

public:
    //! @brief 与えられたNを2^p0*3^p1*5^p2*Reminder の形に分解し、2,3,5のべき数と余りを返す
    //! @param N   [in]  素因数分解を行なう数
    //! @param Pow [out] 結果を格納する配列
    static void Factorize235(const int& N, int Pow[4]);

    //! point1とpoint2をnum_points分割した値を昇順にソートしてrtに格納する
    static void DivideLine1D(std::vector<REAL_TYPE>* rt, const int& num_points, const REAL_TYPE& point1, const REAL_TYPE& point2);

    //! NxMxKの領域をNBxMBxKB に分割する
    //
    //余りは許容し、NBxMBxKB<MaxPoints となるように適当に調整する
    static void DetermineBlockSize(int* arg_NB, int* arg_MB, int* arg_KB, const int& MaxPoints, const int& N, const int& M, const int& K);
};
} // namespace PPlib
#endif