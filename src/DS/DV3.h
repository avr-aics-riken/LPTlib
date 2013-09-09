#ifndef DSLIB_D_V3_H
#define DSLIB_D_V3_H
#include <ostream>
#include <cmath>

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
    //! デストラクタ
    ~DV3()
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
    // x,y,zが全て完全一致または相対誤差が1e-5以下であれば一致とみなす
    // 符号が異なる場合は誤差が非常に小さくても不一致とする
    // see http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
    bool operator==(const DV3 & tgt)const
    {
      const REAL_TYPE MaxRelativeError = 0.00001;
      if (x == tgt.x && y == tgt.y && z == tgt.z)
      {
       return  true;
      }

      REAL_TYPE divider_x=x > tgt.x? x: tgt.x;
      REAL_TYPE divider_y=y > tgt.y? y: tgt.y;
      REAL_TYPE divider_z=z > tgt.z? z: tgt.z;

      return (std::abs(x-tgt.x)/divider_x < MaxRelativeError &&
              std::abs(y-tgt.y)/divider_y < MaxRelativeError &&
              std::abs(z-tgt.z)/divider_z < MaxRelativeError) ? true:false;
    }

    //! 比較演算子
    bool operator<(const DV3 & tgt)const
    {
      if(tgt.x != x){
        return tgt.x > x;
      }else if (tgt.y != y){
        return tgt.y > y;
      }else{
        return tgt.z > z;
      }
    }
    //! 出力演算子
    friend std::ostream & operator <<(std::ostream & stream, const DV3 & obj);
  };
} // namespace DSlib
#endif
