/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <mpi.h>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <fstream>
#include <typeinfo>

#include "PPlib.h"
#include "Interpolator.h"
#include "PP_Integrator.h"
#include "ParticleData.h"
#include "DecompositionManager.h"
#include "DSlib.h"
#include "StartPointAll.h"
#include "LPT_LogOutput.h"
#include "PMlibWrapper.h"
#include "LPT.h"
#include "PDMlib.h"

namespace PPlib
{
void PPlib::WriteStartPoints(const std::string& filename, const REAL_TYPE& RefLength, const double& RefTime)
{
    std::ofstream startpoint(filename.c_str());
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
        startpoint<<(*it)->TextPrint(RefLength, RefTime);
    }
}

void PPlib::ReadStartPoints(const std::string& filename, const REAL_TYPE& RefLength, const double& RefTime)
{
    std::ifstream ifs(filename.c_str());
    std::string   startpoint;
    while(ifs)
    {
        std::getline(ifs, startpoint);
        if(startpoint == "Point")
        {
            Point* tmp = PointFactory(NULL, NULL, NULL, NULL, NULL);
            tmp->ReadText(ifs, RefLength, RefTime);
            StartPoints.push_back(tmp);
        }else if(startpoint == "Line"){
            Line* tmp = LineFactory(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            tmp->ReadText(ifs, RefLength, RefTime);
            StartPoints.push_back(tmp);
        }else if(startpoint == "Rectangle"){
            Rectangle* tmp = RectangleFactory(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            tmp->ReadText(ifs, RefLength, RefTime);
            StartPoints.push_back(tmp);
        }else if(startpoint == "Cuboid"){
            Cuboid* tmp = CuboidFactory(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            tmp->ReadText(ifs, RefLength, RefTime);
            StartPoints.push_back(tmp);
        }else if(startpoint == "Circle"){
            Circle* tmp = CircleFactory(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            tmp->ReadText(ifs, RefLength, RefTime);
            if(tmp->Initialize())
            {
                StartPoints.push_back(tmp);
            }
        }else if(startpoint == "MovingPoints"){
            MovingPoints* tmp = MovingPointsFactory(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            tmp->ReadText(ifs, RefLength, RefTime);
            StartPoints.push_back(tmp);
        }else{
            LPT::LPT_LOG::GetInstance()->WARN("unknown startpoint type : ", startpoint);
        }
        startpoint.erase();
    }
}

void PPlib::EmitNewParticles(const double& CurrentTime, const int& CurrentTimeStep)
{
    LPT::PMlibWrapper& PM = LPT::PMlibWrapper::GetInstance();
    PM.start("EmitParticle");
    std::list<ParticleData*> tmpParticles;
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
        (*it)->UpdateStartPoint(CurrentTime);
        (*it)->EmitNewParticle(&tmpParticles, CurrentTime, CurrentTimeStep);
    }
    if(!tmpParticles.empty())
    {
        LPT::LPT_LOG::GetInstance()->INFO("Number of New Particles = ", tmpParticles.size());
        for(std::list<ParticleData*>::iterator it = tmpParticles.begin(); it != tmpParticles.end(); ++it)
        {
            Particles.insert(*it);
        }
    }
    PM.stop("EmitParticle");
    LPT::LPT_LOG::GetInstance()->LOG("Particle Emission done");
}

void PPlib::MakeRequestQueues(DSlib::DSlib* ptrDSlib)
{
    LPT::PMlibWrapper& PM              = LPT::PMlibWrapper::GetInstance();
    PM.start("MakeRequestQ");
    DSlib::DecompositionManager* ptrDM = DSlib::DecompositionManager::GetInstance();
    std::set<long> tmpIDs;

    //ブロックIDの集合(重複無し)を作る
    for(ParticleContainer::iterator it = Particles.begin(); it != Particles.end(); ++it)
    {
        tmpIDs.insert((*it)->BlockID);
    }

    //周辺のデータブロックを探す(元のデータブロックも含む)
    std::set<long> tmpIDs2;
    for(std::set<long>::iterator it = tmpIDs.begin(); it != tmpIDs.end(); ++it)
    {
        ptrDM->FindNeighborBlockID(*it, &tmpIDs2);
    }

    //RequestQueuesにコピー
    for(std::set<long>::iterator it = tmpIDs2.begin(); it != tmpIDs2.end(); ++it)
    {
        int SubDomainID = ptrDM->FindSubDomainIDByBlock(*it);

        ptrDSlib->AddRequestQueues(SubDomainID, *it);
    }
    LPT::LPT_LOG::GetInstance()->LOG("make request queues done");
    PM.stop("MakeRequestQ");
}

template<typename T>
bool PPlib::isExpired(const double& CurrentTime, T* obj)
{
    if(obj->GetLifeTime() <= 0)
    {
        return false;
    }else{
        return (obj->GetStartTime()+obj->GetLifeTime()) > CurrentTime ? false : true;
    }
}

void PPlib::DestroyExpiredParticles(const double& CurrentTime)
{
    LPT::PMlibWrapper& PM = LPT::PMlibWrapper::GetInstance();
    PM.start("DestroyParticle");
    for(ParticleContainer::iterator it = Particles.begin(); it != Particles.end();)
    {
        if(isExpired(CurrentTime, *it))
        {
            LPT::LPT_LOG::GetInstance()->INFO("Particle Deleted. ID= ", (*it)->GetID());
            delete *it;
            Particles.erase(it++);
        }else{
            ++it;
        }
    }
    LPT::LPT_LOG::GetInstance()->INFO("Number of particles = ", Particles.size());
    LPT::LPT_LOG::GetInstance()->LOG("destroy Particle done");
    PM.stop("DestroyParticle");
}

void PPlib::DestroyExpiredStartPoints(const double& CurrentTime)
{
    LPT::PMlibWrapper& PM = LPT::PMlibWrapper::GetInstance();
    PM.start("DestroyStartPoints");
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end();)
    {
        if(isExpired(CurrentTime, (*it)))
        {
            LPT::LPT_LOG::GetInstance()->INFO("Start Point Deleted. ID= ", (*it)->GetID());
            delete *it;
            it = StartPoints.erase(it);
        }else{
            ++it;
        }
    }
    std::vector<StartPoint*>(StartPoints).swap(StartPoints);
    PM.stop("DestroyStartPoints");
    LPT::LPT_LOG::GetInstance()->LOG("destroy Start point done");
}

void PPlib::DistributeStartPoints(const int& NParticleProcs)
{
    LPT::LPT_LOG::GetInstance()->LOG("DistributeStartPoints() start");
    int MyRank = LPT::MPI_Manager::GetInstance()->get_myrank_p();

    //開始点の総数を計算
    int TotalNumStartPoints = 0;

    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
        TotalNumStartPoints += (*it)->GetSumStartPoints();
    }
    LPT::LPT_LOG::GetInstance()->INFO("TotalNumStartPoints = ", TotalNumStartPoints);

    //1プロセスあたりの平均開始点数を計算
    int AveNumStartPoints = TotalNumStartPoints/NParticleProcs;
    if(AveNumStartPoints == 0)AveNumStartPoints = 1;
    LPT::LPT_LOG::GetInstance()->INFO("AveNumStartPoints = ", AveNumStartPoints);

    std::vector<StartPoint*> NewStartPoints;

    //平均を越えている開始点領域を分割 (分割前の開始点は破棄)
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end();)
    {
        std::vector<StartPoint*> tmpStartPoints;
        (*it)->Divider(&tmpStartPoints, AveNumStartPoints);
        NewStartPoints.insert(NewStartPoints.end(), tmpStartPoints.begin(), tmpStartPoints.end());
        delete *it;
        it = StartPoints.erase(it);
    }

    LPT::LPT_LOG::GetInstance()->LOG("divide StartPoints done");

    //開始点領域を大きい順にソート
    std::sort(NewStartPoints.begin(), NewStartPoints.end(), &StartPoint::isGreater);
    LPT::LPT_LOG::GetInstance()->INFO("total number of new StartPoint = ", NewStartPoints.size());

    //自Rankが担当する開始点領域の先頭と末尾を指すiteratorを決める
    std::vector<StartPoint*>::iterator it1 = NewStartPoints.begin();
    std::vector<StartPoint*>::iterator it2 = NewStartPoints.begin();

    int tmpSumStartPoints                  = 0;

    for(int i = 0; i < NewStartPoints.size(); i++)
    {
        tmpSumStartPoints += (*it1)->GetSumStartPoints();
        if(tmpSumStartPoints > MyRank*AveNumStartPoints)
        {
            break;
        }else{
            ++it1;
        }
    }
    tmpSumStartPoints = 0;
    for(int i = 0; i < NewStartPoints.size(); i++)
    {
        tmpSumStartPoints += (*it2)->GetSumStartPoints();
        if(tmpSumStartPoints > (MyRank+1)*AveNumStartPoints)
        {
            break;
        }else{
            ++it2;
        }
    }

    //最後のRankは残りの開始点を全て担当する
    /// @TODO 最後のRankの担当する開始点が多い場合は余り担当プロセスを増やして、そちらに渡す
    if(MyRank == NParticleProcs-1)
    {
        it2 = NewStartPoints.end();
    }
    //自Rankが担当する開始点をStartPointsにコピー
    StartPoints.assign(it1, it2);

    // shrink to fit
    std::vector<StartPoint*>(StartPoints).swap(StartPoints);

    LPT::LPT_LOG::GetInstance()->LOG("Distribute StartPoints done");
    LPT::LPT_LOG::GetInstance()->INFO("Number of StartPoints for this Rank = ", StartPoints.size());

    //IDの設定
    int id[2] = {MyRank, 0};
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
        (*it)->SetID(id);
        ++(id[1]);
    }
}

void PPlib::OutputStartPoints(const REAL_TYPE& RefLength)
{
    const int     TimeStep = 0;
    const double& Time     = 0.0;
    //開始点の総数をカウント
    size_t SumGridPoints   = 0;
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
        SumGridPoints += (*it)->GetSumStartPoints();
    }
    if(SumGridPoints <= 0)
    {
        return;
    }

    //REAL_TYPE型のデータメンバを出力
    REAL_TYPE* rwork = new REAL_TYPE[SumGridPoints*3];
    size_t     index = 0;
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
        std::vector<REAL_TYPE> Coords;
        (*it)->GetGridPointCoord(Coords);
        for(std::vector<REAL_TYPE> ::iterator it_coord = Coords.begin(); it_coord != Coords.end(); ++it_coord)
        {
            rwork[index++] = *it_coord*RefLength;
        }
    }
    PDMlib::PDMlib::GetInstance().Write("Coordinate", SumGridPoints, rwork, (REAL_TYPE*)NULL, 3, TimeStep, Time);

    for(size_t i = 0; i < SumGridPoints; i++)
    {
        rwork[i] = 0.0;
    }
    PDMlib::PDMlib::GetInstance().Write("Velocity", SumGridPoints, rwork, (REAL_TYPE*)NULL, 3, TimeStep, Time);
    delete[]rwork;

    //int 型のデータメンバを出力
    int* iwork = new int[SumGridPoints*3];
    index = 0;
    for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
        for(size_t i = 0; i < (*it)->GetSumStartPoints(); i++)
        {
            iwork[index++] = (*it)->GetID1();
            iwork[index++] = (*it)->GetID2();
            iwork[index++] = 0;
        }
    }
    PDMlib::PDMlib::GetInstance().Write("ID", SumGridPoints, iwork, (int*)NULL, 3, TimeStep, Time);
    delete[] iwork;

    //double 型のデータメンバを出力
    double* dwork = new double[SumGridPoints];
    index = 0;
    for(size_t i = 0; i < SumGridPoints; i++)
    {
        dwork[index++] = 0.0;
    }
    PDMlib::PDMlib::GetInstance().Write("StartTime", SumGridPoints, dwork, (double*)NULL, 1, TimeStep, Time);
    PDMlib::PDMlib::GetInstance().Write("LifeTime", SumGridPoints, dwork, (double*)NULL, 1, TimeStep, Time);
    delete[] dwork;
}

void PPlib::DetermineMigration()
{
///@TODO マイグレーションの条件が決まったら作成
}

void PPlib::MigrateParticle()
{
///@TODO マイグレーションの条件が決まったら作成
}
} // namespace PPlib