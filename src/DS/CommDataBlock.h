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

  //! @brief データブロックのヘッダ部をまとめた構造体とデータ領域ヘのポインタ、それぞれの転送用MPI_Request変数をまとめて保持するクラス
  //! サイズ指定のコンストラクタを呼ぶと、データ領域用のメモリを指定されたサイズで確保する
  //! DSlib::AddCache内でポインタを移動させて、NULLを入れている
  //! この領域はデストラクタ内で解放しているが、キャッシュに移したデータは解放されないので注意
  class CommDataBlockManager
  {
    CommDataBlockManager();
    CommDataBlockManager(const CommDataBlockManager& obj);
    CommDataBlockManager& operator=(const CommDataBlockManager& obj);

    public:
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

    CommDataBlockManager(int size)
    {
      Buff   = new REAL_TYPE [size];
      Header = new CommDataBlockHeader;
    }
    ~CommDataBlockManager()
    {
      delete Header;
      delete [] Buff;
    }
  };

} // namespace DSlib
#endif
