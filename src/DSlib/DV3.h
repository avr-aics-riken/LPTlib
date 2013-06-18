#ifndef DSLIB_D_V3_H
#define DSLIB_D_V3_H
#include <ostream>

namespace DSlib
{
//! @brief 実数型の3成分ベクトル構造体(REAL_TYPE=float or double)
  struct DV3
  {
    //! 成分
    REAL_TYPE x, y, z;

    //! デフォルトコンストラクタ
      DV3():x(0), y(0), z(0)
    {
    }
    //! コピーコンストラクタ
    DV3(const REAL_TYPE & _x, const REAL_TYPE & _y, const REAL_TYPE & _z):x(_x), y(_y), z(_z)
    {
    }

    //! []オペレータ
    REAL_TYPE & operator[] (const size_t i)
    {
      return (i == 0 ? x : (i == 1 ? y : z));
    }
    //! []オペレータ
    REAL_TYPE operator[] (const size_t i)const
    {
      return (i == 0 ? x : (i == 1 ? y : z));
    }
    //! 等号オペレータ
    bool operator==(const DV3 & tgt)const
    {
      return (x == tgt.x && y == tgt.y && z == tgt.z);
    }
    //! 比較演算子
    bool operator<(const DV3 & tgt)const
    {
      return (((REAL_TYPE) tgt.z - (REAL_TYPE) z) * 1000000 + ((REAL_TYPE) tgt.y - (REAL_TYPE) y) * 1000 + ((REAL_TYPE) tgt.x - (REAL_TYPE) x) > 0.f);
    }
    //! 出力演算子
    friend std::ostream & operator <<(std::ostream & stream, const DV3 & obj);
  };
} // namespace DSlib
#endif
