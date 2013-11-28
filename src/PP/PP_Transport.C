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
  void PP_Transport::UpdateParticle(ParticleData * Particle, const double &CurrentTime, const unsigned int &CurrentTimeStep, REAL_TYPE * Coord )
  {
    Particle->CurrentTime = CurrentTime;
    Particle->CurrentTimeStep = CurrentTimeStep;
    Particle->Coord[0] = Coord[0];
    Particle->Coord[1] = Coord[1];
    Particle->Coord[2] = Coord[2];
  }
  int PP_Transport::Calc(ParticleData * Particle, const double &deltaT, const int &divT, REAL_TYPE * v00, DSlib::DSlib * ptrDSlib, const double &CurrentTime, const unsigned int &CurrentTimeStep)
  {
    //もし計算済の粒子だったらすぐにreturn
    if(CurrentTimeStep <= Particle->CurrentTimeStep) return 0;

    DSlib::DecompositionManager * ptrDM = DSlib::DecompositionManager::GetInstance();
    REAL_TYPE x_i[3];
    REAL_TYPE x_new[3];
    REAL_TYPE v[3];

    LPT::LPT_LOG::GetInstance()->LOG("Old BlockID = ", LoadedBlockID);

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
    LPT::LPT_LOG::GetInstance()->LOG("Coord before calc = ", x_new, 3);

    long old_BlockID_in_ParticleData=-1;
    long NewBlockID=-1;
    for(int t = 0; t < numT; t++) {
      NewBlockID = ptrDM->FindBlockIDByCoordLinear(x_new);
      if(LoadedBlockID != NewBlockID) {
        LPT::LPT_LOG::GetInstance()->LOG("New BlockID = ", NewBlockID);

        int retval = ptrDSlib->Load(NewBlockID, &LoadedDataBlock);

        if(retval == 1){
          LPT::LPT_LOG::GetInstance()->WARN("DataBlock is not arrived: ", NewBlockID);
          //粒子データは変更せずに終了
          return 3;
        } else if(retval == 2) {
          //粒子データは変更せずに終了
          LPT::LPT_LOG::GetInstance()->WARN("DataBlock is not requested at this time: ", NewBlockID);
          return 3;
        } else if(retval == -1) {
          //現在の粒子座標をこのタイムステップでの更新後の座標として計算を終了
          UpdateParticle(Particle, CurrentTime, CurrentTimeStep, x_new);
          Particle->BlockID = NewBlockID;

          LPT::LPT_LOG::GetInstance()->WARN("Particle moved more than 2 blocks away");
          return 4;
        }
        if(!gus->setup(LoadedDataBlock)) {
          LPT::LPT_LOG::GetInstance()->ERROR("Interporator::setup() failed");
          return -10;
        }
        LoadedBlockID = NewBlockID;
      }

      // 粒子座標をデータブロック内の座標値に変換
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
      gus->ConvXtoI(x_new, x_i);

      // ルンゲ=クッタ積分
      int rkg = PP_Integrator::RKG(*gus, dt, x_i);
      if(rkg != 0) LPT::LPT_LOG::GetInstance()->WARN("return value from PP_Integrator::RKG = ", rkg);

      //粒子位置での速度を計算(ParticleDataに代入するのはCalcの最後)
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
      gus->InterpolateData(x_i, v);

      // 粒子座標の逆変換
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
      gus->ConvItoX(x_i, x_new);

      if(ptrDM->CheckBounds(x_new) != 0) return 1;
    } // end of for(t)

    //粒子オブジェクトの座標、時刻、タイムステップを更新
    UpdateParticle(Particle, CurrentTime, CurrentTimeStep, x_new);

    //移動後の位置でのブロックIDと粒子速度を代入
    NewBlockID = ptrDM->FindBlockIDByCoordLinear(Particle->Coord);
    Particle->BlockID = NewBlockID;

    if(LoadedBlockID != NewBlockID) {
      if(ptrDSlib->Load(NewBlockID, &LoadedDataBlock) == 0) {
        LoadedBlockID = NewBlockID;
        //粒子がルンゲ=クッタの最後のステップで別のデータブロックに移動し
        //なおかつ、移動先のデータブロックがキャッシュ内にある場合のみ速度を再計算
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
        gus->InterpolateData(x_i, v);
      } else {
        LPT::LPT_LOG::GetInstance()->WARN("Requested Block was not found. using old ParticleVelocity for this time step");
        LPT::LPT_LOG::GetInstance()->WARN("Current Time = ", Particle->CurrentTime);
        LPT::LPT_LOG::GetInstance()->WARN("ParticleID = ", Particle->GetAllID());
      }
    }

    Particle->ParticleVelocity[0] = v[0] - v00[0];
    Particle->ParticleVelocity[1] = v[1] - v00[1];
    Particle->ParticleVelocity[2] = v[2] - v00[2];

    LPT::LPT_LOG::GetInstance()->LOG("Coord after calc = ", Particle->Coord, 3);

    return old_BlockID_in_ParticleData == Particle->BlockID ? 0:2;
  }
} // namespace PPlib
