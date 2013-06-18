#include <vector>
#include <utility>
#include <cmath>

#include "Utility.h"

namespace PPlib
{

  int pow(const int &N, const int &M)
  {
    int rt = 1;
    for(int i = 0; i < M; i++)
        rt *= N;
      return rt;
  }

  void Factorize235(const int &N, int Pow[4])
  {
    //Pow[]を初期化
    Pow[0] = 0;
    Pow[1] = 0;
    Pow[2] = 0;
    Pow[3] = N;

    //2で何回割れるか？
    while(Pow[3] % 2 == 0) {
      Pow[3] /= 2;
      Pow[0] += 1;
    }

    //3で何回割れるか？
    while(Pow[3] % 3 == 0) {
      Pow[3] /= 3;
      Pow[1] += 1;
    }

    //5で何回割れるか？
    while(Pow[3] % 5 == 0) {
      Pow[3] /= 5;
      Pow[2] += 1;
    }

    //Pow[3]は余り
  }

  void Factorize(int &N, std::vector < std::pair < int, int > >*Factor)
  {
    //念のためFactorを空にする
    Factor->clear();
    //Nが0以下なら何もしない
    if(N <= 0) {
      return;
    }
    //Nが1の時は1x1をFactorに入れて終了
    if(N == 1) {
      Factor->push_back(std::pair < int, int >(1, 1));

      return;
    }
    //2で何回割れるか？
    int pow2 = 0;

    while(N % 2 != 0) {
      N /= 2;
      pow2++;
    }
    Factor->push_back(std::pair < int, int >(2, pow2));

    if(N == 1) {
      return;
    }
    //3以上の奇数の因数を探す
    for(int i = 3; i <= std::sqrt((double)N); i += 2) {
      int pow = 0;

      while(N % i != 0) {
        N /= i;
        pow++;
      }
      Factor->push_back(std::pair < int, int >(i, pow));

      //因数分解が完了してたら終了
      if(N == 1) {
        return;
      }
    }
    //残りの素数があれば、Factorに追加して終了
    Factor->push_back(std::pair < int, int >(N, 1));
    return;
  }

} // namespace PPlib
