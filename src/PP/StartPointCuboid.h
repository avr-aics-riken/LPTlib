/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_CUBOID_H
#define PPLIB_CUBOID_H

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <typeinfo>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
class Cuboid;
Cuboid* CuboidFactory(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);

//! @brief 直方体型で定義された開始点の情報を保持するクラス
class Cuboid: public StartPoint
{
    Cuboid():StartPoint(){}
public:
    //! テキスト出力を行う
    std::string TextPrint(const REAL_TYPE& RefLength, const double& RefTime) const;

    //! TextPrintの出力を読み込む
    void ReadText(std::istream& stream, const REAL_TYPE& RefLength, const double& RefTime);

    //! @brief 開始点オブジェクトをMaxNumStartPointsで指定した開始点数以下のオブジェクトに分割する
    //! 扇型に(中心角の方向のみ分割)し半径方向には分割しない
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param MaxNumStartPoints [in]  分割後のオブジェクトが持つ最大の開始点数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    virtual void Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    virtual void GetGridPointCoord(std::vector<REAL_TYPE>& Coords);

private:
    //! @brief 開始点が存在する直方体領域の頂点
    //! Coord2の対角線上に存在する頂点の座標
    REAL_TYPE Coord1[3];

    //! @brief 開始点が存在する直方体領域の頂点の座標
    //! Coord1の対角線上に存在する頂点の座標
    REAL_TYPE Coord2[3];

    //! x,y,z方向へ並ぶ開始点の個数
    int       NumStartPoints[3];

    //! x方向の余りオブジェクトを分割
    void ShrinkX(std::vector<StartPoint*>* StartPoints, int* N, const int& NB, const std::vector<REAL_TYPE>& coord_x);

    //! y方向の余りオブジェクトを分割
    void ShrinkY(std::vector<StartPoint*>* StartPoints, int* M, const int& MB, const std::vector<REAL_TYPE>& coord_y);

    //! z方向の余りオブジェクトを分割
    void ShrinkZ(std::vector<StartPoint*>* StartPoints, int* K, const int& KB, const std::vector<REAL_TYPE>& coord_z);

    friend Cuboid* CuboidFactory(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
};
static Cuboid* CuboidFactory(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    Cuboid* tmpStartPoint = new Cuboid;
    if(Coord1==NULL&&Coord2==NULL&&NumStartPoints==NULL&&StartTime==NULL&&ReleaseTime==NULL&&TimeSpan==NULL&&ParticleLifeTime==NULL) return tmpStartPoint;
    for(int i = 0; i < 3; i++)
    {
        tmpStartPoint->Coord1[i]         = Coord1[i];
        tmpStartPoint->Coord2[i]         = Coord2[i];
        tmpStartPoint->NumStartPoints[i] = NumStartPoints[i];
    }
    tmpStartPoint->SumStartPoints       = NumStartPoints[0]*NumStartPoints[1]*NumStartPoints[2];
    tmpStartPoint->StartTime            = StartTime;
    tmpStartPoint->ReleaseTime          = ReleaseTime;
    tmpStartPoint->TimeSpan             = TimeSpan;
    tmpStartPoint->ParticleLifeTime     = ParticleLifeTime;

    return tmpStartPoint;
}
} // namespace PPlib
#endif
