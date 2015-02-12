/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_MOVING_POINTS_H
#define PPLIB_MOVING_POINTS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <typeinfo>
#include "StartPoint.h"

namespace PPlib
{
//forward declaration
class MovingPoints;
MovingPoints* MovingPointsFactory(const int& NumPoints, REAL_TYPE* Coords, double* Time, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
//!  @brief 移動する開始点設定を保持するクラス
//!
//! Time[i]の時点の開始点座標がCoords[i]に保持される
class MovingPoints: public StartPoint
{
public:
    //! テキスト出力を行う
    std::string TextPrint(void) const;

    //! TextPrintの出力を読み込む
    void ReadText(std::istream& stream);

    //! 現在のタイムステップにおける開始点座標をCoordsから読み出して、Coord1にコピーする
    void UpdateStartPoint(double CurrentTime);

private:
    //! 指定された時刻の開始点座標を設定する
    //! @param Time [in] 時刻
    //! @param Coord [in] Timeの時点の開始点座標
    //! Coordの4番目以降の引数は無視される。
    void AddCoords(const double& Time, REAL_TYPE* Coord)
    {
        this->Time.push_back(Time);
        for(int i = 0; i < 3; i++)
        {
            this->Coords.push_back(Coord[i]);
        }
    }

    void Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints);
    void GetGridPointCoord(std::vector<REAL_TYPE>& Coords);

    //!@brief 現在時刻で有効な開始点の座標
    //! インスタンス生成時に値が設定されていてもUpdateStartPoint()内で上書きされる
    REAL_TYPE              Coord1[3];

    //! 時系列で指定した開始点座標
    std::vector<REAL_TYPE> Coords;

    //! Coordsで指定した時刻
    std::vector<double>    Time;

    friend MovingPoints* MovingPointsFactory(const int& NumPoints, REAL_TYPE* Coords, double* Time, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
};
static MovingPoints* MovingPointsFactory(const int& NumPoints, REAL_TYPE* Coords, double* Time, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    MovingPoints* tmpStartPoint = new MovingPoints;
    tmpStartPoint->SumStartPoints = 1;
    for(int i = 0; i < NumPoints; i++)
    {
        tmpStartPoint->AddCoords(Time[i], &(Coords[3*i]));
    }
    tmpStartPoint->StartTime            = StartTime;
    tmpStartPoint->ReleaseTime          = ReleaseTime;
    tmpStartPoint->TimeSpan             = TimeSpan;
    tmpStartPoint->ParticleLifeTime     = ParticleLifeTime;
    tmpStartPoint->Coord1[0]            = Coords[0];
    tmpStartPoint->Coord1[1]            = Coords[1];
    tmpStartPoint->Coord1[2]            = Coords[2];
    tmpStartPoint->LatestEmitParticleID = 0;
    tmpStartPoint->LatestEmitTime       = -0.1;
    tmpStartPoint->ID[0]                = -1;
    tmpStartPoint->ID[1]                = -2;

    return tmpStartPoint;
}
} // namespace PPlib
#endif
