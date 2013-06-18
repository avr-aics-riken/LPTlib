#include <algorithm>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "LPT.h"
#include "DSlib.h"
#include "PPlib.h"
#include "Communicator.h"
#include "StartPoint.h"
#include "Line.h"
#include "Rectangle.h"
#include "Cuboid.h"
#include "Circle.h"
#include "DecompositionManager.h"
#include "ParticleData.h"
#include "Cache.h"
#include "CommDataBlock.h"
#include "FileManager.h"
#include "LPT_ParticleOutput.h"
#include "LPT_LogOutput.h"
#include "PP_Transport.h"
#include "PMlibWrapper.h"

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

  //TODO この辺のメソッド内の処理をHogeSimpleFactory classへ変更
  bool LPT::LPT_SetStartPoint(REAL_TYPE Coord1[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    // StartPoints.push_back(PPlib::StartPointSimpleFactory(Coord1, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime)); みたいなイメージ
    PPlib::StartPoint * tmpStartPoint = new PPlib::StartPoint;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetSumStartPoints(1);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);
    StartPoints.push_back(tmpStartPoint);
    return true;
  }

  bool LPT::LPT_SetStartPointMoovingPoints(REAL_TYPE * Coord, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    return false;
  }

  bool LPT::LPT_SetStartPointLine(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int SumStartPoints, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    PPlib::Line * tmpStartPoint = new PPlib::Line;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetCoord2(Coord2);
    tmpStartPoint->SetSumStartPoints(SumStartPoints);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);
    StartPoints.push_back(tmpStartPoint);
    return true;
  }

  bool LPT::LPT_SetStartPointRectangle(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    if(Coord1[0] == Coord2[0]) {
      if(NumStartPoints[0] != 1)
        return false;
    } else if(Coord1[1] == Coord2[1]) {
      if(NumStartPoints[1] != 1)
        return false;
    } else if(Coord1[2] == Coord2[2]) {
      if(NumStartPoints[2] != 1)
        return false;
    } else {
      return false;
    }

    PPlib::Rectangle * tmpStartPoint = new PPlib::Rectangle;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetCoord2(Coord2);
    tmpStartPoint->SetSumStartPoints(NumStartPoints[0] * NumStartPoints[1] * NumStartPoints[2]);
    tmpStartPoint->SetNumStartPoints(NumStartPoints);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);
    StartPoints.push_back(tmpStartPoint);
    return true;
  }

  bool LPT::LPT_SetStartPointCuboid(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    PPlib::Rectangle * tmpStartPoint = new PPlib::Rectangle;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetCoord2(Coord2);
    tmpStartPoint->SetSumStartPoints(NumStartPoints[0] * NumStartPoints[1] * NumStartPoints[2]);
    tmpStartPoint->SetNumStartPoints(NumStartPoints);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);
    StartPoints.push_back(tmpStartPoint);
    return true;
  }

  bool LPT::LPT_SetStartPointCircle(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    PPlib::Circle * tmpStartPoint = new PPlib::Circle;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetSumStartPoints(SumStartPoints);
    tmpStartPoint->SetRadius(Radius);
    tmpStartPoint->SetNormalVector(NormalVector);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);
    if(tmpStartPoint->Initialize()) {
      StartPoints.push_back(tmpStartPoint);
      return true;
    } else {
      delete tmpStartPoint;

      return false;
    }
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

    //粒子位置のデータブロックとそのデータブロックを持つRankを調べて、RequestQueuesに必要なBlockIDを放り込む
    PM.start(PM.tm_MakeRequestQ);
    ptrPPlib->MakeRequestQueues(ptrDM, ptrDSlib);
    LPT_LOG::GetInstance()->LOG("make request queues done");
    PM.stop(PM.tm_MakeRequestQ);

    //ブロックID順に粒子データをソート
    PM.start(PM.tm_SortParticle);
    (ptrPPlib->Particles).sort(PPlib::CompareBlockID());
    LPT_LOG::GetInstance()->LOG("sort particles done");
    PM.stop(PM.tm_SortParticle);

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

    if(SumRequestCount < NumEntryCacheSize) { //このステップでこのRankは1回しか転送が要らない
      NumComm = 1;
    } else {  //このステップでこのRankは2回以上の転送が必要
      NumComm = 1 + (SumRequestCount - NumEntryCacheSize) / NumEntryCommBufferSize;
      if(1 > (SumRequestCount - NumEntryCacheSize) / NumEntryCommBufferSize) {
        NumComm++;
      }
    }
    // Calc～のセクションをDSlibのメソッドに変更
    PM.stop(PM.tm_CalcNumComm);

    //Rank毎に計算した転送回数の最大値をAllreduce
    PM.start(PM.tm_CommNumComm);
    int GlobalNumComm;  //TODO メンバ変数に変更

    MPI_Allreduce(&NumComm, &GlobalNumComm, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    LPT_LOG::GetInstance()->INFO("NumComm = ", NumComm);
    PM.stop(PM.tm_CommNumComm);

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
      //TODO DSlib行き
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
      int vlen = 3;

      std::list < DSlib::CommDataBlockManager * >SendBuff;
      std::list < DSlib::CommDataBlockManager * >RecvBuff;

      ptrComm->CommDataF2P(args.FluidVelocity, args.v00, Mask, vlen, ptrDM, &SendBuff, &RecvBuff);
      PM.stop(PM.tm_CommDataF2P);

      PM.start(PM.tm_CalcParticle);
      //polling & calc PP_Transport
      for(int i = 0; i < NumPolling; i++) {
        for(std::list < DSlib::CommDataBlockManager * >::iterator it2 = RecvBuff.begin(); it2 != RecvBuff.end();) {
          PM.start(PM.tm_MPI_Test);
          MPI_Status status;

          // MPI_Test後に flag = 0であればデータは未着
          int flag0 = 0;
          int flag1 = 0;

          if(MPI_SUCCESS != MPI_Test(&((*it2)->Request0), &flag0, &status)) {
            LPT_LOG::GetInstance()->ERROR("MPI_Test for Data Failed");
          }
          if(MPI_SUCCESS != MPI_Test(&((*it2)->Request1), &flag1, &status)) {
            LPT_LOG::GetInstance()->ERROR("MPI_Test for Header Failed");
          }
          PM.stop(PM.tm_MPI_Test);

          if(flag0 && flag1) {
            PM.start(PM.tm_AddCache);
            ptrDSlib->AddCachedBlocks(*(*it2), GetCurrentTime());
            LPT_LOG::GetInstance()->LOG("Arrived Block = ", (*it2)->Header->BlockID);
            PM.stop(PM.tm_AddCache);

            PM.start(PM.tm_MoveParticle);
            ptrPPlib->MoveParticleByBlockID((*it2)->Header->BlockID);
            (*it2)->Buff = NULL;
            delete(*it2)->Header;
            delete(*it2);
            it2 = RecvBuff.erase(it2);
            PM.stop(PM.tm_MoveParticle);

            PM.start(PM.tm_PP_Transport);
            for(std::list < PPlib::ParticleData * >::iterator it3 = ptrPPlib->WorkingParticles.begin(); it3 != ptrPPlib->WorkingParticles.end();) {
              int ierr = Transport.Calc(*it3, args.deltaT, args.divT, args.v00, ptrDSlib, CurrentTime, CurrentTimeStep);

              if(ierr == 0 || ierr == -1) {
                ptrPPlib->CalcedParticles.push_back(*it3);
                it3 = ptrPPlib->WorkingParticles.erase(it3);
              } else if(ierr == -10) {
                LPT_LOG::GetInstance()->LOG("Delete particle due to out of bounds: ID = ", (*it3)->GetAllID());
                delete *it3;

                it3 = ptrPPlib->WorkingParticles.erase(it3);
              } else {
                LPT_LOG::GetInstance()->LOG("Pending calcuration for Particle ID = ", (*it3)->GetAllID());
                LPT_LOG::GetInstance()->LOG("PP_Transport returns = ", ierr);
                it3++;
              }
            }
            PM.stop(PM.tm_PP_Transport);
            if(RecvBuff.size() == 0) {
              break;
            }
          } else {
            it2++;
          }
        }
      }

      //全ての受信を完了させる
      //受信したデータをCachedBlocksに格納し、そのデータブロック内に存在する粒子データを
      //PPlib->ParticlesからPPlib->WorkingParticlesへ移動させる
      LPT_LOG::GetInstance()->LOG("Polling loop exited");
      for(std::list < DSlib::CommDataBlockManager * >::iterator it2 = RecvBuff.begin(); it2 != RecvBuff.end();) {
        PM.start(PM.tm_MPI_Wait);
        MPI_Status status;

        if(MPI_SUCCESS != MPI_Wait(&((*it2)->Request0), &status)) {
          LPT_LOG::GetInstance()->ERROR("MPI_Wait 1 Failed");
        }
        if(MPI_SUCCESS != MPI_Wait(&((*it2)->Request1), &status)) {
          LPT_LOG::GetInstance()->ERROR("MPI_Wait 2 Failed");
        }
        PM.stop(PM.tm_MPI_Wait);

        PM.start(PM.tm_AddCache);
        ptrDSlib->AddCachedBlocks(*(*it2), GetCurrentTime());
        PM.stop(PM.tm_AddCache);

        PM.start(PM.tm_MoveParticle);
        ptrPPlib->MoveParticleByBlockID((*it2)->Header->BlockID);
        (*it2)->Buff = NULL;
        delete(*it2)->Header;
        delete(*it2);
        it2 = RecvBuff.erase(it2);
        PM.stop(PM.tm_MoveParticle);
      }

      PM.start(PM.tm_PP_Transport);
      //未計算の粒子を全て計算
      for(std::list < PPlib::ParticleData * >::iterator it3 = ptrPPlib->WorkingParticles.begin(); it3 != ptrPPlib->WorkingParticles.end();) {
        int ierr = Transport.Calc(*it3, args.deltaT, args.divT, args.v00, ptrDSlib, CurrentTime, CurrentTimeStep);

        if(ierr == 0 || ierr == -1) {
          ptrPPlib->CalcedParticles.push_back(*it3);
          it3 = ptrPPlib->WorkingParticles.erase(it3);
        } else if(ierr == -10) {
          LPT_LOG::GetInstance()->LOG("Delete particle due to out of bounds: ID = ", (*it3)->GetAllID());
          delete *it3;

          it3 = ptrPPlib->WorkingParticles.erase(it3);
        } else {
          LPT_LOG::GetInstance()->ERROR("PP_Transport failed at last calculation!! return value = ", ierr);
          it3++;
        }
      }
      PM.stop(PM.tm_PP_Transport);

      PM.start(PM.tm_DelSendBuff);
      //送信バッファの解放 
      for(std::list < DSlib::CommDataBlockManager * >::iterator it2 = SendBuff.begin(); it2 != SendBuff.end();) {
        MPI_Status status0;
        MPI_Status status1;

        MPI_Wait(&((*it2)->Request0), &status0);
        MPI_Wait(&((*it2)->Request1), &status1);
        delete(*it2)->Buff;
        delete(*it2)->Header;
        delete(*it2);
        it2 = SendBuff.erase(it2);
      }
      PM.stop(PM.tm_DelSendBuff);

      PM.stop(PM.tm_CalcParticle);
    } //転送回数のループ

    PM.start(PM.tm_Discard_Cache);
    //キャッシュデータを全て削除
    ptrDSlib->PurgeAllCacheLists();
    PM.stop(PM.tm_Discard_Cache);
    PM.start(PM.tm_ExchangeParticleContainers);
    ptrPPlib->ExchangeParticleContainers();
    PM.stop(PM.tm_ExchangeParticleContainers);

    return 0;
  }
} // namespace LPT
