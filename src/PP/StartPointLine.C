/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include "StartPointLine.h"
#include "ParticleData.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
std::string Line::TextPrint(void) const
{
    std::ostringstream oss;
    oss<<typeid(*this).name()<<std::endl;
    oss<<"Coord1          = "<<this->Coord1[0]<<","<<this->Coord1[1]<<","<<this->Coord1[2]<<std::endl;
    oss<<"Coord2          = "<<this->Coord2[0]<<","<<this->Coord2[1]<<","<<this->Coord2[2]<<std::endl;
    oss<<"SumStartPoints  = "<<this->SumStartPoints<<std::endl;
    oss<<"StartTime       = "<<this->StartTime<<std::endl;
    oss<<"ReleaseTime     = "<<this->ReleaseTime<<std::endl;
    oss<<"TimeSpan        = "<<this->TimeSpan<<std::endl;
    oss<<"LatestEmitTime  = "<<this->LatestEmitTime<<std::endl;
    oss<<"ID              = "<<this->ID[0]<<","<<this->ID[1]<<std::endl;
    oss<<"LatestEmitParticleID = "<<this->LatestEmitParticleID<<std::endl;
    return oss.str();
}

void Line::ReadText(std::istream& stream)
{
    std::string work;
    //Coord1
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->Coord1[0] = std::atof(work.c_str());
    std::getline(stream, work, ',');
    this->Coord1[1] = std::atof(work.c_str());
    std::getline(stream, work);
    this->Coord1[2] = std::atof(work.c_str());
    //Coord2
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->Coord2[0] = std::atof(work.c_str());
    std::getline(stream, work, ',');
    this->Coord2[1] = std::atof(work.c_str());
    std::getline(stream, work);
    this->Coord2[2] = std::atof(work.c_str());

    //SumStartPoints
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->SumStartPoints = std::atoi(work.c_str());

    //StartTime
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->StartTime = std::atof(work.c_str());
    //ReleaseTime
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->ReleaseTime = std::atof(work.c_str());
    //TimeSpan
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->TimeSpan = std::atof(work.c_str());
    //LatestEmitTime
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->LatestEmitTime = std::atof(work.c_str());

    //ID
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->ID[0] = std::atoi(work.c_str());
    std::getline(stream, work);
    this->ID[1] = std::atoi(work.c_str());

    //LatestEmitParticleID
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->LatestEmitParticleID = std::atoi(work.c_str());
}

void Line::GetGridPointCoord(std::vector<REAL_TYPE>& Coords)
{
    std::vector<REAL_TYPE>           coord_x;
    utility::DivideLine1D(&coord_x, SumStartPoints, Coord1[0], Coord2[0]);
    std::vector<REAL_TYPE>           coord_y;
    utility::DivideLine1D(&coord_y, SumStartPoints, Coord1[1], Coord2[1]);
    std::vector<REAL_TYPE>           coord_z;
    utility::DivideLine1D(&coord_z, SumStartPoints, Coord1[2], Coord2[2]);

    std::vector<REAL_TYPE>::iterator it_x = coord_x.begin();
    std::vector<REAL_TYPE>::iterator it_y = coord_y.begin();
    std::vector<REAL_TYPE>::iterator it_z = coord_z.begin();
    for(int i = 0; i < SumStartPoints; i++)
    {
        Coords.push_back(*it_x++);
        Coords.push_back(*it_y++);
        Coords.push_back(*it_z++);
    }

    LPT::LPT_LOG::GetInstance()->LOG("Number of grid points = ", Coords.size()/3);
}

void Line::Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints)
{
    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピーを格納したvectorを返す
    if(MaxNumStartPoints >= GetSumStartPoints())
    {
        Line* NewLine = LineFactory(this->Coord1, this->Coord2, this->SumStartPoints, this->StartTime, this->ReleaseTime, this->TimeSpan, this->ParticleLifeTime);
        StartPoints->push_back(NewLine);
        return;
    }else if(MaxNumStartPoints <= 0){
        // MaxNumStartPointsが0以下の時はエラーメッセージを出力して終了
        LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
        return;
    }

    //余りオブジェクトが持つ格子点数
    int NumReminder = GetSumStartPoints()%MaxNumStartPoints;

    //余りオブジェクトを除いた分割後のオブジェクトの数
    int NumParts = GetSumStartPoints()/MaxNumStartPoints;

    //分割後のオブジェクトが持つ格子点数
    int NumGridPoints = NumParts > 1 ? (GetSumStartPoints()-NumReminder)/NumParts : -1;

    //分割前のオブジェクトの開始点座標を取得し、先頭から順にNumGridPoints個毎に新しいオブジェクトを作って
    //StartPointsにpush_backする
    std::vector<REAL_TYPE>           Coords;
    this->GetGridPointCoord(Coords);
    std::vector<REAL_TYPE>::iterator itCoords = Coords.begin();
    for(int i = 0; i < NumParts; i++)
    {
        REAL_TYPE Coord1[3] = {(*itCoords++), (*itCoords++), (*itCoords++)};
        itCoords += 3*(NumGridPoints-2);
        REAL_TYPE Coord2[3] = {(*itCoords++), (*itCoords++), (*itCoords++)};
        StartPoints->push_back(LineFactory(Coord1, Coord2, NumGridPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }

    if(NumReminder != 0)
    {
        REAL_TYPE Coord1[3] = {(*itCoords++), (*itCoords++), (*itCoords++)};
        StartPoints->push_back(LineFactory(Coord1, this->Coord2, NumReminder, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }
}
} // namespace PPlib