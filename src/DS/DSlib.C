#include <iostream>
#include <algorithm>

#include "DSlib.h"
#include "LPT_LogOutput.h"

namespace DSlib
{
  void DSlib::DiscardCacheEntry(const int & j,Communicator * ptrComm )
  {
    int MaxEntry;
      
    if(j==0)
    {
      //1回目はCacheSizeまで転送可能
      MaxEntry = CacheSize * 1024 * 1024 / sizeof(DataBlock);
    }else{
      //2回目以降はCommBufferSizeまで転送可能
      MaxEntry = CommBufferSize / sizeof(DataBlock);
      PurgeCachedBlocks(MaxEntry);
    }

    int NumRequest = 0;
    for(int i = 0; i < ptrComm->GetNumProcs(); i++) {
      NumRequest += CountRequestQueues(i);
      if(NumRequest > MaxEntry) {
        break;
      }
      ptrComm->SetSendRequestCounts(i,CountRequestQueues(i));
    }
  }

  ///@brief キャッシュサイズと転送量を元に何回転送が必要か計算する
  ///@attention  NumEntryCommBufferSize を越える数のデータブロックを
  //             1つの流体プロセスに対して要求した場合
  //             キャッシュ溢れが生じるので注意が必要
  int DSlib::CalcNumComm(Communicator * ptrComm)
  {
    int SumRequestCount = 0;

    for(int i = 0; i < ptrComm->GetNumProcs(); i++) {
      SumRequestCount += CountRequestQueues(i);
    }

    int NumEntryCacheSize = CacheSize * 1024 * 1024 / sizeof(DataBlock);
    int NumEntryCommBufferSize = CommBufferSize / sizeof(DataBlock);
    int NumComm = 0;

    if(SumRequestCount < NumEntryCacheSize)
    {
      //1回の転送で全てのデータをキャッシュ領域に載せることができる
      NumComm = 1;
    } else { 
      //キャッシュから溢れるため2回以上に分けて転送する必要がある
      NumComm = 1 + (SumRequestCount - NumEntryCacheSize) / NumEntryCommBufferSize;
      if(1 > (SumRequestCount - NumEntryCacheSize) / NumEntryCommBufferSize) {
        NumComm++;
      }
    }
    return NumComm;
  }

  void DSlib::AddRequestQueues(const int &SubDomainID, const long &BlockID)
  {
    RequestQueues.at(SubDomainID)->push_back(BlockID);
  }

  void DSlib::DedupulicateRequestQueues()
  {
    for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it != RequestQueues.end(); ++it) {
      std::sort((*it)->begin(), (*it)->end());
      (*it)->erase(std::unique((*it)->begin(), (*it)->end()), (*it)->end());
      std::vector<long>(**it).swap(**it);
    }
  }

  void DSlib::MoveRequestQueues(const int &SubDomainID)
  {
    for(std::vector < long >::iterator it = RequestQueues.at(SubDomainID)->begin(); it != RequestQueues.at(SubDomainID)->end(); ++it) {
      RequestedBlocks.insert(*it);
    }
    std::vector<long>().swap(*(RequestQueues.at(SubDomainID)));
  }

  void DSlib::RemoveRequestedBlocks(const long &BlockID)
  {
    RequestedBlocks.erase(BlockID);
  }

  void DSlib::AddCachedBlocks(CommDataBlockManager * RecvData, const double &Time)
  {
    if(CachedBlocks.size() < (CacheSize * 1024 * 1024 / sizeof(DataBlock))) {
      DataBlock *tmp = new DataBlock;
      tmp->BlockID = RecvData->Header->BlockID;
      tmp->SubDomainID = RecvData->Header->SubDomainID;
      tmp->Time = Time;
      for(int i = 0; i < 3; i++) {
        tmp->Origin[i] = RecvData->Header->Origin[i];
        tmp->OriginCell[i] = RecvData->Header->OriginCell[i];
        tmp->BlockSize[i] = RecvData->Header->BlockSize[i];
        tmp->Pitch[i] = RecvData->Header->Pitch[i];
      }
      tmp->Data = RecvData->Buff;
      RecvData->Buff=NULL;

      Cache *tmp2 = new Cache;
      tmp2->BlockID = RecvData->Header->BlockID;
      tmp2->ptrData = tmp;
      CachedBlocks.push_back(tmp2);
    } else {
      LPT::LPT_LOG::GetInstance()->WARN("DataBlock Cache overflowed");
      LPT::LPT_LOG::GetInstance()->WARN("CachedBlocks size = ", CachedBlocks.size() );
      LPT::LPT_LOG::GetInstance()->WARN("Reserved cache size = ", (CacheSize * 1024 * 1024 / sizeof(DataBlock)));
    }

  }

  void DSlib::PurgeCachedBlocks(const int &NumEntry)
  {
    if(NumEntry >= CachedBlocks.size()) {
      for(std::CACHE_CONTAINER < Cache * >::iterator it = CachedBlocks.begin(); it != CachedBlocks.end(); ++it) {
        delete(*it);
      }
      std::CACHE_CONTAINER < Cache * >().swap(CachedBlocks);
      LPT::LPT_LOG::GetInstance()->LOG("All CachedBlocks is purged");
    } else {
      int DeleteCount = 0;

      std::CACHE_CONTAINER < Cache * >::iterator FirstEntry;
      for(std::CACHE_CONTAINER < Cache * >::reverse_iterator rit = CachedBlocks.rbegin(); rit != CachedBlocks.rend();) {
        if(DeleteCount == NumEntry) {
          break;
        }
        delete(*rit);
        FirstEntry = rit.base();
        DeleteCount++;
      }
      CachedBlocks.erase(FirstEntry, CachedBlocks.end());
      std::CACHE_CONTAINER < Cache * >(CachedBlocks).swap(CachedBlocks);
    }

  }

  void DSlib::PurgeAllCacheLists(void)
  {
    for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it != RequestQueues.end(); ++it) {
      std::vector<long>().swap(**it);
    }
    RequestedBlocks.clear();
    PurgeCachedBlocks(CachedBlocks.size());
  }

  int DSlib::Load(const long &BlockID, DataBlock ** DataBlock)
  {
    //CachedBlocksの中を探索
    for(std::CACHE_CONTAINER < Cache * >::iterator it = CachedBlocks.begin(); it != CachedBlocks.end(); ++it) {
      if((*it)->BlockID == BlockID) {
        *DataBlock = (*it)->ptrData;
        Cache *tmp_Cache = (*it);

        CachedBlocks.erase(it);
        CachedBlocks.push_front(tmp_Cache);
        return 0;
      }
    }

    //RequestedBlocksの中を探索
    if(RequestedBlocks.end() == RequestedBlocks.find(BlockID)) {
      LPT::LPT_LOG::GetInstance()->LOG("DataBlock is not arrived: ", BlockID);
      return 1;
    }
    //RequestQueueの中を探索
    for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it != RequestQueues.end(); ++it) {
      for(std::vector < long >::iterator it2 = (*it)->begin(); it2 != (*it)->end(); ++it2) {
        if((*it2) == BlockID) {
          LPT::LPT_LOG::GetInstance()->LOG("DataBlock is not requested at this time: ", BlockID);
          return 2;
        }
      }
    }
    //どこにも無い -> このタイムステップでの対象粒子の計算は中止
    LPT::LPT_LOG::GetInstance()->WARN("DataBlock is not requested at this time step: ", BlockID);
    return 4;
  }
} // namespace DSlib
