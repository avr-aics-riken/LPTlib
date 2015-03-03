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
    long BlockID;           //!< データブロックのID
    int SubDomainID;        //!< そのデータブロックが存在するサブドメインのID
    REAL_TYPE Origin[3];    //!< データブロックの原点座標
    int OriginCell[3];      //!< データブロックの原点位置を含むセルのindex
    int BlockSize[3];       //!<  このデータブロックのサイズ(単位はセル数)
    REAL_TYPE* Data;        //!<  流速データの配列へのポインタ
    double Time;            //!<  このデータブロックが保持する流速場の情報が、どの時刻のものなのかを保持する
    REAL_TYPE Pitch[3];     //!<  セル幅

    //! 代入オペレータ
    DataBlock& operator=(const DataBlock& org)
    {
        BlockID     = org.BlockID;
        SubDomainID = org.SubDomainID;
        for(int i = 0; i < 3; i++)
        {
            Origin[i]     = org.Origin[i];
            OriginCell[i] = org.OriginCell[i];
            BlockSize[i]  = org.BlockSize[i];
            Pitch[i]      = org.Pitch[i];
        }
        Data = org.Data;
        Time = org.Time;
        return *this;
    }

    //!コンストラクタ
    DataBlock() : Data(NULL), BlockID(-1), SubDomainID(-1), Time(-1.0)
    {
        OriginCell[0] = -1;
        OriginCell[1] = -1;
        OriginCell[2] = -1;
        BlockSize[0]  = -1;
        BlockSize[1]  = -1;
        BlockSize[2]  = -1;
    }

    ~DataBlock()
    {
        delete[] Data;
    }

    //!  挿入子オーバーロード
    friend std::ostream& operator<<(std::ostream& stream, DataBlock obj);
};
} // namespace DSlib
#endif