#ifndef PPLIB_P_P__INTEGRATOR_H
#define PPLIB_P_P__INTEGRATOR_H

#include <iostream>
#include <cmath>

#include "Interpolator.h"

namespace PPlib
{
  //! @brief 4次のルンゲ=クッタ法による速度場の積分を行なう。
  class PP_Integrator
  {
    //! @brief ルンゲ=クッタ用の係数を計算する。
    //! @param gus     [in]   Interpolatorのインスタンス
    //! @param x_i     [in]   粒子座標
    //! @param func    [out]  ルンゲ=クッタ積分用の係数を計算した結果を格納する
    static bool GetIntegrand(Interpolator & gus, const REAL_TYPE x_i[3], REAL_TYPE func[3]);

  public:
    //! @brief 4次ルンゲ=クッタ法による速度場の積分を行なう
    //! @param gus     [in]    Interpolatorのインスタンス
    //! @param t_step  [in]    ルンゲ=クッタ積分の時間刻み
    //! @param x_i     [inout] 粒子座標
    static int RKG(Interpolator & gus, const double t_step, REAL_TYPE x_i[3]);

  };

} // namespace PPlib
#endif
