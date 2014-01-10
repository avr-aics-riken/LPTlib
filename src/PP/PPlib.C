#include <mpi.h>
#include <algorithm>
#include <iomanip>
#include <cmath>

#include "PPlib.h"
#include "Interpolator.h"
#include "PP_Integrator.h"
#include "ParticleData.h"
#include "DecompositionManager.h"
#include "DSlib.h"
#include "StartPointAll.h"
#include "LPT_LogOutput.h"
#include "LPT_ParticleOutput.h"

namespace PPlib
{
  void PPlib::EmitNewParticles(const double &CurrentTime, const int &CurrentTimeStep)
  {
    std::list < ParticleData * > tmpParticles;
    for(std::vector < StartPoint * >::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
    {
      (*it)->UpdateStartPoint(CurrentTime);
      (*it)->EmitNewParticle(&tmpParticles, CurrentTime, CurrentTimeStep);
    }
    if(! tmpParticles.empty())
    {
      LPT::LPT_LOG::GetInstance()->INFO("Number of New Particles = ", tmpParticles.size());
      for(std::list<ParticleData * >::iterator it = tmpParticles.begin(); it!=tmpParticles.end();++it)
      {
        Particles.insert(*it);
      }
    }
  }

  void PPlib::MakeRequestQueues(DSlib::DSlib * ptrDSlib)
  {
    DSlib::DecompositionManager * ptrDM = DSlib::DecompositionManager::GetInstance();
    std::set < long >tmpIDs;

    //ブロックIDの集合(重複無し)を作る
    for(ParticleContainer::iterator it = Particles.begin(); it != Particles.end(); ++it) {
      tmpIDs.insert((*it)->BlockID);
    }

    //周辺のデータブロックを探す(元のデータブロックも含む)
    std::set < long >tmpIDs2;
    for(std::set < long >::iterator it = tmpIDs.begin(); it != tmpIDs.end(); ++it) {
      ptrDM->FindNeighborBlockID(*it, &tmpIDs2);
    }

    //RequestQueuesにコピー
    for(std::set < long >::iterator it = tmpIDs2.begin(); it != tmpIDs2.end(); ++it) {
      int SubDomainID = ptrDM->FindSubDomainIDByBlock(*it);

      ptrDSlib->AddRequestQueues(SubDomainID, *it);
    }
  }

  template < typename T > bool PPlib::isExpired(const double &CurrentTime, T * obj)
  {
    if(obj->GetLifeTime() <= 0) {
      return false;
    } else {
      return (obj->GetStartTime() + obj->GetLifeTime()) > CurrentTime ? false : true;
    }
  }

  void PPlib::DestroyExpiredParticles(const double &CurrentTime)
  {
    for( ParticleContainer::iterator it = Particles.begin(); it != Particles.end();) {
      if(isExpired(CurrentTime, *it)) {
        LPT::LPT_LOG::GetInstance()->INFO("Particle Deleted. ID= ", (*it)->GetID());
        delete *it;
        Particles.erase(it++);
      } else {
        ++it;
      }
    }
  }

  void PPlib::DestroyExpiredStartPoints(const double &CurrentTime)
  {
    for(std::vector < StartPoint * >::iterator it = StartPoints.begin(); it != StartPoints.end();) {
      if(isExpired(CurrentTime, (*it))) {
        LPT::LPT_LOG::GetInstance()->INFO("Start Point Deleted. ID= ", (*it)->GetID());
        delete *it;
        it = StartPoints.erase(it);
      } else {
        ++it;
      }
    }
    std::vector<StartPoint *>(StartPoints).swap(StartPoints);
  }

  void PPlib::DistributeStartPoints(const int &NParticleProcs)
  {
    LPT::LPT_LOG::GetInstance()->LOG("DistributeStartPoints() start");
    int MyRank;

    MPI_Comm_rank(MPI_COMM_WORLD, &MyRank); //粒子プロセス用にグループを分ける時は、コミュニケータを違うところから取得すること

    //開始点の総数を計算
    int TotalNumStartPoints = 0;

    for(std::vector < StartPoint * >::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it) {
      TotalNumStartPoints += (*it)->GetSumStartPoints();
    }

    //1プロセスあたりの平均開始点数を計算
    int AveNumStartPoints;

    if(TotalNumStartPoints < NParticleProcs) {
      AveNumStartPoints = 1;
    } else {
      AveNumStartPoints = TotalNumStartPoints / NParticleProcs;
    }

    LPT::LPT_LOG::GetInstance()->INFO("AveNumStartPoints = ", AveNumStartPoints);

    std::vector < StartPoint * >NewStartPoints;

    //平均を越えている開始点領域を分割 (分割後の開始点は破棄)
    for(std::vector < StartPoint * >::iterator it = StartPoints.begin(); it != StartPoints.end();) {
      std::vector < StartPoint * > tmpStartPoints;
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
    std::vector < StartPoint * >::iterator it1 = NewStartPoints.begin();
    std::vector < StartPoint * >::iterator it2 = NewStartPoints.begin();

    int tmpSumStartPoints = 0;

    for(int i = 0; i < NewStartPoints.size(); i++) {
      tmpSumStartPoints += (*it1)->GetSumStartPoints();
      if(tmpSumStartPoints > MyRank * AveNumStartPoints) {
        break;
      } else {
        ++it1;
      }
    }
    tmpSumStartPoints = 0;
    for(int i = 0; i < NewStartPoints.size(); i++) {
      tmpSumStartPoints += (*it2)->GetSumStartPoints();
      if(tmpSumStartPoints > (MyRank + 1) * AveNumStartPoints) {
        break;
      } else {
        ++it2;
      }
    }

    //最後のRankは残りの開始点を全て担当する
    /// @TODO 最後のRankの担当する開始点が多い場合は余り担当プロセスを増やして、そちらに渡す
    if(MyRank == NParticleProcs - 1) {
      it2 = NewStartPoints.end();
    }
    //自Rankが担当する開始点をStartPointsにコピー
    StartPoints.assign(it1, it2);

    // shrink to fit
    std::vector<StartPoint *>(StartPoints).swap(StartPoints);

    LPT::LPT_LOG::GetInstance()->LOG("Distribute StartPoints done");
    LPT::LPT_LOG::GetInstance()->INFO("Number of StartPoints for this Rank = ", StartPoints.size());

    //IDの設定
    int id[2] = { MyRank, 0 };
    for(std::vector < StartPoint * >::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it) {
      (*it)->SetID(id);
      (id[1])++;
      LPT::LPT_LOG::GetInstance()->INFO("", (*it));
    }

    //開始点の格子座標をTimeStep0の粒子として出力
    for(std::vector < StartPoint * >::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it) {
      std::list< ParticleData *> ParticleList;
      for(int i = 0; i < (*it)->GetSumStartPoints(); i++) {
        ParticleData *tmp = new ParticleData;
        ParticleList.push_back(tmp);
      }

      std::vector < REAL_TYPE > Coords;
      (*it)->GetGridPointCoord(Coords);
      std::vector < REAL_TYPE >::iterator itCoords = Coords.begin();

      for(std::list<ParticleData *>::iterator it_list = ParticleList.begin(); it_list != ParticleList.end(); ++it_list) {
        (*it_list)->StartPointID1   = (*it)->GetID1();
        (*it_list)->StartPointID2   = (*it)->GetID2();
        (*it_list)->ParticleID      = 0;
        (*it_list)->StartTime       = 0.0;
        (*it_list)->LifeTime        = 0.0;
        (*it_list)->CurrentTime     = 0.0;
        (*it_list)->CurrentTimeStep = 0;
        (*it_list)->x = (*itCoords++);
        (*it_list)->y = (*itCoords++);
        (*it_list)->z = (*itCoords++);
      }
      LPT::LPT_ParticleOutput::GetInstance()->SetParticles(&ParticleList);
      LPT::LPT_ParticleOutput::GetInstance()->WriteRecordHeader();
      LPT::LPT_ParticleOutput::GetInstance()->WriteRecord();
      for(std::list<ParticleData *>::iterator it_list = ParticleList.begin(); it_list != ParticleList.end(); ++it_list) {
        delete *it_list;
      }
    }

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
