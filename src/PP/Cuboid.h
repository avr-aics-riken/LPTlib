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
    //! @brief 開始点が存在する直方体領域の頂点
    //! Coord2の対角線上に存在する頂点の座標
    REAL_TYPE Coord1[3];

    //! @brief 開始点が存在する直方体領域の頂点の座標
    //! Coord1の対角線上に存在する頂点の座標
    REAL_TYPE Coord2[3];

    //! x,y,z方向へ並ぶ開始点の個数
    int NumStartPoints[3];

    //! メンバ変数として保持する2頂点の座標から全頂点の座標を生成する
    void getAllVertexCoord(DSlib::DV3 *DV3Coord);

  public:
    //! テキスト出力を行う
    std::ostream & TextPrint(std::ostream & stream) const
    {
      stream << "Coord1          = " << this->Coord1[0] << "," << this->Coord1[1] << "," << this->Coord1[2] << std::endl;
      stream << "Coord2          = " << this->Coord2[0] << "," << this->Coord2[1] << "," << this->Coord2[2] << std::endl;
      stream << "SumStartPoints  = " << this->SumStartPoints << std::endl;
      stream << "NumStartPoints  = " << this->NumStartPoints[0] << "," << this->NumStartPoints[1] << "," << this->NumStartPoints[2] << std::endl;
      stream << "StartTime       = " << this->StartTime << std::endl;
      stream << "ReleaseTime     = " << this->ReleaseTime << std::endl;
      stream << "TimeSpan        = " << this->TimeSpan << std::endl;
      stream << "LatestEmitTime  = " << this->LatestEmitTime << std::endl;
      stream << "ID              = " << this->ID[0] << "," << this->ID[1] << std::endl;
      return stream;
    }
    void SetCoord1(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        this->Coord1[i] = Coord[i];
    }};
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

    //! @brief 開始点オブジェクトをMaxNumStartPointsで指定した開始点数以下のオブジェクトに分割する
    //! 扇型に(中心角の方向のみ分割)し半径方向には分割しない
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param MaxNumStartPoints [in]  分割後のオブジェクトが持つ最大の開始点数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    void Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

    //! Constructor
    Cuboid()
    {
      for(int i = 0; i < 3; i++)
      {
        Coord1[i] = 0.0;
        Coord2[i] = 0.0;
        NumStartPoints[i] = -1;
      }
    }

    //! 比較演算子のオーバーロード
    bool operator==(const Cuboid & obj) const
    {
      return (this->Coord1[0]         == obj.Coord1[0] &&
              this->Coord1[1]         == obj.Coord1[1] &&
              this->Coord1[2]         == obj.Coord1[2] &&
              this->Coord2[0]         == obj.Coord2[0] &&
              this->Coord2[1]         == obj.Coord2[1] &&
              this->Coord2[2]         == obj.Coord2[2] &&
              this->SumStartPoints    == obj.SumStartPoints &&
              this->StartTime         == obj.StartTime &&
              this->ReleaseTime       == obj.ReleaseTime &&
              this->TimeSpan          == obj.TimeSpan &&
              this->ParticleLifeTime  == obj.ParticleLifeTime) ? true : false;
    }

    //! 代入演算子オーバーロード
    Cuboid & operator=(const Cuboid& org)
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

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, const Cuboid& obj);

    //! 入力演算子オーバーロード
    friend std::istream & operator >>(std::istream & stream, Cuboid& obj);

  };

} // namespace PPlib
#endif
