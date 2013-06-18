#include <iostream>
#include <algorithm>

#include "DSlib.h"
#include "LPT_LogOutput.h"

namespace DSlib
{
  void DSlib::AddRequestQueues(const int &SubDomainID, const long &BlockID)
  {
    RequestQueues.at(SubDomainID)->push_back(BlockID);
  }

  void DSlib::DedupulicateRequestQueues()
  {
    for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it != RequestQueues.end(); it++) {
      std::sort((*it)->begin(), (*it)->end());
      (*it)->erase(std::unique((*it)->begin(), (*it)->end()), (*it)->end());
    }
  }

  void DSlib::MoveRequestQueues(const int &SubDomainID)
  {
    for(std::vector < long >::iterator it = RequestQueues.at(SubDomainID)->begin(); it != RequestQueues.at(SubDomainID)->end(); it++) {
      RequestedBlocks.insert(*it);
    }
    RequestQueues.at(SubDomainID)->clear();

  }

  void DSlib::RemoveRequestedBlocks(const long &BlockID)
  {
    RequestedBlocks.erase(BlockID);
  }

  void DSlib::AddCachedBlocks(const CommDataBlockManager & RecvData, const double &Time)
  {
    if(CachedBlocks.size() < (CacheSize * 1024 * 1024 / sizeof(DataBlock))) {
      DataBlock *tmp = new DataBlock;

      tmp->BlockID = RecvData.Header->BlockID;
      tmp->SubDomainID = RecvData.Header->SubDomainID;
      tmp->Time = Time;
      for(int i = 0; i < 3; i++) {
        tmp->Origin[i] = RecvData.Header->Origin[i];
        tmp->OriginCell[i] = RecvData.Header->OriginCell[i];
        tmp->BlockSize[i] = RecvData.Header->BlockSize[i];
        tmp->Pitch[i] = RecvData.Header->Pitch[i];
      }
      tmp->Data = RecvData.Buff;

      Cache *tmp2 = new Cache;

      tmp2->BlockID = RecvData.Header->BlockID;
      tmp2->ptrData = tmp;
      CachedBlocks.push_back(tmp2);
    } else {
      std::cerr << "DataBlock Cache overflowed" << std::endl;
      std::cerr << "CachedBlocks size = " << CachedBlocks.size() << std::endl;
      std::cerr << "Reserved Cache Size = " << (CacheSize * 1024 * 1024 / sizeof(DataBlock)) << std::endl;
    }

  }

  void DSlib::PurgeCachedBlocks(const int &NumEntry)
  {
    if(NumEntry > CachedBlocks.size()) {
      for(std::CACHE_CONTAINER < Cache * >::iterator it = CachedBlocks.begin(); it != CachedBlocks.end(); it++) {
        delete(*it)->ptrData->Data;
        delete(*it)->ptrData;
        delete(*it);
      }
      CachedBlocks.clear();
      LPT::LPT_LOG::GetInstance()->LOG("All CachedBlocks is purged");
    } else {
      int DeleteCount = 0;

      std::CACHE_CONTAINER < Cache * >::iterator FirstEntry;
      for(std::CACHE_CONTAINER < Cache * >::reverse_iterator rit = CachedBlocks.rbegin(); rit != CachedBlocks.rend();) {
        if(DeleteCount == NumEntry) {
          break;
        }
        delete(*rit)->ptrData->Data;
        delete(*rit)->ptrData;
        delete(*rit);
        FirstEntry = rit.base();
        DeleteCount++;
      }
      CachedBlocks.erase(FirstEntry, CachedBlocks.end());
    }

  }

  void DSlib::PurgeAllCacheLists(void)
  {
    for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it != RequestQueues.end(); it++) {
      (*it)->clear();
    }
    RequestedBlocks.clear();
    PurgeCachedBlocks(CacheSize);
  }

  int DSlib::Load(const long &BlockID, DataBlock ** DataBlock)
  {
    //CachedBlocksの中を探索
    for(std::CACHE_CONTAINER < Cache * >::iterator it = CachedBlocks.begin(); it != CachedBlocks.end(); it++) {
      if((*it)->BlockID == BlockID) {
        *DataBlock = (*it)->ptrData;
        Cache *tmp_Cache = (*it);

        CachedBlocks.erase(it);
        CachedBlocks.push_front(tmp_Cache);
        return 0;
      }
    }

    LPT::LPT_LOG::GetInstance()->LOG("DataBlock was not found in Cache.  BlockID = ", BlockID);

    //RequestedBlocksの中を探索
    if(RequestedBlocks.end() == RequestedBlocks.find(BlockID)) {
      LPT::LPT_LOG::GetInstance()->LOG("Requested Block is not arrived at this time");
      return 1;
    }
    //RequestQueueの中を探索
    for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it != RequestQueues.end(); it++) {
      for(std::vector < long >::iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++) {
        if((*it2) == BlockID) {
          LPT::LPT_LOG::GetInstance()->LOG("Requested Block is not transferd in this step");
          return 2;
        }
      }
    }
    //どこにも無い -> このタイムステップでの対象粒子の計算は中止
    LPT::LPT_LOG::GetInstance()->WARN("exit DSlib::DSlib return value = -1");
    return -1;
  }
} // namespace DSlib
