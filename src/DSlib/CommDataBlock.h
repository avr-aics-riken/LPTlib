#ifndef DSLIB_COMM_DATA_BLOCK_H
#define DSLIB_COMM_DATA_BLOCK_H

#include <mpi.h>

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

  //! データ転送に使うバッファ領域へのポインタと、対応するMPI_Requestをまとめた構造体
  struct CommDataBlockManager
  {
    //! @brief データ送受信に使う領域へのポインタ
    //! ポインタの先の領域には、最大サイズのデータブロック(BlockID=0)の
    //! データが収まるサイズで領域を確保しておくこと
    REAL_TYPE *Buff;

    //!  DataBlockのヘッダ部分送信用構造体
    CommDataBlockHeader *Header;

    //!  データ送受信につかうMPI_Request
    MPI_Request Request0;

    //!  データ送受信につかうMPI_Request(ヘッダ部用)
    MPI_Request Request1;
  };

} // namespace DSlib
#endif
