#ifndef PPLIB_CIRCLE_H
#define PPLIB_CIRCLE_H

#include <iostream>
#include <cmath>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
  class ParticleData;
  //! @brief 円型で定義された開始点の情報を保持するクラス
  //! Istart, theta_minは領域に含み、Iend, theta_maxは含まれない
  class Circle:public StartPoint
  {
  private:
    //! @brief 開始点が存在する円の中心座標
    REAL_TYPE Coord1[3];

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

    //! オブジェクトをxy平面内の円から、指定された正しい位置へ移動させる回転行列を求める
    void MakeRotationMatrix(REAL_TYPE * OuterProducts);

    //! メンバ変数の法線ベクトルをz成分が正の方向を向いた単位ベクトルに変換する
    void ConvertNormalVector(void);

    //! メンバ変数の法線ベクトルとz軸(0,0,1)との外積を求める
    void MakeOuterProducts(REAL_TYPE* OuterProducts);

    //! 引数で与えられたベクトルを単位ベクトルに変換する
    void NormalizeVector(REAL_TYPE * v);

    //! 開始点数を元にメンバ変数のNとaの値を決める
    bool Make_N_and_a(void);

    //! @brief Nとaの値が妥当かどうかを判定する
    //! 開始点を配置した同心円の半径の差と、最外周の円上の開始点間の直線距離の比が
    //! 0.5から1.5の間にあれば妥当
  bool isReasonable_N_and_a(void);

  public:
    //! テキスト出力を行う
    std::ostream & TextPrint(std::ostream & stream) const
    {
      stream << "Coord1          = " << this->Coord1[0] << "," << this->Coord1[1] << "," << this->Coord1[2] << std::endl;
      stream << "NormalVector    = " << this->NormalVector[0] << "," << this->NormalVector[1] << "," << this->NormalVector[2] << std::endl;
      stream << "Radius          = " << this->Radius << std::endl;
      stream << "SumStartPoints  = " << this->SumStartPoints << std::endl;
      stream << "StartTime       = " << this->StartTime << std::endl;
      stream << "ReleaseTime     = " << this->ReleaseTime << std::endl;
      stream << "TimeSpan        = " << this->TimeSpan << std::endl;
      stream << "LatestEmitTime  = " << this->LatestEmitTime << std::endl;
      stream << "ID              = " << this->ID[0] << "," << this->ID[1] << std::endl;
      return stream;
    }

    //! 指定された開始点数からN, aの値を計算する
    bool Initialize(void);

    //! Accessor
    void SetCoord1(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        this->Coord1[i] = Coord[i];
    }};
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
    std::vector < StartPoint * >*Divider(const int &AveNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

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

    //! Copy Constructor
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

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, const Circle& obj);

    //! 入力演算子オーバーロード
    friend std::istream & operator >>(std::istream & stream, Circle& obj);

  };

} // namespace PPlib
#endif
