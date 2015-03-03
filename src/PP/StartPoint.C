/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <utility>
#include <cmath>
#include <typeinfo>

#include "StartPoint.h"
#include "ParticleData.h"
#include "StartPointAll.h"
#include "DecompositionManager.h"

namespace PPlib
{
void StartPoint::EmitNewParticle(std::list<ParticleData*>* ParticleList, const double& CurrentTime, const int& CurrentTimeStep)
{
    bool DoEmit = false;

    if(StartTime <= CurrentTime)
    {
        if(LatestEmitTime >= 0)
        {
            if(LatestEmitTime+TimeSpan <= CurrentTime)
            {
                DoEmit = true;
            }
        }else{
            DoEmit = true;
        }
    }

    std::list<PPlib::ParticleData*> tmpParticleList;
    if(DoEmit)
    {
        try
        {
            for(int i = 0; i < GetSumStartPoints(); i++)
            {
                PPlib::ParticleData* tmp = new ParticleData;
                tmpParticleList.push_back(tmp);
            }
        }
        catch(std::bad_alloc)
        {
            for(std::list<PPlib::ParticleData*>::iterator it = tmpParticleList.begin(); it != tmpParticleList.end(); ++it)
            {
                delete *it;
            }
            std::cerr<<"faild to allocate memory for ParticleData. particle emittion is skiped for this time step"<<std::endl;
            return;
        }

        std::vector<REAL_TYPE>           Coords;
        GetGridPointCoord(Coords);

        std::vector<REAL_TYPE>::iterator itCoords = Coords.begin();
        for(std::list<PPlib::ParticleData*>::iterator it = tmpParticleList.begin(); it != tmpParticleList.end(); ++it)
        {
            (*it)->StartPointID1 = ID[0];
            (*it)->StartPointID2 = ID[1];
            (*it)->ParticleID    = LatestEmitParticleID++;
            (*it)->StartTime     = CurrentTime;
            (*it)->LifeTime      = ParticleLifeTime;
            //放出直後の時刻は不正値(-1.0)を入れておく
            (*it)->CurrentTime = -1.0;
            //放出されたタイミングでは粒子移動の計算前なのでCurrentTimeStep -1の値とする
            //PP_Transport内で計算されたタイミングで更新後の時刻、タイムステップが代入される
            (*it)->CurrentTimeStep = CurrentTimeStep-1;
            (*it)->x               = (*itCoords++);
            (*it)->y               = (*itCoords++);
            (*it)->z               = (*itCoords++);
            (*it)->BlockID         = DSlib::DecompositionManager::GetInstance()->FindBlockIDByCoordLinear((*it)->x, (*it)->y, (*it)->z);
        }
        this->LatestEmitTime = CurrentTime;
        ParticleList->splice(ParticleList->end(), tmpParticleList);
    }
}

void StartPoint::DividePoints(std::vector<REAL_TYPE>* Coords, const int& NumPoints, const REAL_TYPE Coord1[3], const REAL_TYPE Coord2[3])
{
    if(NumPoints == 1)
    {
        Coords->push_back(Coord1[0]);
        Coords->push_back(Coord1[1]);
        Coords->push_back(Coord1[2]);
    }else{
        for(int i = 0; i < NumPoints; i++)
        {
            REAL_TYPE x = Coord1[0] == Coord2[0] ? Coord1[0] : Coord1[0]+(Coord2[0]-Coord1[0])/(NumPoints-1)*i;
            Coords->push_back(x);
            REAL_TYPE y = Coord1[1] == Coord2[1] ? Coord1[1] : Coord1[1]+(Coord2[1]-Coord1[1])/(NumPoints-1)*i;
            Coords->push_back(y);
            REAL_TYPE z = Coord1[2] == Coord2[2] ? Coord1[2] : Coord1[2]+(Coord2[2]-Coord1[2])/(NumPoints-1)*i;
            Coords->push_back(z);
        }
    }
}

bool StartPoint::CheckReleaseTime(const double& CurrentTime)
{
    if(ReleaseTime <= 0)
    {
        return false;
    }else{
        return (StartTime+ReleaseTime) > CurrentTime ? false : true;
    }
}
} // namespace PPlib