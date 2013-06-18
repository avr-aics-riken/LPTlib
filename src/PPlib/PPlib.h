#ifndef PPLIB_P_PLIB_H
#define PPLIB_P_PLIB_H

#include <iostream>

#include <vector>
#include <list>
#include "PerfMonitor.h"
//forward declaration
namespace DSlib
{
  class DSlib;
  class DecompositionManager;
}
namespace PPlib
{
//forward declaration
  class ParticleData;
  class StartPoint;

  //! @brief 粒子データの保持、管理とマイグレーション処理を行なう
  class PPlib
  {
  private:
  public:
    //! 担当する開始点データへのポインタのvector
    std::vector < StartPoint * >StartPoints;

    //!  @brief 計算を担当する粒子データのオブジェクトへのポインタを格納する。
    //!  粒子が存在する位置のブロックID毎にソートしておく
    //!  
    //!  粒子が放出される時点では、ブロックIDは不明なので
    //!  map等のソート済コンテナは使えない。
    //!  放出-->BlockIDの問い合わせ-->ソート の処理が必要
    //!  ソートを容易にするために、listで実装している 2012/8/14
    std::list < ParticleData * >Particles;

    //! @brief データブロックが到着して、現在計算中の粒子データのオブジェクトへのポインタを保存するリスト
    //! タイムステップ終了時には、Particlesに全て移動し、サイズは0になっている(計算できなかった粒子も戻す)
    std::list < ParticleData * >WorkingParticles;

    //!  このタイムステップでの計算が終わった粒子データのオブジェクトへのポインタを保存するリスト
    std::list < ParticleData * >CalcedParticles;

    //! @brief StartPointsに登録されている全ての開始点から粒子を放出させる
    //! 開始点がMovingPoints型だった場合は現在時刻に応じた位置へ移動させてから粒子を放出する
    //! @param CurrentTime [in] 現在時刻
    void EmitNewParticles(const double &CurrentTime, const unsigned int &CurrentTimeStep);

    //! @brief Particlesに登録されている粒子が存在する位置のデータブロックIDをDSlib::RequestQueuesに登録する
    void MakeRequestQueues(DSlib::DecompositionManager * ptrDM, DSlib::DSlib * ptrDSlib);

    //! @brief StartPointsに登録されている個々の開始点データのリリースタイムをチェックし、現在時刻がリリースタイムを越えていたらその開始点のインスタンスを破棄する
    //! @param CurrentTime [in] 現在時刻
    void DestroyExpiredStartPoints(const double &CurrentTime);

    //! @brief Particlesに登録されている個々の粒子データの寿命をチェックし、現在時刻が寿命を越えていたらその粒子のインスタンスを破棄する
    //! @param CurrentTime [in] 現在時刻
    void DestroyExpiredParticles(const double &CurrentTime);

    //! @brief 与えられたオブジェクト(粒子または開始点を想定)の寿命と現在時刻を比較
    //! @param CurrentTime [in] 現在時刻
    //! @retval true  このオブジェクトは破棄の対象
    //! @retval false このオブジェクトは破棄の対象外
      template < class T > bool Check(const double &CurrentTime, T * obj);

    //! @brief Particlesに登録されている個々の粒子データの寿命をチェックし、現在時刻が寿命を越えていたらその粒子のインスタンスを破棄する
    //! @param CurrentTime [in] 現在時刻
    void CheckLifeTime(const double CurrentTime);

    //! @brief 引数で与えられた粒子データを、Particlesの適切な位置に追加する
    //! ParticlesはブロックID順にソートされているので、追加しようとする粒子がと同じブロックに存在する粒子データが、既にParticlesにあれば、その直後、無ければ末尾に追加する
    //! 新規に放出された粒子は、ブロックIDを調べていないのでこのルーチンでは追加しない。
    //! マイグレーションで受け取った粒子に対して使う？
    //! @param ParticleData * [in] 粒子データのインスタンスへのポインタ
    //! @param ptdDSlib       [in] DSlibのインスタンスへのポインタ、FindBlockIDByCoord()を呼ぶためにとりあえず渡している
    void AddParticle(ParticleData * Particle, DSlib::DecompositionManager * ptrDM);

    //! 引数で指定されたBlockIDの粒子データをParticlesからWorkingParticlesへ移動させる
    void MoveParticleByBlockID(long BlockID);

    //! メンバ変数CalcParticlesとParticlesを入れ替える
    void ExchangeParticleContainers(void)
    {
      Particles.swap(CalcedParticles);
    }

    //! @brief  粒子のマイグレーションが必要かどうか判定する
    void DetermineMigration();

    //! @brief 粒子のマイグレーションを行なう
    void MigrateParticle();

    //!  引数で指定されたプロセス数を目標に、開始点のデータ分散を行なう
    void DistributeStartPoints(const int &NParticleProcs);

  };

} // namespace PPlib
#endif
