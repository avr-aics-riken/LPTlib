/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef DSLIB_COMM_DATA_BLOCK_H
#define DSLIB_COMM_DATA_BLOCK_H

#include <mpi.h>
#include "LPT_LogOutput.h"
#include "PMlibWrapper.h"

namespace DSlib
{
//! DataBlockのDataとTime以外のメンバをまとめた構造体
struct CommDataBlockHeader
{
    long BlockID;
    int SubDomainID;
    REAL_TYPE Origin[3];
    int OriginCell[3];
    int BlockSize[3];
    REAL_TYPE Pitch[3];
};

//! @brief データブロックのヘッダ部をまとめた構造体とデータ領域ヘのポインタ、それぞれの転送用MPI_Request変数をまとめて保持するクラス
//!
//! サイズ指定のコンストラクタを呼ぶと、データ領域用のメモリ(Buff)を指定されたサイズで確保する
//! 受信側はBuffは、DSlib::AddCache()内でキャッシュにポインタを移動されBuffにはNULLが代入されるので
//! デストラクタ内のdeleteでは解放されない。
//! 逆に送信側ではこのdleteで全て解放される
class CommDataBlockManager
{
private:
    CommDataBlockManager();
    CommDataBlockManager(const CommDataBlockManager& obj);
    CommDataBlockManager& operator=(const CommDataBlockManager& obj);

public:
    CommDataBlockManager(int size)
    {
        Buff   = new REAL_TYPE[size];
        Header = new CommDataBlockHeader;
    }

    ~CommDataBlockManager()
    {
        delete Header;
        delete[] Buff;
        Buff = NULL;
    }

    bool Wait()
    {
        LPT::PMlibWrapper& PM = LPT::PMlibWrapper::GetInstance();
        PM.start("MPI_Wait");
        MPI_Status         status;
        if(MPI_SUCCESS != MPI_Wait(&Request0, &status))
        {
            LPT::LPT_LOG::GetInstance()->ERROR("MPI_Wait 1 Failed");
        }
        if(MPI_SUCCESS != MPI_Wait(&Request1, &status))
        {
            LPT::LPT_LOG::GetInstance()->ERROR("MPI_Wait 2 Failed");
        }
        PM.stop("MPI_Wait");
        return true;
    }

    bool Test()
    {
        LPT::PMlibWrapper& PM = LPT::PMlibWrapper::GetInstance();
        PM.start("MPI_Wait");
        MPI_Status         status;
        int                flag0 = 0;
        int                flag1 = 0;
        if(MPI_SUCCESS != MPI_Test(&Request0, &flag0, &status))
        {
            LPT::LPT_LOG::GetInstance()->ERROR("MPI_Test for Data Failed");
        }
        if(MPI_SUCCESS != MPI_Test(&Request1, &flag1, &status))
        {
            LPT::LPT_LOG::GetInstance()->ERROR("MPI_Test for Header Failed");
        }

        PM.stop("MPI_Wait");

        // MPI_Test後に flag == 0の時、転送は未完了
        return flag0 != 0 && flag1 != 0;
    }

    //! @brief データ送受信に使う領域へのポインタ
    //! ポインタの先の領域には、最大サイズのデータブロック(BlockID=0)の
    //! データが収まるサイズで領域を確保しておくこと
    REAL_TYPE*           Buff;

    //!  DataBlockのヘッダ部分送信用構造体
    CommDataBlockHeader* Header;

    //!  データ送受信につかうMPI_Request
    MPI_Request          Request0;

    //!  データ送受信につかうMPI_Request(ヘッダ部用)
    MPI_Request          Request1;
};
} // namespace DSlib
#endif
