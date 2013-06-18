#ifndef PPLIB_UTILITY_H
#define PPLIB_UTILITY_H

namespace PPlib
{
  //! @brief 与えられたNを2^p0*3^p1*5^p2*Reminder の形に分解し、2,3,5のべき数と余りを返す
  //! @param N   [in]  素因数分解を行なう数
  //! @param Pow [out] 結果を格納する配列
  void Factorize235(const int &N, int Pow[4]);

  //! @brief 与えられたNを素因数分解して、結果をFactorに収める
  //! @param N      [in]  素因数分解を行なう数
  //! @param Factor [out] 結果を格納するコンテナ
  void Factorize(int &N, std::vector < std::pair < int, int > >*Factor);

  //! @brief N^Mを計算して返す
  int pow(const int &N, const int &M);

} // namespace PPlib
#endif
