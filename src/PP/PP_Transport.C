/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <mpi.h>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <omp.h>

#include "PPlib.h"
#include "Interpolator.h"
#include "PP_Integrator.h"
#include "ParticleData.h"
#include "DecompositionManager.h"
#include "DSlib.h"
#include "StartPointAll.h"
#include "LPT_LogOutput.h"
#include "PP_Transport.h"

namespace PPlib
{
void PP_Transport::UpdateParticle(ParticleData* Particle, const double& CurrentTime, const int& CurrentTimeStep, REAL_TYPE* Coord)
{
    Particle->CurrentTime     = CurrentTime;
    Particle->CurrentTimeStep = CurrentTimeStep;
    Particle->x               = Coord[0];
    Particle->y               = Coord[1];
    Particle->z               = Coord[2];
}

int PP_Transport::Calc(ParticleData* Particle, const double& deltaT, const int& divT, const double& CurrentTime, const int& CurrentTimeStep)
{
    //もし計算済の粒子だったらすぐにreturn
    if(CurrentTimeStep <= Particle->CurrentTimeStep)
    {
        return 5;
    }
    num_called++;

    DSlib::DecompositionManager* ptrDM    = DSlib::DecompositionManager::GetInstance();
    DSlib::DSlib*                ptrDSlib = DSlib::DSlib::GetInstance();
    REAL_TYPE                    x_i[3];
    REAL_TYPE                    x_new[3] = {Particle->x, Particle->y, Particle->z};
    REAL_TYPE                    v[3];

    if(LoadedDataBlock != NULL) LPT::LPT_LOG::GetInstance()->LOG("Old BlockID = ", LoadedDataBlock->BlockID);

    // deltaTの再分割
    double dt   = deltaT;
    int    numT = 1;
    if(divT > 1)
    {
        dt  /= divT;
        numT = divT;
    }

    LPT::LPT_LOG::GetInstance()->LOG("Coord before calc = ", x_new, 3);
    long old_BlockID_in_ParticleData = Particle->BlockID;
    long NewBlockID                  = -1;
    for(int t = 0; t < numT; t++)
    {
        NewBlockID = ptrDM->FindBlockIDByCoordLinear(x_new);
        if(LoadedDataBlock == NULL || LoadedDataBlock->BlockID != NewBlockID)
        {
            LPT::LPT_LOG::GetInstance()->LOG("New BlockID = ", NewBlockID);
            int retval = ptrDSlib->Load(NewBlockID, &LoadedDataBlock);
            if(retval == 1 || retval == 2)
            {
                //再度LPT_CalcParticleData()から呼び出されるので、今回は呼び出されなかったことにして終了
                num_called--;
                return 3;
            }else if(retval == 4){
                //現在の粒子座標をこのタイムステップでの更新後の座標として計算を終了
                UpdateParticle(Particle, CurrentTime, CurrentTimeStep, x_new);
                Particle->BlockID = NewBlockID;

                LPT::LPT_LOG::GetInstance()->WARN("Particle moved too far. Particle ID = ", Particle->GetAllID());
                return 4;
            }
        }

        // 粒子座標をデータブロック内の座標値に変換
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
        Interpolator::ConvXtoI(x_new, x_i, LoadedDataBlock->Origin, LoadedDataBlock->Pitch);

        // ルンゲ=クッタ積分
        int rkg = PP_Integrator::RKG(*LoadedDataBlock, dt, x_i);
        if(rkg != 0) LPT::LPT_LOG::GetInstance()->WARN("return value from PP_Integrator::RKG = ", rkg);

        // 粒子座標の逆変換
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
        Interpolator::ConvItoX(x_i, x_new, LoadedDataBlock->Origin, LoadedDataBlock->Pitch);

        LPT::LPT_LOG::GetInstance()->LOG("Coord after calc = ", x_new, 3);
        if(ptrDM->CheckBounds(x_new) != 0) return 1;
    } // end of for(t)

    //粒子オブジェクトの座標、時刻、タイムステップを更新
    UpdateParticle(Particle, CurrentTime, CurrentTimeStep, x_new);

    //移動後の位置でのブロックIDと粒子速度を代入
    NewBlockID        = ptrDM->FindBlockIDByCoordLinear(x_new);
    Particle->BlockID = NewBlockID;

    if(LoadedDataBlock == NULL || LoadedDataBlock->BlockID != NewBlockID)
    {
        int retval = ptrDSlib->Load(NewBlockID, &LoadedDataBlock);
        if(retval == 0)
        {
            //粒子がルンゲ=クッタの最後のステップで別のデータブロックに移動し
            //なおかつ、移動先のデータブロックがキャッシュ内にある場合のみ速度を再計算
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
            Interpolator::InterpolateData(*LoadedDataBlock, x_i, v);
        }else{
            counter++;
            LPT::LPT_LOG::GetInstance()->LOG("Requested Block was not found. using old ParticleVelocity for this time step");
            LPT::LPT_LOG::GetInstance()->LOG("Current Time = ", Particle->CurrentTime);
            LPT::LPT_LOG::GetInstance()->LOG("ParticleID = ", Particle->GetAllID());
        }
    }

    //粒子位置での速度を計算
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
    Interpolator::InterpolateData(*LoadedDataBlock, x_i, v);

    Particle->Vx = v[0];
    Particle->Vy = v[1];
    Particle->Vz = v[2];

    return old_BlockID_in_ParticleData == Particle->BlockID ? 0 : 2;
}
} // namespace PPlib
