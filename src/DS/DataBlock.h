#ifndef DSLIB_DATA_BLOCK_H
#define DSLIB_DATA_BLOCK_H
#include <iostream>

namespace DSlib
{
  //! @brief 1データブロック分の流速場情報を保持する構造体
  struct DataBlock
  {
  public:
    //! @brief データブロックのid
    //! 解析領域全体を通じて一意の番号
    //! ただし、TimeStepが異なれば同じBlockIDを持つインスタンスが存在する可能性もあることに注意
    long BlockID;

    //! @brief そのデータブロックが存在するサブドメインのID
    //! サブドメインID=その領域の計算を担当する流体計算プロセスのRank番号
    int SubDomainID;

    //! データブロックの原点座標
    REAL_TYPE Origin[3];

    //! データブロックの原点位置を含むセルのindex
    int OriginCell[3];

    //!  このデータブロックのサイズ(単位はセル数)
    int BlockSize[3];

    //!  流速データの配列へのポインタ
    REAL_TYPE *Data;

    //!  このデータブロックが保持する流速場の情報が、どの時刻のものなのかを保持する
    double Time;

    //!  セル幅
    REAL_TYPE Pitch[3];

    //! 代入オペレータ
    void operator=(const DataBlock & org)
    {
      BlockID = org.BlockID;
      SubDomainID = org.SubDomainID;
      for(int i = 0; i < 3; i++)
      {
        Origin[i] = org.Origin[i];
        OriginCell[i] = org.OriginCell[i];
        BlockSize[i] = org.BlockSize[i];
        Pitch[i] = org.Pitch[i];
      }
      Data = org.Data;
        Time = org.Time;
    }

    //!コンストラクタ
    DataBlock():Data(NULL), BlockID(-1), SubDomainID(-1)
    {
    }

    //!  挿入子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, DataBlock obj);
  };

} // namespace DSlib
#endif
