#ifndef PPLIB_PP_TRANSPORT_H
#define PPLIB_PP_TRANSPORT_H

#include <iostream>

#include <vector>
#include <list>
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
    PP_Transport(const PP_Transport & obj){}
  public:
      PP_Transport()
    {
      gus = new Interpolator;
    };
     ~PP_Transport()
    {
      delete gus;
    };

    //! @brief 引数で与えられた粒子データの流速場に沿った移動を計算する
    //! @retval 0 正常終了
    //! @retval 1 解析領域外に移動した
    //! @retval 2 移動開始前とは違うデータブロックに移動したが計算は完了した
    //! @retval 3 未着のデータブロックに移動したため計算を中断した
    //! @retval 4 未要求のデータブロックに移動したため計算を中断した
    //! 返り値が1の時は呼び出し元で粒子オブジェクトを削除すること
    //! 返り値が2の時は呼び出し元でコンテナからの削除&再挿入を行う
    //! 返り値が3の時は再計算用リストに登録すること
    //! 再計算用リストに登録された粒子データは全ての通信が完了した後で再計算を行う
    //! 返り値が4の時は計算終了とみなすので、呼び出し元では処理は不要
    int Calc(ParticleData * Particle, const double &deltaT, const int &divT, REAL_TYPE * v00, DSlib::DSlib * ptrDSlib, const double &CurrentTime, const unsigned int &CurrentTimeStep);

  private:
    /// 物理量の補間を行うオブジェクトへのポインタ
    Interpolator * gus;

    //! @brief 粒子データの時刻、タイムステップ、座標を更新する
    void UpdateParticle(ParticleData * Particle, const double &CurrentTime, const unsigned int &CurrentTimeStep, REAL_TYPE * Coord );
  };

} // namespace PPlib
#endif
