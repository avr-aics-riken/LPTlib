/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef DSLIB_DATA_BLOCK_H
#define DSLIB_DATA_BLOCK_H
#include <iostream>

namespace DSlib
{
//! @brief 1データブロック分の流速場情報を保持する構造体
struct DataBlock
{
public:
    long       BlockID;        //!< データブロックのID
    int        SubDomainID;    //!< そのデータブロックが存在するサブドメインのID
    REAL_TYPE  Origin[3];      //!< データブロックの原点座標
    int        OriginCell[3];  //!< データブロックの原点位置を含むセルのindex
    int        BlockSize[3];   //!<  このデータブロックのサイズ(単位はセル数)
    double     Time;           //!<  このデータブロックが保持する流速場の情報が、どの時刻のものなのかを保持する
    REAL_TYPE  Pitch[3];       //!<  セル幅
    //TODO ここまでを内部クラスにまとめる
    REAL_TYPE* Data;           //!<  流速データの配列へのポインタ

    //! コンストラクタ
    DataBlock() : Data(NULL), BlockID(-1), SubDomainID(-1), Time(-1.0)
    {
        OriginCell[0] = -1;
        OriginCell[1] = -1;
        OriginCell[2] = -1;
        BlockSize[0]  = -1;
        BlockSize[1]  = -1;
        BlockSize[2]  = -1;
        //TODO OriginとpitchはNanで初期化したい
    }
    //! デストラクタ
    ~DataBlock()
    {
        delete[] Data;
    }

    //!コピーコンストラクタ
    DataBlock(const DataBlock& arg)
    {
        BlockID     = arg.BlockID;
        SubDomainID = arg.SubDomainID;
        for(int i = 0; i < 3; i++)
        {
            Origin[i]     = arg.Origin[i];
            OriginCell[i] = arg.OriginCell[i];
            BlockSize[i]  = arg.BlockSize[i];
            Pitch[i]      = arg.Pitch[i];
        }
        Data = arg.Data;
        Time = arg.Time;
    }

    //! 代入オペレータ
    DataBlock& operator=(const DataBlock& arg)
    {
        BlockID     = arg.BlockID;
        SubDomainID = arg.SubDomainID;
        for(int i = 0; i < 3; i++)
        {
            Origin[i]     = arg.Origin[i];
            OriginCell[i] = arg.OriginCell[i];
            BlockSize[i]  = arg.BlockSize[i];
            Pitch[i]      = arg.Pitch[i];
        }
        Data = arg.Data;
        Time = arg.Time;
        return *this;
    }


    //!  挿入子オーバーロード
    friend std::ostream& operator<<(std::ostream& stream, DataBlock obj);
};
} // namespace DSlib
#endif
