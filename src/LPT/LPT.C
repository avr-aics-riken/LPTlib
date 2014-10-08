/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <algorithm>
#include <list>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <omp.h>

#include "LPT.h"
#include "DSlib.h"
#include "PPlib.h"
#include "Communicator.h"
#include "StartPointAll.h"
#include "DecompositionManager.h"
#include "ParticleData.h"
#include "Cache.h"
#include "CommDataBlock.h"
#include "FileManager.h"
#include "LPT_ParticleOutput.h"
#include "LPT_LogOutput.h"
#include "PP_Transport.h"
#include "PMlibWrapper.h"
#include "SimpleStartPointFactory.h"

namespace LPT
{
  std::ostream & operator <<(std::ostream & stream, LPT_InitializeArgs args)
  {
    stream << std::endl;
    stream << "Nx,       Ny,       Nz      = " << args.Nx << "," << args.Ny << "," << args.Nz << std::endl;
    stream << "NPx,      NPy,      NPz     = " << args.NPx << "," << args.NPy << "," << args.NPz << std::endl;
    stream << "NBx,      NBy,      NBz     = " << args.NBx << "," << args.NBy << "," << args.NBz << std::endl;
    stream << "dx,       dy,       dz      = " << args.dx << "," << args.dy << "," << args.dz << std::endl;
    stream << "OriginX,  OriginY,  OriginZ = " << args.OriginX << "," << args.OriginY << "," << args.OriginZ << std::endl;
    stream << "GuideCellSize = " << args.GuideCellSize;
    return stream;
  }
  std::istream & operator >>(std::istream & stream, LPT_InitializeArgs & args)
  {
    stream >> args.Nx >> args.Ny >> args.Nz;
    stream >> args.NPx >> args.NPy >> args.NPz;
    stream >> args.NBx >> args.NBy >> args.NBz;
    stream >> args.dx >> args.dy >> args.dz;
    stream >> args.OriginX >> args.OriginY >> args.OriginZ;
    stream >> args.GuideCellSize;

    return stream;
  }

  bool LPT::LPT_SetStartPoint(REAL_TYPE Coord1[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    StartPoints.push_back(PPlib::PointFactory::create(Coord1, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
  }

  bool LPT::LPT_SetStartPointLine(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int SumStartPoints, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    StartPoints.push_back(PPlib::LineFactory::create(Coord1, Coord2, SumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
  }

  bool LPT::LPT_SetStartPointRectangle(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    PPlib::StartPoint *tmpStartPoint = PPlib::RectangleFactory::create(Coord1, Coord2, NumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime);
    if (tmpStartPoint == NULL)
    {
      std::cerr << "PPlib::RectangleFactory::create failed"<<std::endl;
      return false;
    }else{
      StartPoints.push_back(tmpStartPoint);
      return true;
    }
  }

  bool LPT::LPT_SetStartPointCuboid(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    StartPoints.push_back(PPlib::CuboidFactory::create(Coord1, Coord2, NumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
  }

  bool LPT::LPT_SetStartPointCircle(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    StartPoints.push_back(PPlib::CircleFactory::create(Coord1, SumStartPoints, Radius, NormalVector, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
  }

  bool LPT::LPT_SetStartPointMoovingPoints(REAL_TYPE * Coord, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    return false;
  }


  int LPT::LPT_OutputParticleData()
  {
    if(!initialized) return 1;
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start(PM.tm_FileOutput);
    LPT_ParticleOutput::GetInstance()->SetParticles(&(ptrPPlib->Particles));
    LPT_ParticleOutput::GetInstance()->WriteRecordHeader();
    LPT_ParticleOutput::GetInstance()->WriteRecord();
    PM.stop(PM.tm_FileOutput);
    return 0;
  }

  int LPT::LPT_Initialize(LPT_InitializeArgs args)
  {
    if(initialized)
    {
      std::cerr << "LPT_Initialize() is called more than 1 time"<<std::endl;
      return 1;
    }
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.Initialize(args.PMlibOutputFileName, args.PMlibDetailedOutputFileName);

    PM.start(PM.tm_Initialize);
    FileManager::GetInstance()->SetBaseFileName(args.OutputFileName);
    LPT_LOG::GetInstance()->INFO("LPT_Initialize called");
    LPT_LOG::GetInstance()->INFO("Init args = ", args);

    NumParticleProcs = args.NumParticleProcs;
    OutputFileName = args.OutputFileName;

    ptrDSlib = DSlib::DSlib::GetInstance();
    ptrDSlib->Initialize(args.CacheSize, args.CommBufferSize);
    LPT_LOG::GetInstance()->LOG("DSlib instantiated");
    ptrDM = DSlib::DecompositionManager::GetInstance();
    LPT_LOG::GetInstance()->LOG("DecompositionManager instantiated");
    ptrDM->Initialize(args.Nx, args.Ny, args.Nz, args.NPx, args.NPy, args.NPz, args.NBx, args.NBy, args.NBz, args.OriginX, args.OriginY, args.OriginZ, args.dx, args.dy, args.dz, args.GuideCellSize);
    LPT_LOG::GetInstance()->LOG("DecompositionManager initialized");
    ptrPPlib = PPlib::PPlib::GetInstance();
    LPT_LOG::GetInstance()->LOG("PPlib instantiated");
    ptrComm = new DSlib::Communicator;
    LPT_LOG::GetInstance()->LOG("Communicator instantiated");
    LPT_ParticleOutput::GetInstance()->WriteFileHeader();
    LPT_LOG::GetInstance()->LOG("Write output file header done");

//d_bcvの30bit目を配列にする
//d_bcvはFFV-C内でセルの情報を保持しているビット配列
    int N = (ptrDM->GetSubDomainSizeX(MyRank) + args.GuideCellSize * 2) * (ptrDM->GetSubDomainSizeY(MyRank) + args.GuideCellSize * 2) * (ptrDM->GetSubDomainSizeZ(MyRank) + args.GuideCellSize * 2);
    Mask = new int[N];

    if(args.d_bcv != NULL) {
      for(int i = 0; i < N; ++i) {
        Mask[i] = ((((args.d_bcv)[i] >> 30) & 0x1) ? 1 : 0);
      }
    } else {
      for(int i = 0; i < N; ++i) {
        Mask[i] = 1;
      }
    }

//開始点の分散処理 
    ptrPPlib->StartPoints = StartPoints;

    // clear and minimize LPT.StartPoints
    {
      std::vector<PPlib::StartPoint *>().swap(StartPoints);
    }

    ptrPPlib->DistributeStartPoints(NumParticleProcs);
    LPT_LOG::GetInstance()->LOG("Distribute StartPoints done");

    if(ptrDSlib == NULL || ptrDM == NULL || ptrPPlib == NULL || ptrComm == NULL) {
      LPT_LOG::GetInstance()->ERROR("instantiation failed!!");
      return -1;
    }
    initialized=true;
    PM.stop(PM.tm_Initialize);
    return 0;
  }

  int LPT::LPT_Post(void)
  {
    if(!initialized) return 1;
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start(PM.tm_Post);
    delete ptrComm;
    delete [] Mask;

    PM.stop(PM.tm_Post);
    PM.Finalize();
    LPT_LOG::GetInstance()->INFO("LPT_Post finished.");
    return 0;
  }

  int LPT::LPT_CalcParticleData(LPT_CalcArgs args)
  {
    static bool error_messaged_loged=false;
    if(!initialized)
    {
      if(!error_messaged_loged)
      {
        std::cerr << "LPT_CalcParticleData is called before LPT_Initialize()"<<std::endl;
        error_messaged_loged=true;
      }
      return 1;
    }
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start(PM.tm_PrepareCalc);
    CurrentTime = args.CurrentTime;
    CurrentTimeStep = args.CurrentTimeStep;
    LPT_LOG::GetInstance()->INFO("Current Time = ", args.CurrentTime);
    LPT_LOG::GetInstance()->INFO("Current Time Step = ", args.CurrentTimeStep);
    PM.stop(PM.tm_PrepareCalc);

    //寿命を過ぎた開始点を破棄
    ptrPPlib->DestroyExpiredStartPoints(args.CurrentTime);

    //新規粒子の放出
    //開始点がMovingPointsの場合は座標のアップデートも行う
    ptrPPlib->EmitNewParticles(args.CurrentTime, args.CurrentTimeStep);

    //寿命を過ぎた粒子を破棄
    ptrPPlib->DestroyExpiredParticles(args.CurrentTime);

    LPT_LOG::GetInstance()->INFO("Number of particles = ", ptrPPlib->Particles.size());

    //粒子位置および周辺のデータブロックをRequestQueueに登録
    ptrPPlib->MakeRequestQueues(ptrDSlib);
    
    //このタイムステップ中に必要な転送回数(転送量の最大値/キャッシュサイズ)を計算
    int GlobalNumComm = ptrDSlib->CalcNumComm(ptrComm);

    PPlib::PP_Transport Transport;
    //キャッシュサイズでブロッキングしたループ
    for(int j = 0; j < GlobalNumComm; j++)
    {
      //キャッシュ領域を空けて、転送可能な量のデータブロックをリクエストする
      ptrDSlib->DiscardCacheEntry(j, ptrComm);

      //Alltoallで必要なブロック数を通信
      ptrComm->CommRequestsAlltoall();

      //P2P通信で必要なブロックIDを通信
      ptrComm->CommRequest(ptrDSlib);

      //データブロックの転送開始
      const int vlen = 3;
      std::list < DSlib::CommDataBlockManager * >SendBuff;
      std::list < DSlib::CommDataBlockManager * >RecvBuff;
      ptrComm->CommDataF2P(args.FluidVelocity, args.v00, Mask, vlen, &SendBuff, &RecvBuff);

      PM.start(PM.tm_CalcParticle);
      int polling_counter=NumPolling;
      //polling & calc PP_Transport
      #pragma omp parallel private(Transport)
      {
        std::list<std::list<PPlib::ParticleData*>*> calced;
        std::list<PPlib::ParticleData*> moved;
        #pragma omp single
        while(!RecvBuff.empty())
        {
          polling_counter--;
          for(std::list < DSlib::CommDataBlockManager * >::iterator it_RecvBuff = RecvBuff.begin(); it_RecvBuff != RecvBuff.end();)
          {
            //TODO
            // MPI_Test/MPI_Waitの切り替えをiteration回数ではなく
            // 送信完了の割合で指定できる方が良いかもしれない
            bool is_arived=false;
            if(polling_counter<1)
            {
              // 最後のPolling loopでは未受信のデータ転送を1つづつ完了させてから計算
              is_arived=(*it_RecvBuff)->Wait();
            }else{
              is_arived=(*it_RecvBuff)->Test();
            }
            if(is_arived)
            {
              long ArrivedBlockID = ptrDSlib->AddCachedBlocks((*it_RecvBuff), args.CurrentTime);
              delete(*it_RecvBuff);
              it_RecvBuff = RecvBuff.erase(it_RecvBuff);
              PM.start(PM.tm_PP_Transport);
              #pragma omp task firstprivate(ArrivedBlockID)
              {
                std::list<PPlib::ParticleData*>* work;
                work=ptrPPlib->Particles.find(ArrivedBlockID);
                if (work != NULL)
                {
                  for(std::list<PPlib::ParticleData*>::iterator it_Particle=work->begin();it_Particle != work->end(); )
                  {
                    int ierr = Transport.Calc(*it_Particle, args.deltaT, args.divT, args.v00, args.CurrentTime, args.CurrentTimeStep);
                    LPT_LOG::GetInstance()->LOG("return value from PP_Transport::Calc() = ", ierr);
                    if(ierr == 0|| ierr == 3||ierr==4 || ierr ==5){
                      ++it_Particle;
                    }else if(ierr == 1) {
                      LPT_LOG::GetInstance()->LOG("Delete particle due to out of bounds: ID = ", (*it_Particle)->GetAllID());
                      delete *it_Particle;
                      it_Particle=work->erase(it_Particle);
                    }else if(ierr == 2){
                      moved.push_back(*it_Particle);
                      it_Particle=work->erase(it_Particle);
                    }else {
                      ++it_Particle;
                      LPT_LOG::GetInstance()->ERROR("illegal return value from PP_Transport::Calc() : ParticleID = ", (*it_Particle)->GetAllID());
                    }
                  }
                  calced.push_back(work);  // !!error
                }
              } // omp end task
              PM.stop(PM.tm_PP_Transport);
            } else {
              ++it_RecvBuff;
            }
          }
        }//omp end single
#pragma omp single
        {
          PM.start(PM.tm_PP_Transport);
        }
        //計算済ブロックに含まれていた粒子をParticleContainerに戻す
        for(std::list<std::list<PPlib::ParticleData*>*>::iterator it_list=calced.begin();it_list!=calced.end();++it_list)
        {
          ptrPPlib->Particles.insert(*it_list);
        }
        for(std::list<PPlib::ParticleData*>::iterator it_Particle=moved.begin();it_Particle!=moved.end();++it_Particle)
        {
          ptrPPlib->Particles.insert(*it_Particle);
        }
        calced.clear();
        moved.clear();
#pragma omp single
        {
          PM.stop(PM.tm_PP_Transport);
        }

      }//omp end parallel
      //ここまでで計算できていなかった粒子を再計算
      PM.start(PM.tm_PP_Transport);
      long re_calced_particles=0;
      for(PPlib::ParticleContainer::iterator it_Particle = ptrPPlib->Particles.begin();it_Particle!=ptrPPlib->Particles.end();)
      {
        int ierr = Transport.Calc(*it_Particle, args.deltaT, args.divT, args.v00, args.CurrentTime, args.CurrentTimeStep);
        if(ierr == 0||ierr==3 || ierr == 4){
          re_calced_particles++;
          ++it_Particle;
        }else if(ierr == 1) {
          LPT_LOG::GetInstance()->LOG("Delete particle due to out of bounds: ID = ", (*it_Particle)->GetAllID());
          delete  *it_Particle;
          it_Particle=ptrPPlib->Particles.erase(it_Particle);
          re_calced_particles++;
        }else if(ierr == 2){
          LPT_LOG::GetInstance()->LOG("moved to another datablock: ID = ", (*it_Particle)->GetAllID());
          ptrPPlib->Particles.insert(*it_Particle);
          it_Particle=ptrPPlib->Particles.erase(it_Particle);
          re_calced_particles++;
        }else if(ierr == 5){
          ++it_Particle;
        }else {
          LPT_LOG::GetInstance()->ERROR("illegal return value from PP_Transport::Calc() : ParticleID = ", (*it_Particle)->GetAllID());
          ++it_Particle;
        }
      }
      LPT_LOG::GetInstance()->LOG("Number of Particle (re-calculated) = ", re_calced_particles);
      PM.stop(PM.tm_PP_Transport);
      DeleteCommBuff(&SendBuff, &RecvBuff);
      PM.stop(PM.tm_CalcParticle);
    } //転送回数のループ
    //キャッシュデータを全て削除
    ptrDSlib->PurgeAllCacheLists();
    return 0;
  }


  void LPT::DeleteCommBuff(std::list< DSlib::CommDataBlockManager* >* SendBuff, std::list< DSlib::CommDataBlockManager* >* RecvBuff)
  {
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start(PM.tm_DelSendBuff);
    for(std::list < DSlib::CommDataBlockManager* >::iterator it_SendBuff = SendBuff->begin(); it_SendBuff != SendBuff->end();) {
      MPI_Status status0;
      MPI_Status status1;
      MPI_Wait(&((*it_SendBuff)->Request0), &status0);
      MPI_Wait(&((*it_SendBuff)->Request1), &status1);
      delete *it_SendBuff;
      it_SendBuff = SendBuff->erase(it_SendBuff);
    }
    for(std::list < DSlib::CommDataBlockManager* >::iterator it_RecvBuff = RecvBuff->begin(); it_RecvBuff != RecvBuff->end();) {
      delete *it_RecvBuff;
      it_RecvBuff = RecvBuff->erase(it_RecvBuff);
    }
    PM.stop(PM.tm_DelSendBuff);
  }
} // namespace LPT
