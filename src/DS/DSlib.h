#ifndef DSLIB_D_SLIB_H
#define DSLIB_D_SLIB_H

#include <iostream>
#include <vector>
#include <set>
#include <deque>
#include <list>
#include <mpi.h>

#include "DataBlock.h"
#include "Cache.h"
#include "CommDataBlock.h"
#include "Communicator.h"

namespace DSlib
{
  //!  @brief データブロックの管理を行なうクラス
  //!
  //! 転送待ちブロック、転送中ブロックおよび転送済ブロックの3つに分けてデータブロックの状態を管理する。
  //！
  class DSlib
  {
    friend void Communicator::CommRequest(DSlib * ptrDSlib);

    //! データ転送を要求するブロックIDのリスト
      std::vector < std::vector < long >*>RequestQueues;

    //! データ転送を要求したブロックIDのリスト
    //!
    //! RequestQueueにあったブロックIDのうち、CommRequestsで転送要求を送る対象として数えられたものをこちらに移す
      std::set < long >RequestedBlocks;

    //! データブロックのキャッシュ
      std::CACHE_CONTAINER < Cache * >CachedBlocks;

    //! @brief CachedBlocksに登録できるブロック数を決めるパラメータ
    //!  サイズ指定はMB単位なので、実際のエントリ数は
    //!  CacheSize * 1024 * 1024 / sizeof(DataBlock)
    int CacheSize;

    DSlib(const DSlib& obj);
    DSlib& operator=(const DSlib& obj);

  public:
    int CalcNumComm(Communicator * ptrComm);
    void DiscardCacheEntry(const int & j,Communicator * ptrComm );
    //! CacheSizeを越えるデータブロックが必要となった時に、1回にキャッシュから追い出すサイズを決める
    int CommBufferSize;

    //!  @brief CachedBlocksから要求された位置のデータブロックをロードして返す
    //!  @param BlockID   [in]  このブロックIDのデータをキャッシュから探す
    //!  @param DataBlock [out] BlockIDで指定されたブロックのデータ
    //!  @retval  0 要求されたデータブロックが正常にロードされた
    //!  @retval  1 要求されたデータブロックはリクエスト済だが未転送
    //!  @retval  2 要求されたデータブロックは未リクエスト
    //!  @retval -1 要求されたデータブロックはリクエストキューに無い(=このステップで転送する予定が無い)
    int Load(const long &BlockID, DataBlock ** DataBlock);

    //!  指定されたエントリ数分のキャッシュデータをCachedBlocksから削除する
    void PurgeCachedBlocks(const int &NumEntry);

    //!  CachedBlocks, RequestedBlocks, RequestedQueuesを全て破棄
    void PurgeAllCacheLists(void);

    //! CachedBlocksのエントリを登録する 
    void AddCachedBlocks(CommDataBlockManager * RecvData, const double &Time);

    //!  RequestQueuesにブロックIDを登録する
    void AddRequestQueues(const int &SubDomainID, const long &BlockID);

    //!  引数で指定されたRankに要求するブロックIDの数を取得する
    int CountRequestQueues(const int &SubDomainID)
    {
      return this->RequestQueues.at(SubDomainID)->size();
    }

    //!  RequestQueuesから重複したIDを取り除く
    void DedupulicateRequestQueues();

    //! 引数で指定されたRankに要求するブロックIDをRequestQueuesからRequestedBlocksへ移動させる 
    void MoveRequestQueues(const int &SubDomainID);

    //!  RequestedBlocksから指定されたIDを削除する
    void RemoveRequestedBlocks(const long &BlockID);

    //! @brief Constructor
    //! @param CacheSize [in] データブロックのキャッシュエントリ数
    DSlib(int argCacheSize, int argCommBufferSize):CacheSize(argCacheSize), CommBufferSize(argCommBufferSize)
    {
      int NumProcs;
      MPI_Comm_size(MPI_COMM_WORLD, &NumProcs);
      for(int i = 0; i < NumProcs; i++)
      {
        std::vector < long >*tmp = new std::vector < long >;
          RequestQueues.push_back(tmp);
      }
    }
    ~DSlib()
    {
      int NumProcs;
      MPI_Comm_size(MPI_COMM_WORLD, &NumProcs);
      for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it!=RequestQueues.end(); ++it)
      {
        delete *it;
      }
    }
    void PrintVectorSize(void)
    {
      int size=0;
      int i=0;
      for(std::vector < std::vector < long >*>::iterator it = RequestQueues.begin(); it!=RequestQueues.end(); ++it)
      {
        size+=(*it)->capacity();
        std::cerr << "RequestQueues["<<i<<"].capacity = "<<(*it)->capacity() << " ";
        std::cerr << "RequestQueues["<<i<<"].size     = "<<(*it)->size() << std::endl;
        i++;
      }
      std::cerr << "RequestQueues.capacity = "<<RequestQueues.capacity()<< " ";
      std::cerr << "RequestQueues.size     = "<<RequestQueues.size()<< " ";
      std::cerr << "Allocated vector size in DSlib = "<<size*sizeof(long) + RequestQueues.capacity()*sizeof(size_t)<<std::endl;
    }
  };

} // namespace DSlib
#endif
