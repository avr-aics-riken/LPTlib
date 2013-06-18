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
    //! @brief 開始点が存在する矩形領域の頂点
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
        Coord2[i] = org.Coord2[i];
        NumStartPoints[i] = org.NumStartPoints[i];
      }
    }

    //! 代入演算子オーバーロード
    Rectangle & operator=(const Rectangle & org)
    {
      for(int i = 0; i < 3; i++) {
        Coord2[i] = org.Coord2[i];
        NumStartPoints[i] = org.NumStartPoints[i];
      }
      return *this;
    }

    //! @brief 開始点オブジェクトをAveNumStartPointsで指定した開始点数以下のオブジェクトに分割する
    //! NxM行列をNB, MBで2次元のブロック分割(余りあり)するようなイメージで
    //! 矩形領域をNBxMBの小領域+余り領域に分割する
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param NumParts    [in]  余り領域を除いた分割後のオブジェクト数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    std::vector < StartPoint * >*Divider(const int &NumParts);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

    //! Setter/Getter
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
      this->SetSumStartPoints(NumStartPoints[0] * NumStartPoints[1] * NumStartPoints[2]);
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

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, Rectangle & obj);
    friend std::istream & operator >>(std::istream & stream, Rectangle & obj);

  };

} // namespace PPlib
#endif
