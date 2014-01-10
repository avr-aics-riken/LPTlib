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
    //! @brief 開始点が存在する線分の端点の座標
    //!  Coord2とは異なる座標でなければならない
    REAL_TYPE Coord1[3];

    //! @brief 開始点が存在する線分の端点の座標
    //!  Coord1とは異なる座標でなければならない
    REAL_TYPE Coord2[3];

  public:
    //! テキスト出力を行う
    std::ostream & TextPrint(std::ostream & stream) const
    {
      stream << "Coord1          = " << this->Coord1[0] << "," << this->Coord1[1] << "," << this->Coord1[2] << std::endl;
      stream << "Coord2          = " << this->Coord2[0] << "," << this->Coord2[1] << "," << this->Coord2[2] << std::endl;
      stream << "SumStartPoints  = " << this->SumStartPoints << std::endl;
      stream << "StartTime       = " << this->StartTime << std::endl;
      stream << "ReleaseTime     = " << this->ReleaseTime << std::endl;
      stream << "TimeSpan        = " << this->TimeSpan << std::endl;
      stream << "LatestEmitTime  = " << this->LatestEmitTime << std::endl;
      stream << "ID              = " << this->ID[0] << "," << this->ID[1] << std::endl;
      return stream;
    }

    //! @brief 開始点オブジェクトをMaxNumStartPointsで指定した開始点数以下のオブジェクトに分割する。
    //! 余りが生じた場合は1つ余計にオブジェクトを生成し、そのオブジェクトに余り領域を入れて返す
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param MaxNumStartPoints [in]  分割後のオブジェクトが持つ最大の開始点数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    void Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector < REAL_TYPE > &Coords);

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
    void GetCoord1(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        Coord[i] = this->Coord1[i];
      }};
    void GetCoord2(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        Coord[i] = this->Coord2[i];
      }};

    //!  Constructor
    Line()
    {
      for(int i = 0; i < 3; i++)
      {
        this->Coord1[i] = 0.0;
        this->Coord2[i] = 0.0;
      }
    }

    //! Copy Constructor
    Line(const Line & org):StartPoint(org)
    {
      for(int i = 0; i < 3; i++) {
        Coord1[i] = org.Coord1[i];
        Coord2[i] = org.Coord2[i];
      }
    }

    //! 比較演算子のオーバーロード
    bool operator==(const Line & obj) const
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

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, const Line& obj);

    //! 入力演算子オーバーロード
    friend std::istream & operator >>(std::istream & stream, Line & obj);
  };

} // namespace PPlib
#endif
