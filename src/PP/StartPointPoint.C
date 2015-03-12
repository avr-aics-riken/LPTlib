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
std::string Point::TextPrint(const REAL_TYPE& RefLength, const double& RefTime) const
{
    std::ostringstream oss;
    oss<<"Point"<<std::endl;
    oss<<"Coord1          = "<<this->Coord1[0]*RefLength<<","<<this->Coord1[1]*RefLength<<","<<this->Coord1[2]*RefLength<<std::endl;

    oss<<this->PrintTimeAndID(RefTime);
    return oss.str();
}

void Point::ReadText(std::istream& stream, const REAL_TYPE& RefLength, const double& RefTime)
{
    std::string work;
    //Coord1
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->Coord1[0] = std::atof(work.c_str())/RefLength;
    std::getline(stream, work, ',');
    this->Coord1[1] = std::atof(work.c_str())/RefLength;
    std::getline(stream, work);
    this->Coord1[2] = std::atof(work.c_str())/RefLength;

    this->ReadTimeAndID(stream, RefTime);
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
