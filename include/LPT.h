#ifndef LPT_L_P_T_H
#define LPT_L_P_T_H

#include <iostream>
#include <vector>
#include <string>
#include <mpi.h>
#include "LPT_Args.h"

namespace DSlib
{
  class DSlib;
  class Communicator;
  class DecompositionManager;
}
namespace PPlib
{
  class PPlib;
  class StartPoint;
}
namespace pm_lib
{
  class PerfMonitor;
}

namespace LPT
{
  //! @brief LPTのインターフェースルーチンを提供するクラス
  //! 計算に必要なパラメータ、他クラスへのポインタを保持し
  //! それらを呼び出すことで粒子計算および必要な初期化、後処理を行なう
  class LPT
  {
  private:
    //! 自RankのRank番号
    int MyRank;

    //! CommWolrd内のプロセス数
    int NProc;

    //! データブロックの到着をポーリングする回数
    int NumPolling;

    //! @brief ソルバー側からLPT_SetStartPoint*() 経由で渡されてきた開始点のインスタンスを一時保存するvector
    //! PPlibのインスタンス生成後にそっちに渡して中身は破棄する
      std::vector < PPlib::StartPoint * >StartPoints;

    //! @brief データブロックのキャッシュに使う領域のサイズ。単位はMByte
    //! 実際はsizeof(DataBlock)でまるめられる
    int CacheSize;

    //! @brief キャッシュから1度に追い出すサイズ
    //! 単位はMByteで指定し、sizeof(DataBlock)で丸められる
    //! CommBufferSize <= CacheSize でなければならない
    //! CacheSizeを越えた値が指定された場合はCacheSizeと同じ値に変更する
    int CommBufferSize;

    //! 粒子計算に参加するプロセス数の初期値
    int NumParticleProcs;

    //! セルが固体(=0)か流体(=1)かを示すマスク配列
    int *Mask;

    //! 現在時刻
    double CurrentTime;

    //! 現在のタイムステップ
    unsigned int CurrentTimeStep;

    //! ファイル入出力メソッドで使われるファイル名のprefix
    //! この後にRank番号を付け加えたものが実際のファイル名となる
      std::string OutputFileName;

  public:
    //! DSlibのオブジェクトへのポインタ
      DSlib::DSlib * ptrDSlib;

    //! Communicatorのオブジェクトへのポインタ
      DSlib::Communicator * ptrComm;

    //! DecompositionManagerのオブジェクトへのポインタ
      DSlib::DecompositionManager * ptrDM;

    //! PPlibのオブジェクトへのポインタ
      PPlib::PPlib * ptrPPlib;

    //!  @brief 粒子計算に必要なパラメータを受け取り、DSlib, PPlib等のインスタンスを生成する
    //! 開始点のインスタンス生成およびデータ分散もこの関数内で処理する
    //! @param args [in] 引数構造体
    int LPT_Initialize(LPT_InitializeArgs args);

    //! @brief 粒子計算を行なう
    //! 返り値はエラーコード
    //! 0:正常
    //! @param args [in] 引数構造体
    int LPT_CalcParticleData(LPT_CalcArgs args);

    //! @brief  呼び出された時点での粒子データをファイルに出力する
    //! ファイル名は "LPT::OutputFileName + Rank番号" とし
    //! プロセス毎に異なるファイルに出力する
    int LPT_OutputParticleData(void);

    //! 粒子データを出力し、LPTの内部で保持している全ての開始点, 粒子, 流速データを破棄する
    int LPT_Post(void);

    //! 開始点のインスタンスを生成するためのインターフェース関数群
    bool LPT_SetStartPoint(REAL_TYPE Coord1[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointMoovingPoints(REAL_TYPE * Coord, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointLine(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int SumStartPoints, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointRectangle(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointCuboid(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointCircle(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);

    //! 現在時刻を取得
    double GetCurrentTime()
    {
      return this->CurrentTime;
    };
    //! 現在のタイムステップを取得
    unsigned int GetCurrentTimeStep()
    {
      return this->CurrentTimeStep;
    };
    //! MPI_Testによるポーリング回数を取得
    int GetNumPolling()
    {
      return this->NumPolling;
    };

    //! MPI_Testによるポーリング回数を設定
    void SetNumPolling(const int &NumPolling)
    {
      this->NumPolling = NumPolling;
    };

  private:
    //Singletonパターンを適用
    LPT() {
      MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
      MPI_Comm_size(MPI_COMM_WORLD, &NProc);
      NumPolling = 10;
    }
    LPT(const LPT & obj);
    LPT & operator=(const LPT & obj);

    //! デストラクタ
    ~LPT() {
    }

  public:
    static LPT *GetInstance()
    {
      static LPT instance;
      return &instance;
    }

  };

} // namespace LPT
#endif
