#ifndef PPLIB_CUBOID_H
#define PPLIB_CUBOID_H

#include <iostream>
#include <vector>
#include <list>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
  class ParticleData;

  //! @brief 直方体型で定義された開始点の情報を保持するクラス
  class Cuboid:public StartPoint
  {
  private:
    //! 開始点が存在する直方体領域の頂点
    REAL_TYPE Coord2[3];

    //! x,y,z方向へ並ぶ開始点の個数
    int NumStartPoints[3];

  public:
    //! Constructor
    Cuboid()
    {
      Coord2[0] = 0.0;
      Coord2[1] = 0.0;
      Coord2[2] = 0.0;
      NumStartPoints[0] = -1;
      NumStartPoints[1] = -1;
      NumStartPoints[2] = -1;
    }

    //! @brief 引数に指定された配列の先頭から3要素をメンバ変数Coord2に格納する
    //!  4番目以降の要素が存在しても無視される
    void SetCoord2(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++)
      {
        this->Coord2[i] = Coord[i];
      }
    };

    //! 引数に指定された配列の先頭から3要素をメンバ変数NumStartPointsに格納する
    //!  4番目以降の要素が存在しても無視される
    //!  また、NumStartPointsの積をSumStartPointsに格納する
    void SetNumStartPoints(int *NumStartPoints)
    {
      for(int i = 0; i < 3; i++) {
        this->NumStartPoints[i] = NumStartPoints[i];
      }
      SumStartPoints = NumStartPoints[0] * NumStartPoints[1] * NumStartPoints[2];
    }

    //!  引数で指定された数で2次元のブロック分割状の分割を行ない、個々の小領域を指定する1個以上のCuboidのインスタンス(複数)を返す
    std::vector < StartPoint * >*Divider(const int &NumParts);

    void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

    //! 挿入子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, Cuboid & obj);

  };

} // namespace PPlib
#endif
