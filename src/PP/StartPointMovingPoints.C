/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <iostream>
#include "StartPointMovingPoints.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
std::string MovingPoints::TextPrint(void) const
{
    std::ostringstream oss;
    oss<<typeid(*this).name()<<std::endl;
    size_t             NumCoordinateSets = this->Coords.size()/3;
    oss<<"Number of Coords = "<<NumCoordinateSets<<std::endl;
    oss<<"Coords        = "<<std::endl;
    for(int i = 0; i < NumCoordinateSets; i++)
    {
        oss<<this->Coords[3*i]<<","<<this->Coords[3*i+1]<<","<<this->Coords[3*i+2]<<std::endl;
    }

    oss<<"Number of Times = "<<(this->Time).size()<<std::endl;
    oss<<"Times         = "<<std::endl;
    for(int i = 0; i < (this->Time).size(); i++)
    {
        oss<<this->Time[i]<<std::endl;
    }

    oss<<"StartTime       = "<<this->StartTime<<std::endl;
    oss<<"ReleaseTime     = "<<this->ReleaseTime<<std::endl;
    oss<<"TimeSpan        = "<<this->TimeSpan<<std::endl;
    oss<<"LatestEmitTime  = "<<this->LatestEmitTime<<std::endl;
    oss<<"ID              = "<<this->ID[0]<<","<<this->ID[1]<<std::endl;
    oss<<"LatestEmitParticleID = "<<this->LatestEmitParticleID<<std::endl;
    return oss.str();
}

void MovingPoints::ReadText(std::istream& stream)
{
    std::string work;
    //Coords
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    long NumData = std::atol(work.c_str());
    std::getline(stream, work, '=');
    for(long i = 0; i < NumData; i++)
    {
        std::getline(stream, work, ',');
        this->Coords.push_back(std::atof(work.c_str()));
        std::getline(stream, work, ',');
        this->Coords.push_back(std::atof(work.c_str()));
        std::getline(stream, work, ',');
        this->Coords.push_back(std::atof(work.c_str()));
    }

    //Times
    std::getline(stream, work, '=');
    std::getline(stream, work);
    NumData = std::atol(work.c_str());
    std::getline(stream, work, '=');
    for(long i = 0; i < NumData; i++)
    {
        std::getline(stream, work);
        this->Time.push_back(std::atof(work.c_str()));
    }

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

void MovingPoints::UpdateStartPoint(double CurrentTime)
{
    if(CurrentTime < *(Time.begin())) return;

    //TimeからCurrentTimeに相当するindexを取得
    size_t index = distance(Time.begin(), lower_bound(Time.begin(), Time.end(), CurrentTime))-1;
    if(index >= 0 && index < Time.size())
    {
        Coord1[0] = Coords[index*3];
        Coord1[1] = Coords[index*3+1];
        Coord1[2] = Coords[index*3+2];
    }
}

void MovingPoints::GetGridPointCoord(std::vector<REAL_TYPE>& Coords)
{
    Coords.push_back(Coord1[0]);
    Coords.push_back(Coord1[1]);
    Coords.push_back(Coord1[2]);
}

void MovingPoints::Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints)
{
    if(MaxNumStartPoints >= GetSumStartPoints())
    {
        MovingPoints* NewPoint = MovingPointsFactory(this->Time.size(), &*(this->Coords.begin()), &*(this->Time.begin()), this->StartTime, this->ReleaseTime, this->TimeSpan, this->ParticleLifeTime);
        StartPoints->push_back(NewPoint);
    }else{
        // MaxNumStartPointsがGetSumStartPoints() (=必ず1)より小さい場合はwarnningを出力するだけ
        LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
    }
}
} // namespace PPlib