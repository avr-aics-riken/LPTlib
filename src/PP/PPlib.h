/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_P_PLIB_H
#define PPLIB_P_PLIB_H

#include <iostream>

#include <vector>
#include <list>
#include <map>
#include "ParticleData.h"
#include "StartPointAll.h"
#include "ParticleContainer.h"
//forward declaration
namespace DSlib
{
class DSlib;
class DecompositionManager;
}
namespace PPlib
{
//! @brief 粒子データの保持、管理とマイグレーション処理を行なう
class PPlib
{
private:
    //Singletonパターンを適用
    PPlib(){}
    PPlib(const PPlib& obj);
    PPlib& operator=(const PPlib& obj);
    ~PPlib()
    {
        for(std::vector<StartPoint*>::iterator it = StartPoints.begin(); it != StartPoints.end(); ++it)
        {
            delete *it;
        }
    }

public:
    static PPlib* GetInstance()
    {
        static PPlib instance;
        return &instance;
    }

public:
    std::vector<StartPoint*> StartPoints; //!< 担当する開始点データへのポインタのvector
    ParticleContainer        Particles;   //!< 計算を担当する粒子データオブジェクトへのポインタを格納する。



    //! @brief StartPointsに登録されている全ての開始点から粒子を放出させる
    //! 開始点がMovingPoints型だった場合は現在時刻に応じた位置へ移動させてから粒子を放出する
    //! @param CurrentTime [in] 現在時刻
    void EmitNewParticles(const double& CurrentTime, const int& CurrentTimeStep);

    //! @brief Particlesに登録されている粒子が存在する位置のデータブロックIDをDSlib::RequestQueuesに登録する
    void MakeRequestQueues(DSlib::DSlib* ptrDSlib);

    //! @brief StartPointsに登録されている個々の開始点データのリリースタイムをチェックし、現在時刻がリリースタイムを越えていたらその開始点のインスタンスを破棄する
    //! @param CurrentTime [in] 現在時刻
    void DestroyExpiredStartPoints(const double& CurrentTime);

    //! @brief Particlesに登録されている個々の粒子データの寿命をチェックし、現在時刻が寿命を越えていたらその粒子のインスタンスを破棄する
    //! @param CurrentTime [in] 現在時刻
    void DestroyExpiredParticles(const double& CurrentTime);

    //! @brief 与えられたオブジェクト(粒子または開始点を想定)の寿命と現在時刻を比較
    //! @param CurrentTime [in] 現在時刻
    //! @retval true  このオブジェクトは破棄の対象
    //! @retval false このオブジェクトは破棄の対象外
    template<typename T>
    bool isExpired(const double& CurrentTime, T* obj);

    //! @brief  粒子のマイグレーションが必要かどうか判定する
    void DetermineMigration();

    //! @brief 粒子のマイグレーションを行なう
    void MigrateParticle();

    //!  引数で指定されたプロセス数を目標に、開始点のデータ分散を行なう
    void DistributeStartPoints(const int& NParticleProcs);

    //!開始点の格子座標をTimeStep0、時刻0.0 の粒子として出力する
    void OutputStartPoints(const REAL_TYPE& RefLength);

    //! ファイルから開始点情報を読み込む
    void ReadStartPoints(const std::string& filename, const REAL_TYPE& RefLength, const double& RefTime);

    //! ファイルに開始点情報を出力する
    void WriteStartPoints(const std::string& filename, const REAL_TYPE& RefLength, const double& RefTime);
};
} // namespace PPlib
#endif
