#ifndef DSLIB_COMMUNICATOR_H
#define DSLIB_COMMUNICATOR_H

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <mpi.h>

#include "CommDataBlock.h"

namespace DSlib
{
//forward declaration
  struct Cache;
  class DecompositionManager;
  class DSlib;

  //! 粒子-流体間の通信を行なうクラス
  class Communicator
  {
  private:
    //!  粒子-流体間の通信に使われるコミュニケータ
    MPI_Comm Comm;

    //! Comm内の総プロセス数
    int NumProcs;

    //! 自RankのRank番号
    int MyRank;

    //!  DataBlockのヘッダデータ送受信用データタイプ
    MPI_Datatype MPI_DataBlockHeader;

    //!  Rank i のプロセスへ要求するデータブロック数をSendRequestCounts[i]に保持する
    int *SendRequestCounts;

    //!  Rank i のプロセスから受けとるデータブロックのリクエスト数をRecvRequestCounts[i]に保持する
    int *RecvRequestCounts;

    //!  自Rankから各Rankへ転送するデータブロックのIDを保持する配列
    std::vector < std::vector < long >*>RequestedBlockIDs;

    Communicator(const Communicator& obj);
    Communicator& operator=(const Communicator& obj);

  public:
    // Constructor
    Communicator():Comm(MPI_COMM_WORLD)
    {
      MPI_Comm_size(Comm, &NumProcs);
      MPI_Comm_rank(Comm, &MyRank);

      SendRequestCounts = new int[NumProcs];
      RecvRequestCounts = new int[NumProcs];

        MPI_Type_contiguous(sizeof(CommDataBlockHeader), MPI_BYTE, &MPI_DataBlockHeader);
      if(MPI_SUCCESS != MPI_Type_commit(&MPI_DataBlockHeader))
      {
        throw "MPI_Type_commit failed !";
      }

      for(int i = 0; i < NumProcs; i++)
      {
        std::vector < long >*tmp = new std::vector < long >;
          RequestedBlockIDs.push_back(tmp);
          SendRequestCounts[i] = 0;
          RecvRequestCounts[i] = 0;
      }
    }

    //Destructor
     ~Communicator()
    {
      delete[]SendRequestCounts;
      delete[]RecvRequestCounts;
      for(std::vector < std::vector < long >*>::iterator it = RequestedBlockIDs.begin(); it != RequestedBlockIDs.end(); ++it) {
        delete *it;
      }
    }

    /**
     * 各RankがSendRequestCountsに保持する自Rankから他Rankへのデータ転送数のリストを
     * Alltoall転送によって他Rankから自Rankへのデータ転送数リストに変換し、RecvConutsに格納する
     * 
     * ex.
     * -----
     * Rank0
     *   SendRequestCounts={0,5,6,2}
     * Rank1
     *   SendRequestCounts={3,0,8,1}
     * Rank2
     *   SendRequestCounts={4,2,0,5}
     * Rank3
     *   SendRequestCounts={7,9,6,0}
     * -----
     *       ↓Alltoall
     * -----
     * Rank0
     *   RecvRequestCounts={0,3,4,7}
     * Rank1
     *   RecvRequestCounts={5,0,2,9}
     * Rank2
     *   RecvRequestCounts={6,8,0,6}
     * Rank3
     *   RecvRequestCounts={2,1,5,0}
     * -----
     */
    void CommRequestsAlltoall();

    //! P2P転送でのリクエストを行ない、メンバ変数RequestedBlockIDsに自Rankから送るブロックIDのリストを保持する
    void CommRequest(DSlib * ptrDSlib);

    //!  粒子計算プロセスから流体計算プロセスへ、P2Pのデータ転送を行なう
    void CommDataP2F();

    //! @brief 流体計算プロセスから粒子計算プロセスへ、P2Pのデータ転送を行なう
    //! @param Data [in] 送信するデータを格納した領域へのポインタ
    //! @param vlen [in] Dataのベクトル長(1=スカラーデータ, 3=ベクトルデータ)
    void CommDataF2P(REAL_TYPE * Data, REAL_TYPE * v00, int *Mask, const int &vlen, DecompositionManager * ptrDM, std::list < CommDataBlockManager * >*SendBuff, std::list < CommDataBlockManager * >*RecvBuff);

    //! 引数で指定されたRankのRequestQueuesの数をDSlibから取得し、SendRequestCountsに格納する
    void MakeSendRequestCounts(const int &SubDomainID, DSlib * ptrDSlib);

    //! @brief *Dataが示す領域に保持されているデータから、BlockIDに相当するブロックのデータを取り出して、SendBuffにパッキングする
    //! @param BlockID  [in]  必要な領域のブロックID
    //! @param Data     [in]  流体ソルバーからもらってきた物理量を格納しているデータ領域へのポインタ
    //! @param Mask     [in]  流体ソルバーからもらってきた物理量のマスク(物理量が存在しないセルは0他は1)
    //! @param vlen     [in]  Dataの領域に格納されている物理量のベクトル長
    //! @param SendBuff [out] 送信バッファ
    //! @param SendSize [out] 送信サイズ
    void CommPacking(const long &BlockID, REAL_TYPE * Data, int *Mask, const int &vlen, REAL_TYPE * SendBuff, CommDataBlockHeader * Header, int *SendSize, const int &MyRank);

    void PrintVectorSize(void)
    {
      int size=0;
      int i=0;
      for(std::vector < std::vector < long >*>::iterator it = RequestedBlockIDs.begin(); it != RequestedBlockIDs.end(); ++it)
      {
        size+=(*it)->capacity();
        std::cerr << "RequestedBlockIDs["<<i<<"].capacity = "<<(*it)->capacity()<<" ";
        std::cerr << "RequestedBlockIDs["<<i<<"].size     = "<<(*it)->size()<<std::endl;
        i++;
      }
      std::cerr << "RequestedBlockIDs.capacity = "<<RequestedBlockIDs.capacity()<<" ";
      std::cerr << "RequestedBlockIDs.size     = "<<RequestedBlockIDs.size()<<" ";
      std::cerr << "Allocated vector size in Communicator = "<<size*sizeof(long) + RequestedBlockIDs.capacity()*sizeof(size_t)<<std::endl;
    }
    void PurgeRequestedBlockIDs(void)
    {
      for (std::vector<std::vector<long> * >::iterator it=RequestedBlockIDs.begin();it!=RequestedBlockIDs.end();++it)
      {
        std::vector<long>().swap(**it);
      }
    }

    //! Accessor
    int GetMyRank()
    {
      return this->MyRank;
    };
    int GetNumProcs()
    {
      return this->NumProcs;
    };
    int GetSumSendRequestCounts()
    {
      int sum = 0;

      for(int i = 0; i < NumProcs; i++)
        sum += SendRequestCounts[i];
      return sum;
    };

    int GetSumRecvRequestCounts()
    {
      int sum = 0;

      for(int i = 0; i < NumProcs; i++)
        sum += RecvRequestCounts[i];
      return sum;
    };

    //単体テスト用Getter/Setter
#ifndef DEBUG
  private:
    void SetSendRequestCounts(const int &SubDomainID, const int &Count)
    {
      SendRequestCounts[SubDomainID] = Count;
    };
    void SetRecvRequestCounts(const int &SubDomainID, const int &Count)
    {
      RecvRequestCounts[SubDomainID] = Count;
    };
    int GetSendRequestCounts(const int &SubDomainID)
    {
      return this->SendRequestCounts[SubDomainID];
    };
    int GetRecvRequestCounts(const int &SubDomainID)
    {
      return this->RecvRequestCounts[SubDomainID];
    };
    void DumpRequestCounts(void)
    {
      for(int i = 0; i < NumProcs; i++)
        std::cerr << "Send,Recv = " << SendRequestCounts[i] << ", " << RecvRequestCounts[i] << std::endl;
    };
    void SetRequestedBlockIDs(const int &SubDomainID, const int &BlockID)
    {
      RequestedBlockIDs.at(SubDomainID)->push_back(BlockID);
    };
#endif
  };

} // namespace DSlib
#endif
