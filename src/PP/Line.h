#ifndef PPLIB_LINE_H
#define PPLIB_LINE_H

#include <iostream>
#include <vector>
#include <list>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
  class ParticleData;
  //! @brief 線分型で定義された開始点の情報を保持するクラス
  class Line:public StartPoint
  {
  private:
    //! @brief 開始点が存在する線分の端点
    //!  Coord1とは異なる座標でなければならない
    REAL_TYPE Coord2[3];

  public:
    //!  Constructor
    Line()
    {
      Coord2[0] = 0.0;
      Coord2[1] = 0.0;
      Coord2[2] = 0.0;
    }

    //! @brief 開始点オブジェクトをAveNumStartPointsで指定した開始点数以下のオブジェクトに分割する。
    //! 余りが生じた場合は1つ余計にオブジェクトを生成し、そのオブジェクトに余り領域を入れて返す
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param NumParts    [in]  余り領域を除いた分割後のオブジェクト数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    std::vector < StartPoint * >*Divider(const int &AveNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

    //!  Setter/Getter
    void SetCoord2(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++)
      {
        this->Coord2[i] = Coord[i];
      }
    };
    void GetCoord2(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        Coord[i] = this->Coord2[i];
      }
    };

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, Line & obj);
    friend std::istream & operator >>(std::istream & stream, Line & obj);

    //! 代入演算子オーバーロード
    Line & operator=(const Line & org)
    {
      for(int i = 0; i < 3; i++) {
        Coord1[i] = org.Coord1[i];
        Coord2[i] = org.Coord2[i];
      }
      SumStartPoints = org.SumStartPoints;
      StartTime = org.StartTime;
      ReleaseTime = org.ReleaseTime;
      TimeSpan = org.TimeSpan;
      LatestEmitTime = org.LatestEmitTime;
      ID[0] = org.ID[0];
      ID[1] = org.ID[1];
      ParticleLifeTime = org.ParticleLifeTime;
      return *this;
    }

  };

} // namespace PPlib
#endif
