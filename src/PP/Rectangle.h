#ifndef PPLIB_RECTANGLE_H
#define PPLIB_RECTANGLE_H

#include <iostream>

#include "StartPoint.h"

namespace PPlib
{
//forward declaration
  class ParticleData;
  //! @brief 矩形領域型で定義された開始点の情報を保持するクラス
  class Rectangle:public StartPoint
  {
  private:
    //! @brief 開始点が存在する矩形領域の頂点の座標
    //! Coord2とは異なる座標かつ、両方の点が xy, yz, zx いずれかの平面上に存在しなければならない
    REAL_TYPE Coord1[3];

    //! @brief 開始点が存在する矩形領域の頂点の座標
    //! Coord1とは異なる座標かつ、両方の点が xy, yz, zx いずれかの平面上に存在しなければならない
    REAL_TYPE Coord2[3];

    //!  x,y,z方向へ並ぶ開始点の個数(どれかひとつは必ず1になる)
    int NumStartPoints[3];

    //! @brief 矩形領域の頂点のうち、Coord1,2以外の2点と、各辺上の格子点数(両端を含む)を求める
    //! @param Coord3     [out] 格子点座標
    //! @param NumPoints1 [out] Coord1とCoord3の間の格子点数
    //! @param Coord4     [out] 格子点座標
    //! @param NumPoints2 [out] Coord1とCoord4の間の格子点数
    void MakeCoord3_4(DSlib::DV3 * Coord3, int *NumPoints1, DSlib::DV3 * Coord4, int *NumPoints2);

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

    //! @brief 開始点オブジェクトをAveNumStartPointsで指定した開始点数以下のオブジェクトに分割する
    //! NxM行列をNB, MBで2次元のブロック分割(余りあり)するようなイメージで
    //! 矩形領域をNBxMBの小領域+余り領域に分割する
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param AveNumStartPoints [in]  分割後のオブジェクトが持つ最大の開始点数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    std::vector < StartPoint * >*Divider(const int &AveNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

    //! Setter/Getter
    void SetCoord1(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        this->Coord1[i] = Coord[i];
    }};
    void SetCoord2(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        this->Coord2[i] = Coord[i];
    }};
    void GetCoord2(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        Coord[i] = this->Coord2[i];
    }};
    void SetNumStartPoints(int *NumStartPoints)
    {
      for(int i = 0; i < 3; i++) {
        this->NumStartPoints[i] = NumStartPoints[i];
      }
      SumStartPoints = NumStartPoints[0] * NumStartPoints[1] * NumStartPoints[2];
    }
    void GetNumStartPoints(int *NumStartPoints)
    {
      for(int i = 0; i < 3; i++) {
        NumStartPoints[i] = this->NumStartPoints[i];
    }};
    int GetSumStartPoints()
    {
      return (this->NumStartPoints)[0] * (this->NumStartPoints)[1] * (this->NumStartPoints)[2];
    };

    //!  Constructor
      Rectangle():StartPoint()
    {
      for(int i = 0; i < 3; i++)
      {
        Coord2[i] = 0.0;
        NumStartPoints[i] = -1;
      }
    }

    //! Copy Constructor
    Rectangle(const Rectangle & org):StartPoint(org)
    {
      for(int i = 0; i < 3; i++) {
        Coord1[i] = org.Coord1[i];
        Coord2[i] = org.Coord2[i];
        NumStartPoints[i] = org.NumStartPoints[i];
      }
    }

    //! 代入演算子オーバーロード
    Rectangle & operator=(const Rectangle & org)
    {
      for(int i = 0; i < 3; i++) {
        Coord1[i] = org.Coord1[i];
        Coord2[i] = org.Coord2[i];
        NumStartPoints[i] = org.NumStartPoints[i];
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

    //! 比較演算子のオーバーロード
    bool operator==(const Rectangle& obj) const
    {
      return (this->Coord1[0]         == obj.Coord1[0] &&
              this->Coord1[1]         == obj.Coord1[1] &&
              this->Coord1[2]         == obj.Coord1[2] &&
              this->Coord2[0]         == obj.Coord2[0] &&
              this->Coord2[1]         == obj.Coord2[1] &&
              this->Coord2[2]         == obj.Coord2[2] &&
              this->NumStartPoints[0] == obj.NumStartPoints[0] &&
              this->NumStartPoints[1] == obj.NumStartPoints[1] &&
              this->NumStartPoints[2] == obj.NumStartPoints[2] &&
              this->StartTime         == obj.StartTime &&
              this->ReleaseTime       == obj.ReleaseTime &&
              this->TimeSpan          == obj.TimeSpan &&
              this->ParticleLifeTime  == obj.ParticleLifeTime) ? true : false;
    }

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, const Rectangle& obj);

    //! 入力演算子オーバーロード
    friend std::istream & operator >>(std::istream & stream, Rectangle & obj);

  };

} // namespace PPlib
#endif
