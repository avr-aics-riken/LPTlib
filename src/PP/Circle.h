#ifndef PPLIB_CIRCLE_H
#define PPLIB_CIRCLE_H

#include <iostream>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
  class ParticleData;
  //! @brief 円型で定義された開始点の情報を保持するクラス
  class Circle:public StartPoint
  {
  private:
    //! 開始点が存在する円の半径
    REAL_TYPE Radius;

    //! 円が存在する平面の法線ベクトル
    REAL_TYPE NormalVector[3];

    //! 半径方向に並ぶ開始点数
    int N;

    //! 円周方向に並ぶ開始点数の計算に使う係数
    int a;

    //! z軸から法線ベクトルへの回転行列
    REAL_TYPE R[9];

    //! 半径方向の分割の開始位置
    int Istart;

    //! 半径方向の分割の終了位置
    int Iend;

    //! 円周方向の分割の開始位置
    int theta_min;

    //! 円周方向の分割の開始位置
    int theta_max;

  public:
    //! Constructor
    Circle():StartPoint()
    {
      Radius = -1.0;
      NormalVector[0] = -1.0;
      NormalVector[1] = -1.0;
      NormalVector[2] = -1.0;
      N = 1;
      a = 1;
      Istart = 1;
      Iend = N;
      theta_min = 0;
      theta_max = 2 * M_PI;
    }

    Circle(const Circle & org):StartPoint(org)
    {
      Radius = org.Radius;
      for(int i = 0; i < 3; ++i)
        NormalVector[i] = org.NormalVector[i];
      N = org.N;
      a = org.a;
      for(int i = 0; i < 9; ++i)
        R[i] = org.R[i];
      Istart = org.Istart;
      Iend = org.Iend;
      theta_min = org.theta_min;
      theta_max = org.theta_max;
    }

    //! 代入演算子オーバーロード
    Circle & operator=(const Circle & org)
    {
      Radius = org.Radius;
      for(int i = 0; i < 3; ++i)
        NormalVector[i] = org.NormalVector[i];
      N = org.N;
      a = org.a;
      for(int i = 0; i < 9; ++i)
        R[i] = org.R[i];
      Istart = org.Istart;
      Iend = org.Iend;
      theta_min = org.theta_min;
      theta_max = org.theta_max;
      return *this;
    }

    //! 指定された開始点数からN, aの値を計算する
    bool Initialize(void);

    //! Accessor
    void SetRadius(REAL_TYPE Radius)
    {
      this->Radius = Radius;
    };
    REAL_TYPE GetRadius(void)
    {
      return this->Radius;
    };

    void SetNormalVector(REAL_TYPE argNormalVector[3])
    {
      this->NormalVector[0] = argNormalVector[0];
      this->NormalVector[1] = argNormalVector[1];
      this->NormalVector[2] = argNormalVector[2];
    };

    void GetNormalVector(REAL_TYPE argNormalVector[3])
    {
      argNormalVector[0] = this->NormalVector[0];
      argNormalVector[1] = this->NormalVector[1];
      argNormalVector[2] = this->NormalVector[2];
    };

    //! オブジェクトを分割する 
    std::vector < StartPoint * >*Divider(const int &NumParts);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

    //! 挿入子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, Circle & obj);

  };

} // namespace PPlib
#endif
