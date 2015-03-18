/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_RECTANGLE_H
#define PPLIB_RECTANGLE_H

#include <iostream>
#include <sstream>

#include <typeinfo>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
class Rectangle;
Rectangle* RectangleFactory(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
//! @brief 矩形領域型で定義された開始点の情報を保持するクラス
//
//領域は座標軸に直交した平面内で定義され、各辺はどれかひとつの座標軸に並行でなければならない
class Rectangle: public StartPoint
{
    Rectangle() : StartPoint(){}

public:
    //! テキスト出力を行う
    std::string TextPrint(const REAL_TYPE& RefLength, const double& RefTime) const;

    //! TextPrintの出力を読み込む
    void ReadText(std::istream& stream, const REAL_TYPE& RefLength, const double& RefTime);

    //! @brief 開始点オブジェクトをMaxNumStartPointsで指定した開始点数以下のオブジェクトに分割する
    //! NxM行列をNB, MBで2次元のブロック分割(余りあり)するようなイメージで
    //! 矩形領域をNBxMBの小領域+余り領域に分割する
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param MaxNumStartPoints [in]  分割後のオブジェクトが持つ最大の開始点数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    void Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints);

    //! 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    void GetGridPointCoord(std::vector<REAL_TYPE>& Coords);

    void GetCoord2(REAL_TYPE* Coord)
    {
        for(int i = 0; i < 3; i++)
        {
            Coord[i] = this->Coord2[i];
        }
    }

    void GetNumStartPoints(int* NumStartPoints)
    {
        for(int i = 0; i < 3; i++)
        {
            NumStartPoints[i] = this->NumStartPoints[i];
        }
    }

private:
    REAL_TYPE Coord1[3];         //!< 開始点が存在する矩形領域のひとつの頂点の座標
    REAL_TYPE Coord2[3];         //!< Coord1と対角線上にある頂点の座標
    int       NumStartPoints[3]; //!< x,y,z方向へ並ぶ開始点の個数(どれかひとつは必ず1になる)

    //! @brief 矩形領域の頂点のうち、Coord1,2以外の2点と、各辺上の格子点数(両端を含む)を求める
    //! @param Coord3     [out] 格子点座標
    //! @param NumPoints1 [out] Coord1とCoord3の間の格子点数
    //! @param Coord4     [out] 格子点座標
    //! @param NumPoints2 [out] Coord1とCoord4の間の格子点数
    void MakeCoord3_4(REAL_TYPE Coord3[3], int* NumPoints1, REAL_TYPE Coord4[3], int* NumPoints2);

    friend Rectangle* RectangleFactory(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
};
static Rectangle* RectangleFactory(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    if(Coord1 == NULL && Coord2 == NULL && NumStartPoints == NULL && StartTime == NULL && ReleaseTime == NULL && TimeSpan == NULL && ParticleLifeTime == NULL)
    {
        Rectangle* tmpStartPoint = new Rectangle;
        return tmpStartPoint;
    }

    if(Coord1[0] == Coord2[0])
    {
        if(NumStartPoints[0] != 1)return NULL;
    }else if(Coord1[1] == Coord2[1]){
        if(NumStartPoints[1] != 1)return NULL;
    }else if(Coord1[2] == Coord2[2]){
        if(NumStartPoints[2] != 1)return NULL;
    }else{
        return NULL;
    }

    Rectangle* tmpStartPoint = new Rectangle;
    for(int i = 0; i < 3; i++)
    {
        tmpStartPoint->Coord1[i]         = Coord1[i];
        tmpStartPoint->Coord2[i]         = Coord2[i];
        tmpStartPoint->NumStartPoints[i] = NumStartPoints[i];
    }
    tmpStartPoint->SumStartPoints   = NumStartPoints[0]*NumStartPoints[1]*NumStartPoints[2];
    tmpStartPoint->StartTime        = StartTime;
    tmpStartPoint->ReleaseTime      = ReleaseTime;
    tmpStartPoint->TimeSpan         = TimeSpan;
    tmpStartPoint->ParticleLifeTime = ParticleLifeTime;
    return tmpStartPoint;
}
} // namespace PPlib
#endif