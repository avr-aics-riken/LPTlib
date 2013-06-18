#ifndef PPLIB__INTERPOLATOR_H
#define PPLIB__INTERPOLATOR_H
#include "DataBlock.h"

namespace PPlib
{
  //! @brief 流速の補間を行なうクラス
  class Interpolator
  {
  private:
    //! 流速データの配列へのポインタ
    REAL_TYPE * p_vecd;

    //! データブロックの原点座標
    REAL_TYPE m_orig[3];

    //! データブロックのセル幅
    REAL_TYPE m_pitch[3];

    //! データブロックが持つ物理量のベクトル長
    int m_vecLen;

    //! 袖領域のサイズ(単位はセル数)
    int m_halo;

    //! copy constructor
    Interpolator(const Interpolator & obj)
    {
    };
  public:
    //! データブロックの各軸方向へのセル数
    int m_dims[3];

    //! constructor
    Interpolator()
    {
      m_dims[0] = m_dims[1] = m_dims[2] = 0;
      p_vecd = NULL;
      m_vecLen = 0;
      m_orig[0] = m_orig[1] = m_orig[2] = 0.0;
      m_pitch[0] = m_pitch[1] = m_pitch[2] = 0.0;
      m_halo = 0;
    }

    //! @brief DataBlockから補間に必要なデータを取り出してInterporaltorのメンバ変数に渡す
    //! @param DataBlock [in] DSlib::DataBlock構造体を参照のこと
    bool setup(DSlib::DataBlock * DataBlock);

    //! @brief ベクトルデータの補間
    //! @param x_I  [in]  粒子座標
    //! @param didx [in]  補間対象データのindex番号
    //! @param dval [out] 補間したベクトルデータを格納する領域
    bool InterpolateData(const REAL_TYPE x_I[3], REAL_TYPE dval[3]);

    //! @brief 解析領域全体でのグローバル座標の座標値を、データブロック内のローカル座標に変換する
    //! @param [in]  x   解析領域内でのグローバル座標
    //! @param [out] x_i データブロック内でのローカル座標
    //! グローバル座標では、袖領域を含まない範囲で一番端のセルのコーナーを原点とする
    //! ローカル座標では、袖領域を含む範囲で一番端のセルの中心を原点とする
    void ConvXtoI(const REAL_TYPE x[3], REAL_TYPE x_i[3]);

    //! @brief ConvXtoIの逆変換を行う
    //! @param x_I [in]  データブロック内でのローカル座標
    //! @param x   [out] 解析領域内でのグローバル座標
    void ConvItoX(const REAL_TYPE x_I[3], REAL_TYPE x[3]);

    double GetPitchX(void)
    {
      return this->m_pitch[0];
    };
    double GetPitchY(void)
    {
      return this->m_pitch[1];
    };
    double GetPitchZ(void)
    {
      return this->m_pitch[2];
    };

  };

} // namespace PPlib
#endif
