/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include "StartPointPoint.h"
#include "ParticleData.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
std::string Point::TextPrint(void) const
{
    std::ostringstream oss;
    oss<<typeid(*this).name()<<std::endl;
    oss<<"Coord1          = "<<this->Coord1[0]<<","<<this->Coord1[1]<<","<<this->Coord1[2]<<std::endl;
    oss<<"SumStartPoints  = "<<this->SumStartPoints<<std::endl;
    oss<<"StartTime       = "<<this->StartTime<<std::endl;
    oss<<"ReleaseTime     = "<<this->ReleaseTime<<std::endl;
    oss<<"TimeSpan        = "<<this->TimeSpan<<std::endl;
    oss<<"LatestEmitTime  = "<<this->LatestEmitTime<<std::endl;
    oss<<"ID              = "<<this->ID[0]<<","<<this->ID[1]<<std::endl;
    oss<<"LatestEmitParticleID = "<<this->LatestEmitParticleID<<std::endl;
    return oss.str();
}

void Point::ReadText(std::istream& stream)
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

void Point::GetGridPointCoord(std::vector<REAL_TYPE>& Coords)
{
    Coords.push_back(Coord1[0]);
    Coords.push_back(Coord1[1]);
    Coords.push_back(Coord1[2]);
}

void Point::Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints)
{
    if(MaxNumStartPoints >= GetSumStartPoints())
    {
        Point* NewPoint = PointFactory(this->Coord1, this->StartTime, this->ReleaseTime, this->TimeSpan, this->ParticleLifeTime);
        StartPoints->push_back(NewPoint);
    }else{
        // MaxNumStartPointsがGetSumStartPoints() (=必ず1)より小さい場合はwarnningを出力するだけ
        LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
    }
}
} // namespace PPlib