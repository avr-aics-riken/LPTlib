/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_PARTICLE_DATA_H
#define PPLIB_PARTICLE_DATA_H

///@attention PARTICLE_DATA_FORMAT_VERSIONは、double型の変数
//            したがって、1.0.0のような形式のバージョン番号は使用できない
#define PARTICLE_DATA_FORMAT_VERSION 2.0

#include <iostream>
#include <sstream>
#include <string>

namespace PPlib
{
//! @brief 粒子データを保持するための構造体
//!
//! インスタンスを生成した後はPPlib::Particlesにポインタを登録し、そちらで管理する。
//! インスタンスを破棄する時は、そちらのエントリも必ず削除すること
class ParticleData
{
public:
    int StartPointID1;          //!< この粒子を放出した開始点のRank番号
    int StartPointID2;          //!< この粒子を放出した開始点のID番号（同一プロセス内でのみ一意）
    int ParticleID;             //!< 開始点から放出された順番を示す番号 (同一開始点内で一意)
    REAL_TYPE x;                //!< 粒子のx座標
    REAL_TYPE y;                //!< 粒子のy座標
    REAL_TYPE z;                //!< 粒子のz座標
    REAL_TYPE Vx;               //!< 粒子速度のx成分
    REAL_TYPE Vy;               //!< 粒子速度のy成分
    REAL_TYPE Vz;               //!< 粒子速度のz成分
    double    StartTime;        //!< 粒子が放出された時刻
    double    LifeTime;         //!< 粒子の寿命(放出された時刻からLiftime時間経過後に消滅する
    long      BlockID;          //!< 粒子が存在する位置のブロックID
    double    CurrentTime;      //!< どの時点の粒子データかを格納する
    int       CurrentTimeStep;  //!< どの時点の粒子データかを格納する(タイムステップ)

    //! Getter
    //!  publicなメンバなので本来不要だが、LifeTime classへのインターフェースとして用意している
    double GetStartTime()
    {
        return this->StartTime;
    }

    double GetLifeTime()
    {
        return this->LifeTime;
    }

    double GetID()
    {
        return this->ParticleID;
    }

    //! @brief 全プロセスを通して一意な粒子IDを文字列として返す
    std::string GetAllID(void)
    {
        std::ostringstream oss;
        oss<<this->StartPointID1<<","<<this->StartPointID2<<","<<GetID();
        return oss.str();
    }

    //! 出力演算子オーバーロード
    friend std::ostream& operator<<(std::ostream& stream, ParticleData& obj);

    //! 比較演算子のオーバーロード
    bool operator==(const ParticleData& obj)
    {
        return (this->ParticleID == obj.ParticleID
                && this->StartPointID1 == obj.StartPointID1
                && this->StartPointID2 == obj.StartPointID2
                && this->x == obj.x
                && this->y == obj.y
                && this->z == obj.z
                && this->StartTime == obj.StartTime) ? true : false;
    }
};

//! 粒子をBlockID順にソートするためのファンクタ
struct CompareBlockID
{
    bool operator()(const ParticleData* left, const ParticleData* right) const
    {
        return (left->BlockID < right->BlockID) ? true : false;
    }
};

//! 粒子をタイムステップ順にソートするためのファンクタ
struct CompareTimeStep
{
    bool operator()(const ParticleData* left, const ParticleData* right) const
    {
        return (left->CurrentTimeStep < right->CurrentTimeStep) ? true : false;
    }
};

//! 粒子をID順にソートするためのファンクタ
struct CompareID
{
    bool operator()(const ParticleData* left, const ParticleData* right) const
    {
        bool retval;
        if(left->StartPointID1 == right->StartPointID1)
        {
            if(left->StartPointID2 == right->StartPointID2)
            {
                retval = left->ParticleID < right->ParticleID ? true : false;
            }else{
                retval = left->StartPointID2 < right->StartPointID2 ? true : false;
            }
        }else{
            retval = left->StartPointID1 < right->StartPointID2 ? true : false;
        }
        return retval;
    }
};
} // namespace PPlib
#endif