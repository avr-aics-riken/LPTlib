#ifndef PPLIB__INTERPOLATOR_H
#define PPLIB__INTERPOLATOR_H

//forward declaration
namespace DSlib
{
  class  DataBlock;
}
namespace PPlib
{
  //! @brief 流速の補間を行なうクラス
  class Interpolator
  {
  public:
    //! @brief ベクトルデータの補間
    //! @param x_I  [in]  粒子座標
    //! @param didx [in]  補間対象データのindex番号
    //! @param dval [out] 補間したベクトルデータを格納する領域
    static bool InterpolateData(const DSlib::DataBlock& DataBlock, const REAL_TYPE x_I[3], REAL_TYPE dval[3]);

    //! @brief 解析領域全体でのグローバル座標の座標値を、データブロック内のローカル座標に変換する
    //! @param [in]  x   解析領域内でのグローバル座標
    //! @param [out] x_i データブロック内でのローカル座標
    //! グローバル座標では、袖領域を含まない範囲で一番端のセルのコーナーを原点とする
    //! ローカル座標では、袖領域を含む範囲で一番端のセルの中心を原点とする
    static void ConvXtoI(const REAL_TYPE x_g[3], REAL_TYPE x_l[3], const REAL_TYPE orig[3], const REAL_TYPE pitch[3]);

    //! @brief ConvXtoIの逆変換を行う
    //! @param x_I [in]  データブロック内でのローカル座標
    //! @param x   [out] 解析領域内でのグローバル座標
    static void ConvItoX(const REAL_TYPE x_l[3], REAL_TYPE x_g[3], const REAL_TYPE orig[3], const REAL_TYPE pitch[3]);
  };

} // namespace PPlib
#endif
