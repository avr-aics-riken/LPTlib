#include <mpi.h>
#include <algorithm>
#include <iomanip>
#include <cmath>

#include "PPlib.h"
#include "Interpolator.h"
#include "PP_Integrator.h"
#include "ParticleData.h"
#include "DecompositionManager.h"
#include "DSlib.h"
#include "StartPointAll.h"
#include "LPT_LogOutput.h"
#include "LPT_ParticleOutput.h"
#include "PP_Transport.h"

namespace PPlib
{
  int PP_Transport::Calc(ParticleData * Particle, const double &deltaT, const int &divT, REAL_TYPE * v00, DSlib::DSlib * ptrDSlib, const double &CurrentTime, const unsigned int &CurrentTimeStep)
  {
    DSlib::DecompositionManager * ptrDM = DSlib::DecompositionManager::GetInstance();
    REAL_TYPE x_i[3];
    REAL_TYPE x_old[3];
    REAL_TYPE x_new[3];

      LPT::LPT_LOG::GetInstance()->LOG("Old BlockID = ", OldBlockID);

    // deltaTの再分割
    double dt = deltaT;
    int numT = 1;
    if(divT > 1)
    {
      dt /= divT;
      numT = divT;
    }
    //計算用の一時配列に粒子座標をコピー
    x_new[0] = Particle->Coord[0];
    x_new[1] = Particle->Coord[1];
    x_new[2] = Particle->Coord[2];

    for(int t = 0; t < numT; t++) {
      x_old[0] = x_new[0];
      x_old[1] = x_new[1];
      x_old[2] = x_new[2];
      long NewBlockID = ptrDM->FindBlockIDByCoordLinear(x_new);

      if(Particle->BlockID != NewBlockID) {
        Particle->BlockID = NewBlockID;
      }

      if(OldBlockID != NewBlockID) {
        LPT::LPT_LOG::GetInstance()->LOG("Coord = ", x_new, 3);
        LPT::LPT_LOG::GetInstance()->LOG("New BlockID = ", NewBlockID);
        int retval = ptrDSlib->Load(NewBlockID, &LoadedDataBlock);

        if((retval == 1) || (retval == 2)) {
          LPT::LPT_LOG::GetInstance()->WARN("exit Calc.PP_Transport() due to DSlib::Load() failed. return value = ", retval);
          return retval;
        } else if(retval == -1) {
          //現在の粒子座標をこのタイムステップでの更新後の座標として計算を終了
          Particle->Coord[0] = x_new[0];
          Particle->Coord[1] = x_new[1];
          Particle->Coord[2] = x_new[2];

          Particle->CurrentTime = CurrentTime;
          Particle->CurrentTimeStep = CurrentTimeStep;

          LPT::LPT_LOG::GetInstance()->WARN("Particle was moved 2 block");
          return retval;
        }
        if(!gus->setup(LoadedDataBlock)) {
          LPT::LPT_LOG::GetInstance()->ERROR("Interporator::setup() failed");
          return -10;
        }
        OldBlockID = NewBlockID;
      }
      // 粒子座標をデータブロック内の座標値に変換
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
      gus->ConvXtoI(x_old, x_i);

      //流速をParticleDataに代入
      REAL_TYPE v[3];

#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
      gus->InterpolateData(x_i, v);

      // ルンゲ=クッタ積分
      int rkg = PP_Integrator::RKG(*gus, dt, x_i);

      if(rkg != 0)
        LPT::LPT_LOG::GetInstance()->WARN("return value from PP_Integrator::RKG = ", rkg);

      // 粒子座標の逆変換
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
      gus->ConvItoX(x_i, x_new);
      LPT::LPT_LOG::GetInstance()->LOG("Coord = ", x_new, 3);
      int chkBounds = ptrDM->CheckBounds(x_new);

      if(chkBounds != 0) {
        LPT::LPT_LOG::GetInstance()->WARN("return value from CheckBounds = ", chkBounds);
        return -10;
      }
    } // end of for(t)

    //粒子座標を新しい位置に変更し、時刻、タイムステップを進める
    Particle->Coord[0] = x_new[0];
    Particle->Coord[1] = x_new[1];
    Particle->Coord[2] = x_new[2];

    Particle->CurrentTime = CurrentTime;
    Particle->CurrentTimeStep = CurrentTimeStep;

    //移動後の位置でのブロックIDと粒子速度を代入
    long NewBlockID = ptrDM->FindBlockIDByCoordLinear(Particle->Coord);


    if(OldBlockID != NewBlockID) {
      int retval = ptrDSlib->Load(NewBlockID, &LoadedDataBlock);


      if(retval == 0) {
        if(!gus->setup(LoadedDataBlock)) {
          LPT::LPT_LOG::GetInstance()->ERROR("Interporator::setup() failed");
          return -10;
        }
        OldBlockID = NewBlockID;
      } else {
        LPT::LPT_LOG::GetInstance()->WARN("DSlib->Load failed = ", retval);
        LPT::LPT_LOG::GetInstance()->WARN("Requested Block was not found. using old ParticleVelocity for this time step");
        LPT::LPT_LOG::GetInstance()->WARN("Current Time = ", Particle->CurrentTime);
        LPT::LPT_LOG::GetInstance()->WARN("ParticleID = ", Particle->GetAllID());
      }
    }

    REAL_TYPE v[3];

#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
    gus->InterpolateData(x_i, v);
    Particle->ParticleVelocity[0] = v[0] - v00[0];
    Particle->ParticleVelocity[1] = v[1] - v00[1];
    Particle->ParticleVelocity[2] = v[2] - v00[2];

    LPT::LPT_LOG::GetInstance()->LOG("Coord = ", Particle->Coord, 3);
    return 0;

  }
} // namespace PPlib
