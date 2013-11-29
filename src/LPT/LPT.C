#include <algorithm>
#include <list>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>
#include <iomanip>

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
    PMlibWrapper & PM = PMlibWrapper::GetInstance();
    PM.start(PM.tm_FileOutput);
    LPT_ParticleOutput::GetInstance()->SetParticles(&(ptrPPlib->Particles));
    LPT_ParticleOutput::GetInstance()->WriteRecordHeader();
    LPT_ParticleOutput::GetInstance()->WriteRecord();
    PM.stop(PM.tm_FileOutput);
    return 0;
  }

  int LPT::LPT_Initialize(LPT_InitializeArgs args)
  {
    PMlibWrapper & PM = PMlibWrapper::GetInstance();
    PM.Initialize(args.PMlibOutputFileName, args.PMlibDetailedOutputFileName);

    PM.start(PM.tm_Initialize);
    FileManager::GetInstance()->SetBaseFileName(args.OutputFileName);
    LPT_LOG::GetInstance()->INFO("LPT_Initialize called");
    LPT_LOG::GetInstance()->INFO("Init args = ", args);

    NumParticleProcs = args.NumParticleProcs;
    CacheSize = args.CacheSize;
    CommBufferSize = args.CommBufferSize;
    OutputFileName = args.OutputFileName;

    ptrDSlib = new DSlib::DSlib(args.CacheSize);
    LPT_LOG::GetInstance()->LOG("DSlib instantiated");
    ptrDM = DSlib::DecompositionManager::GetInstance();
    LPT_LOG::GetInstance()->LOG("DecompositionManager instantiated");
    ptrDM->Initialize(args.Nx, args.Ny, args.Nz, args.NPx, args.NPy, args.NPz, args.NBx, args.NBy, args.NBz, args.OriginX, args.OriginY, args.OriginZ, args.dx, args.dy, args.dz, args.GuideCellSize);
    LPT_LOG::GetInstance()->LOG("DecompositionManager initialized");
    ptrPPlib = new PPlib::PPlib;
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
    PM.stop(PM.tm_Initialize);
    return 0;
  }

  int LPT::LPT_Post(void)
  {
    PMlibWrapper & PM = PMlibWrapper::GetInstance();
    PM.start(PM.tm_Post);
    delete ptrComm;
    delete ptrPPlib;
    delete ptrDSlib;
    delete [] Mask;

    PM.stop(PM.tm_Post);
    PM.Finalize();
    LPT_LOG::GetInstance()->INFO("LPT_Post finished.");
    return 0;
  }

  int LPT::LPT_CalcParticleData(LPT_CalcArgs args)
  {
    PMlibWrapper & PM = PMlibWrapper::GetInstance();
    PM.start(PM.tm_PrepareCalc);
    CurrentTime = args.CurrentTime;
    CurrentTimeStep = args.CurrentTimeStep;
    LPT_LOG::GetInstance()->INFO("Current Time =", GetCurrentTime());
    LPT_LOG::GetInstance()->INFO("Current Time Step =", GetCurrentTimeStep());
    LPT_LOG::GetInstance()->INFO("Number of particles=", ptrPPlib->Particles.size());
    PPlib::PP_Transport Transport;
    PM.stop(PM.tm_PrepareCalc);

    //寿命を過ぎた開始点を破棄
    PM.start(PM.tm_DestroyStartPoints);
    ptrPPlib->DestroyExpiredStartPoints(GetCurrentTime());
    LPT_LOG::GetInstance()->LOG("destroy Start point done");
    PM.stop(PM.tm_DestroyStartPoints);

    //新規粒子の放出
    //開始点がMovingPointsの場合は座標のアップデートも行う
    PM.start(PM.tm_EmitParticle);
    ptrPPlib->EmitNewParticles(GetCurrentTime(), GetCurrentTimeStep());
    LPT_LOG::GetInstance()->LOG("Particle Emission done");
    PM.stop(PM.tm_EmitParticle);

    //寿命を過ぎた粒子を破棄
    PM.start(PM.tm_DestroyParticle);
    ptrPPlib->DestroyExpiredParticles(GetCurrentTime());
    LPT_LOG::GetInstance()->LOG("destroy Particle done");
    PM.stop(PM.tm_DestroyParticle);

    //粒子位置のデータブロックとその周辺のデータブロックを持つRankを調べて、RequestQueuesに必要なBlockIDを登録
    PM.start(PM.tm_MakeRequestQ);
    ptrPPlib->MakeRequestQueues(ptrDSlib);
    LPT_LOG::GetInstance()->LOG("make request queues done");
    PM.stop(PM.tm_MakeRequestQ);

    //このタイムステップ中に必要な転送回数(転送量の最大値/キャッシュサイズ)を計算
    PM.start(PM.tm_CalcNumComm);

    ///@attention  NumEntryCommBufferSize を越える数のデータブロックを
    //             1つの流体プロセスに対して要求した場合
    //             キャッシュ溢れが生じるので注意が必要

    int SumRequestCount = 0;

    for(int i = 0; i < ptrComm->GetNumProcs(); i++) {
      SumRequestCount += ptrDSlib->CountRequestQueues(i);
    }

    int NumEntryCacheSize = CacheSize * 1024 * 1024 / sizeof(DSlib::DataBlock);
    int NumEntryCommBufferSize = CommBufferSize / sizeof(DSlib::DataBlock);
    int NumComm = 0;

    if(SumRequestCount < NumEntryCacheSize)
    {
      //このステップでこのRankは1回しか転送が要らない
      NumComm = 1;
    } else { 
      //このステップでこのRankは2回以上の転送が必要
      NumComm = 1 + (SumRequestCount - NumEntryCacheSize) / NumEntryCommBufferSize;
      if(1 > (SumRequestCount - NumEntryCacheSize) / NumEntryCommBufferSize) {
        NumComm++;
      }
    }
    //TODO Calc～のセクションをDSlibのメソッドに変更
    PM.stop(PM.tm_CalcNumComm);

    //Rank毎に計算した転送回数の最大値をAllreduceで集計
    PM.start(PM.tm_CommNumComm);
    int GlobalNumComm;

    MPI_Allreduce(&NumComm, &GlobalNumComm, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    LPT_LOG::GetInstance()->INFO("NumComm = ", NumComm);
    PM.stop(PM.tm_CommNumComm);

    std::list < PPlib::ParticleData *> WorkParticles;
    //キャッシュサイズでブロッキングしたループ
    for(int j = 0; j < GlobalNumComm; j++) {
      PM.start(PM.tm_PrepareComm);
      int MaxEntry = NumEntryCacheSize; //1回目の転送サイズはCacheSize以下

      if(j > 0) {
        MaxEntry = NumEntryCommBufferSize;  //2回目以降はCommBufferSize分のキャッシュを捨てて、転送処理を行う
        ptrDSlib->PurgeCachedBlocks(MaxEntry);
      }

      int NumRequest = 0;
      for(int i = 0; i < ptrComm->GetNumProcs(); i++) {
        NumRequest += ptrDSlib->CountRequestQueues(i);
        if(NumRequest > MaxEntry) {
          break;
        }
        ptrComm->MakeSendRequestCounts(i, ptrDSlib);
      }
      PM.stop(PM.tm_PrepareComm);

      //Alltoallで必要なブロック数を通信
      PM.start(PM.tm_AlltoAllRequest);
      ptrComm->CommRequestsAlltoall();
      LPT_LOG::GetInstance()->LOG("AlltoAll request done");
      PM.stop(PM.tm_AlltoAllRequest);

      //P2P通信で必要なブロックIDを通信
      PM.start(PM.tm_P2PRequest);
      ptrComm->CommRequest(ptrDSlib);
      LPT_LOG::GetInstance()->LOG("P2P request done");
      PM.stop(PM.tm_P2PRequest);

      //データブロックの転送開始
      PM.start(PM.tm_CommDataF2P);
      std::list < DSlib::CommDataBlockManager * >SendBuff;
      std::list < DSlib::CommDataBlockManager * >RecvBuff;
      int vlen = 3;
      ptrComm->CommDataF2P(args.FluidVelocity, args.v00, Mask, vlen, &SendBuff, &RecvBuff);
      PM.stop(PM.tm_CommDataF2P);

      PM.start(PM.tm_CalcParticle);
      //polling & calc PP_Transport
      for(int i = 0; i < NumPolling; i++) {
        for(std::list < DSlib::CommDataBlockManager * >::iterator it_RecvBuff = RecvBuff.begin(); it_RecvBuff != RecvBuff.end();) {
          PM.start(PM.tm_MPI_Test);
          //TODO CommDataBlockManager->is_arrived() としてくくり出す
          MPI_Status status;
          // MPI_Test後に flag = 0であればデータは未着
          int flag0 = 0;
          int flag1 = 0;
          if(MPI_SUCCESS != MPI_Test(&((*it_RecvBuff)->Request0), &flag0, &status)) {
            LPT_LOG::GetInstance()->ERROR("MPI_Test for Data Failed");
          }
          if(MPI_SUCCESS != MPI_Test(&((*it_RecvBuff)->Request1), &flag1, &status)) {
            LPT_LOG::GetInstance()->ERROR("MPI_Test for Header Failed");
          }
          PM.stop(PM.tm_MPI_Test);

          if(flag0 && flag1) {
            PM.start(PM.tm_AddCache);
            ptrDSlib->AddCachedBlocks((*it_RecvBuff), GetCurrentTime());
            long ArrivedBlockID=(*it_RecvBuff)->Header->BlockID;
            LPT_LOG::GetInstance()->LOG("Arrived Block = ", ArrivedBlockID);
            delete(*it_RecvBuff);
            it_RecvBuff = RecvBuff.erase(it_RecvBuff);
            PM.stop(PM.tm_AddCache);

            PM.start(PM.tm_PP_Transport);
            std::pair< std::multimap< long, PPlib::ParticleData*>::iterator, std::multimap<long, PPlib::ParticleData*>::iterator > \
              range = ptrPPlib->Particles.equal_range(ArrivedBlockID);
            for(std::multimap<long, PPlib::ParticleData*>::iterator it_Particle=range.first;it_Particle != range.second;){
              int ierr = Transport.Calc((*it_Particle).second, args.deltaT, args.divT, args.v00, ptrDSlib, CurrentTime, CurrentTimeStep);
              if(ierr == 1) {
                LPT_LOG::GetInstance()->INFO("Delete particle due to out of bounds: ID = ", (*it_Particle).second->GetAllID());
                delete (*it_Particle).second;
                ptrPPlib->Particles.erase(it_Particle++);
              }else if(ierr == 2){
                ptrPPlib->Particles.insert(std::make_pair((*it_Particle).second->BlockID, (*it_Particle).second));
                ptrPPlib->Particles.erase(it_Particle++);
              }else if(ierr == 3){
                WorkParticles.push_back((*it_Particle).second);
                ptrPPlib->Particles.erase(it_Particle++);
              }else if(ierr ==0 || ierr ==4){
                ++it_Particle;
              }else{
                LPT_LOG::GetInstance()->ERROR("illegal return value from PP_Transport::Calc() : ParticleID = ", (*it_Particle).second->GetAllID());
              }
            }
            PM.stop(PM.tm_PP_Transport);
            if(RecvBuff.empty()) break;
          } else {
            ++it_RecvBuff;
          }
        }
      }

      // 未受信のデータ到着を待ちつつ計算
      LPT_LOG::GetInstance()->LOG("Polling loop exited");
      for(std::list < DSlib::CommDataBlockManager * >::iterator it_RecvBuff = RecvBuff.begin(); it_RecvBuff != RecvBuff.end();) {
        PM.start(PM.tm_MPI_Wait);
        MPI_Status status;

        if(MPI_SUCCESS != MPI_Wait(&((*it_RecvBuff)->Request0), &status)) {
          LPT_LOG::GetInstance()->ERROR("MPI_Wait 1 Failed");
        }
        if(MPI_SUCCESS != MPI_Wait(&((*it_RecvBuff)->Request1), &status)) {
          LPT_LOG::GetInstance()->ERROR("MPI_Wait 2 Failed");
        }
        PM.stop(PM.tm_MPI_Wait);

        PM.start(PM.tm_AddCache);
        ptrDSlib->AddCachedBlocks((*it_RecvBuff), GetCurrentTime());
        long ArrivedBlockID=(*it_RecvBuff)->Header->BlockID;
        LPT_LOG::GetInstance()->LOG("Arrived Block = ", ArrivedBlockID);
        delete(*it_RecvBuff);
        it_RecvBuff = RecvBuff.erase(it_RecvBuff);
        PM.stop(PM.tm_AddCache);
        //到着した粒子の移動を計算
        PM.start(PM.tm_PP_Transport);
        std::pair< std::multimap< long, PPlib::ParticleData*>::iterator, std::multimap<long, PPlib::ParticleData*>::iterator > range
          = ptrPPlib->Particles.equal_range(ArrivedBlockID);
        for(std::multimap<long, PPlib::ParticleData*>::iterator it_Particle=range.first;it_Particle != range.second;){
          int ierr = Transport.Calc((*it_Particle).second, args.deltaT, args.divT, args.v00, ptrDSlib, CurrentTime, CurrentTimeStep);
          if(ierr == 1) {
            LPT_LOG::GetInstance()->INFO("Delete particle due to out of bounds: ID = ", (*it_Particle).second->GetAllID());
            delete (*it_Particle).second;
            ptrPPlib->Particles.erase(it_Particle++);
          }else if(ierr == 2){
            ptrPPlib->Particles.insert(std::make_pair((*it_Particle).second->BlockID, (*it_Particle).second));
            ptrPPlib->Particles.erase(it_Particle++);
          }else if(ierr == 3){
            WorkParticles.push_back((*it_Particle).second);
            ptrPPlib->Particles.erase(it_Particle++);
          }else if(ierr == 0||ierr==4){
            ++it_Particle;
          }else {
            LPT_LOG::GetInstance()->ERROR("illegal return value from PP_Transport::Calc() : ParticleID = ", (*it_Particle).second->GetAllID());
          }
        }

        PM.stop(PM.tm_PP_Transport);
      }
      PM.start(PM.tm_PP_Transport);
      //再計算が必要な粒子の移動を計算
      for(std::list<PPlib::ParticleData *>::iterator it_Particle=WorkParticles.begin();it_Particle!=WorkParticles.end();)
      {
        int ierr = Transport.Calc((*it_Particle), args.deltaT, args.divT, args.v00, ptrDSlib, CurrentTime, CurrentTimeStep);
        if(ierr == 1) {
          LPT_LOG::GetInstance()->INFO("Delete particle due to out of bounds: ID = ", (*it_Particle)->GetAllID());
          delete (*it_Particle);
        }else {
          ptrPPlib->Particles.insert(std::make_pair((*it_Particle)->BlockID, (*it_Particle)));
        }
        it_Particle=WorkParticles.erase(it_Particle);
      }
      PM.stop(PM.tm_PP_Transport);

      PM.start(PM.tm_DelSendBuff);
      //送信バッファの解放 
      for(std::list < DSlib::CommDataBlockManager * >::iterator it_SendBuff = SendBuff.begin(); it_SendBuff != SendBuff.end();) {
        MPI_Status status0;
        MPI_Status status1;
        MPI_Wait(&((*it_SendBuff)->Request0), &status0);
        MPI_Wait(&((*it_SendBuff)->Request1), &status1);
        delete(*it_SendBuff);
        it_SendBuff = SendBuff.erase(it_SendBuff);
      }
      PM.stop(PM.tm_DelSendBuff);
      PM.stop(PM.tm_CalcParticle);
    } //転送回数のループ
#ifdef DEBUG
    LPT_LOG::GetInstance()->INFO("WorkParticles.size() = ", WorkParticles.size());
    for(std::multimap<long, PPlib::ParticleData*>::iterator it_Particle=ptrPPlib->Particles.begin();it_Particle != ptrPPlib->Particles.end();++it_Particle){
      if ((*it_Particle).second->CurrentTimeStep <CurrentTimeStep)
      {
        LPT_LOG::GetInstance()->INFO("This particle is not calculated: ID = ", (*it_Particle).second->GetAllID());
        LPT_LOG::GetInstance()->INFO("Particle Current Time Step = ", (*it_Particle).second->CurrentTimeStep);
      }
    }
#endif
    PM.start(PM.tm_Discard_Cache);
    //キャッシュデータを全て削除
    ptrDSlib->PurgeAllCacheLists();
    PM.stop(PM.tm_Discard_Cache);
      


    return 0;
  }
} // namespace LPT
