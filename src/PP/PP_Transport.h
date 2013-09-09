#ifndef PPLIB_PP_TRANSPORT_H
#define PPLIB_PP_TRANSPORT_H

#include <iostream>

#include <vector>
#include <list>
#include "PerfMonitor.h"
#include "Interpolator.h"
//forward declaration
namespace DSlib
{
  class DSlib;
}
namespace PPlib
{
//forward declaration
  class ParticleData;

  //! @brief 粒子データの保持、管理とマイグレーション処理を行なう
  class PP_Transport
  {
  private:
    /// 物理量の補間を行うオブジェクトへのポインタ
    Interpolator * gus;

    /// Load済DataBlockのID
    long OldBlockID;

    /// Load済DataBlockへのポインタ
      DSlib::DataBlock * LoadedDataBlock;

  public:
      PP_Transport():OldBlockID(-1), LoadedDataBlock(NULL)
    {
      gus = new Interpolator;
    };
     ~PP_Transport()
    {
      delete gus;
    };

    //! @brief 引数で与えられた粒子データの流速場に沿った移動を計算する
    //! @retval 0   正常終了
    //! @retval 1   必要な流速場が未着だったため、粒子を呼び出し前の状態に戻した
    //! @retval 2   未要求の流速場が必要になったため、粒子を呼び出し前の状態に戻した
    //! @retval -1  このタイムステップでは転送要求していないブロックに粒子が移動したため、計算を打ち切った
    //! 返り値が1の時は通信を完了させた後で、再計算を行なう
    //! 返り値が2の時は次以降の通信ループ内で計算を行なう
    //! 返り値が-1の時は、このタイムステップではこの粒子の計算は行なわない
    //! TODO 解析領域外に出た時の処理は、このルーチン内で完結させること
    //! 流速データは引数で受けとるDSlibのオブジェクトへのポインタから読み出す
    //!
    int Calc(ParticleData * Particle, const double &deltaT, const int &divT, REAL_TYPE * v00, DSlib::DSlib * ptrDSlib, const double &CurrentTime, const unsigned int &CurrentTimeStep);
  };

} // namespace PPlib
#endif
