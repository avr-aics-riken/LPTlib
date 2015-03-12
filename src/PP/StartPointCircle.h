/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_CIRCLE_H
#define PPLIB_CIRCLE_H

#include <iostream>
#include <sstream>
#include <cmath>
#include <typeinfo>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
class Circle;
Circle* CircleFactory(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
//! @brief 円型で定義された開始点の情報を保持するクラス
//!
//! Istart, theta_minは領域に含み、Iend, theta_maxは含まれない
class Circle: public StartPoint
{
    Circle() : StartPoint(), N(1), a(1), Istart(1), Iend(1), theta_min(0), theta_max(2*M_PI){}

public:
    //! テキスト出力を行う
    std::string TextPrint(const REAL_TYPE& RefLength, const double& RefTime) const;

    //! TextPrintの出力を読み込む
    void ReadText(std::istream& stream, const REAL_TYPE& RefLength, const double& RefTime);

    //! 指定された開始点数からN, aの値を計算する
    bool Initialize(void);

    //! 領域内に含まれる開始点数を計算して返す
    int CalcSumStartPoints(void);

    //! @brief オブジェクトを分割する
    // 円周方向のみを分割する
    // 元の開始点数に対してMaxNumStartPointsで割ると余りがでる場合は
    // 2*M_PIに近いところ(要するに一番最後)の部分を余りオブジェクトとする
    void Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector<REAL_TYPE>& Coords);

private:
    REAL_TYPE Coord1[3];           //!< @brief 開始点が存在する円の中心座標
    REAL_TYPE Radius;              //!< 開始点が存在する円の半径
    REAL_TYPE NormalVector[3];     //!< 円が存在する平面の法線ベクトル
    int       N;                   //!< 開始点が並ぶ同心円の数(N=1の時は外周+中心に開始点が存在する)
    int       a;                   //!< 円周方向に並ぶ開始点数の計算に使う係数
    REAL_TYPE R[9];                //!< z軸から法線ベクトルへの回転行列
    int       Istart;              //!< 半径方向の分割の開始位置
    int       Iend;                //!< 半径方向の分割の終了位置
    int       theta_min;           //!< 円周方向の分割の開始位置
    int       theta_max;           //!< 円周方向の分割の開始位置

    //! オブジェクトをxy平面内の円から、指定された正しい位置へ移動させる回転行列を求める
    void MakeRotationMatrix(REAL_TYPE* OuterProducts);

    //! メンバ変数の法線ベクトルをz成分が正の方向を向いた単位ベクトルに変換する
    void ConvertNormalVector(void);

    //! メンバ変数の法線ベクトルとz軸(0,0,1)との外積を求める
    void MakeOuterProducts(REAL_TYPE* OuterProducts);

    //! 引数で与えられたベクトルを単位ベクトルに変換する
    void NormalizeVector(REAL_TYPE* v);

    //! 開始点数を元にメンバ変数のNとaの値を決める
    bool Make_N_and_a(void);

    //! @brief Nとaの値が妥当かどうかを判定する
    //! 開始点を配置した同心円の半径の差と、最外周の円上の開始点間の直線距離の比が
    //! 0.5から1.5の間にあれば妥当とみなしている
    bool isReasonable_N_and_a(void);

    friend Circle* CircleFactory(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
};
static Circle* CircleFactory(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    Circle* tmpStartPoint = new Circle;
    if(Coord1 == NULL && SumStartPoints == NULL && Radius == NULL && NormalVector == NULL && StartTime == NULL && ReleaseTime == NULL && TimeSpan == NULL && ParticleLifeTime == NULL)return tmpStartPoint;

    for(int i = 0; i < 3; i++)
    {
        tmpStartPoint->Coord1[i] = Coord1[i];
    }
    tmpStartPoint->SumStartPoints = SumStartPoints;
    tmpStartPoint->Radius         = Radius;
    for(int i = 0; i < 3; i++)
    {
        tmpStartPoint->NormalVector[i] = NormalVector[i];
    }
    tmpStartPoint->StartTime        = StartTime;
    tmpStartPoint->ReleaseTime      = ReleaseTime;
    tmpStartPoint->TimeSpan         = TimeSpan;
    tmpStartPoint->ParticleLifeTime = ParticleLifeTime;

    if(!tmpStartPoint->Initialize())
    {
        delete tmpStartPoint;
        tmpStartPoint = NULL;
    }

    return tmpStartPoint;
}
} // namespace PPlib
#endif