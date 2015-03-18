/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef DSLIB_D_SLIB_H
#define DSLIB_D_SLIB_H

#include <iostream>
#include <vector>
#include <set>
#include <deque>
#include <list>
#include <mpi.h>
#include <omp.h>

#include "DataBlock.h"
#include "Cache.h"
#include "CommDataBlock.h"
#include "Communicator.h"

namespace DSlib
{
//forward declaration
class CommDataBlockManager;
//!  @brief データブロックの管理を行なうクラス
//!
//! 転送待ちブロック、転送中ブロックおよび転送済ブロックの3つに分けてデータブロックの状態を管理する。
//！
class DSlib
{
    friend bool Communicator::CommRequest2(DSlib* ptrDSlib, std::list<CommDataBlockManager*>* RecvBuff, const int& fence);

private:
    //Singletonパターンを適用
    DSlib(){}
    DSlib(const DSlib& obj);
    DSlib& operator=(const DSlib& obj);
    ~DSlib()
    {
        for(std::vector<std::vector<long>*>::iterator it = RequestQueues.begin(); it != RequestQueues.end(); ++it)
        {
            delete *it;
        }
    }

public:
    static DSlib* GetInstance()
    {
        static DSlib instance;
        return &instance;
    }

    //! @param CacheSize [in] データブロックのキャッシュエントリ数
    void Initialize(const int& argCacheSize)
    {
        CacheSize = argCacheSize;
        for(int i = 0; i < LPT::MPI_Manager::GetInstance()->get_nproc_f(); i++)
        {
            std::vector<long>* tmp = new std::vector<long>;
            RequestQueues.push_back(tmp);
        }
        omp_init_lock(&CachedBlocksLock);
    }

public:
    //! 最大num_entry個のキャッシュ領域を空ける
    void DiscardCacheEntry2(const long& num_entry);

    //!  @brief CachedBlocksから要求された位置のデータブロックをロードして返す
    //!  @param BlockID   [in]  このブロックIDのデータをキャッシュから探す
    //!  @param DataBlock [out] BlockIDで指定されたブロックのデータ
    //!  @retval  0 要求されたデータブロックが正常にロードされた
    //!  @retval  1 要求されたデータブロックはリクエスト済だが未転送
    //!  @retval  2 要求されたデータブロックは未リクエスト
    //!  @retval -1 要求されたデータブロックはリクエストキューに無い(=このステップで転送する予定が無い)
    int Load(const long& BlockID, DataBlock** DataBlock);

    //!  指定されたエントリ数分のキャッシュデータをCachedBlocksから削除する
    void PurgeCachedBlocks(const int& NumEntry);

    //!  CachedBlocks, RequestedBlocks, RequestedQueuesを全て破棄
    void PurgeAllCacheLists(void);

    //! CachedBlocksのエントリを登録する
    long AddCachedBlocks(CommDataBlockManager* RecvData, const double& Time);

    //!  RequestQueuesにブロックIDを登録する
    void AddRequestQueues(const int& SubDomainID, const long& BlockID);

    //! RequestedBlocksにブロックIDを登録する
    //
    //このルーチンはCommunicator::CommRequest2()からしか呼ばれないので、非スレッドセーフな処理だがロックしていない
    void AddRequestedBlocks(const long BlockID)
    {
        RequestedBlocks.insert(BlockID);
    }

    //! RequestedBlocksからブロックIDを削除する
    //
    //このルーチンはLPT_CalcParticleData()のSingleスレッド区間からしか呼ばれないので、非スレッドセーフな処理だがロックしていない
    void DeleteRequestedBlocks(const long BlockID)
    {
        int num_removed = RequestedBlocks.erase(BlockID);
        if(num_removed != 1)
        {
            LPT::LPT_LOG::GetInstance()->ERROR("remove key from RequestedBlocks failed: ", BlockID);
        }
    }

    //! 転送を要求したブロックIDの数を返す
    long get_num_requested_block_id(void) const
    {
        return RequestedBlocks.size();
    }

    /*
    //! RequestQueuesに入っているブロックIDの個数を返す
    long GetTotalNumberOfRequestID(void)
    {
        long sum_requestID=0;
        for(std::vector<std::vector<long>*>::iterator it=RequestQueues.begin();  it!= RequestQueues.end(); ++it)
        {
            sum_requestID+=(*it)->size();
        }
        return sum_requestID;
    }
    */

private:
    omp_lock_t     CachedBlocksLock;                  //!< CachedBlocksの操作に関わるlock変数
    std::vector<std::vector<long>*>RequestQueues;     //!< データ転送を要求するブロックIDのリスト
    std::set<long> RequestedBlocks;                   //!< データ転送を要求したブロックIDのリスト
    std::CACHE_CONTAINER<Cache*> CachedBlocks;        //!< データブロックのキャッシュ
    int CacheSize;                                    //!< CachedBlocksに登録できるブロック数
};
} // namespace DSlib
#endif