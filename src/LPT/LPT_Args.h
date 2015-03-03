/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef LPT_LPT_ARGS_H
#define LPT_LPT_ARGS_H

namespace LPT
{
//! LPT_Initialize()の引数を保持する構造体
struct LPT_InitializeArgs
{
    int argc;                     //!< main関数のargc
    char** argv;                  //!< main関数のargv

    int Nx;                       //!< 計算領域のx方向のサイズ 単位はセル数
    int Ny;                       //!< 計算領域のy方向のサイズ 単位はセル数
    int Nz;                       //!< 計算領域のz方向のサイズ 単位はセル数
    int NPx;                      //!< x軸方向の分割数(サブドメイン数)
    int NPy;                      //!< y軸方向の分割数(サブドメイン数)
    int NPz;                      //!< z軸方向の分割数(サブドメイン数)
    int NBx;                      //!< 1サブドメインあたりのx軸方向のデータブロック数
    int NBy;                      //!< 1サブドメインあたりのx軸方向のデータブロック数
    int NBz;                      //!< 1サブドメインあたりのx軸方向のデータブロック数
    REAL_TYPE dx;                 //!< x方向のセル幅
    REAL_TYPE dy;                 //!< y方向のセル幅
    REAL_TYPE dz;                 //!< z方向のセル幅
    REAL_TYPE OriginX;            //!< 原点のX座標
    REAL_TYPE OriginY;            //!< 原点のY座標
    REAL_TYPE OriginZ;            //!< 原点のZ座標
    int GuideCellSize;            //!< ガイドセル(袖領域)のサイズ
    int* d_bcv;                   //!< セルが流体か固体かを示すマスク配列(30bit目のみを使用)

    REAL_TYPE RefLength;          //!< 代表長さ
    REAL_TYPE RefVelocity;        //!< 代表速度

    MPI_Comm ParticleComm;        //!< 粒子計算で使うコミュニケータ
    MPI_Comm FluidComm;           //!< 流体計算で使うコミュニケータ

    std::string OutputFileName;                  //!< PDMlibに渡すベースファイル名
    std::string PMlibOutputFileName;             //!< PMlibの情報を出力するファイル名
    std::string PMlibDetailedOutputFileName;     //!< PMlibの詳細情報を出力するファイル名

    float CurrentTime;                //!< リスタート計算をする時の開始時刻
    int CurrentTimeStep;              //!< リスタート計算をする時の開始タイムステップ (1以上の時はリスタート計算とみなす)
    bool MigrateOnRestart;            //!< リスタートデータの読み込み時にマイグレーションするかどうかのフラグ

    int MigrationInterval;     //!< マイグレーションの判定を行なうタイムステップ間隔

    int CacheSize;         //!< データブロックのキャッシュに使う領域のサイズ(単位はMByte)
    int MaxRequestSize;    //!< 1プロセスあたりの最大同時データブロック要求数

    int NumInitialParticleProcs; //!< 粒子計算に使う初期プロセス数

    bool OutputDimensional;      //!< ファイル出力を有次元に換算してから行うかどうかのフラグ
    


    //! Constructor
    LPT_InitializeArgs() :
        RefLength(1.0),
        RefVelocity(1.0),
        ParticleComm(MPI_COMM_WORLD),
        FluidComm(MPI_COMM_WORLD),
        OutputFileName("ParticleData"),
        PMlibOutputFileName("PMlibOutput.txt"),
        PMlibDetailedOutputFileName("PMlibDetailedOutput.txt"),
        CurrentTime(0.0),
        CurrentTimeStep(0),
        MigrationInterval(-1),
        MigrateOnRestart(false),
        CacheSize(1024),
        MaxRequestSize(2700),
        NumInitialParticleProcs(-1),
        OutputDimensional(true)
    {}
};

//! @brief LPT_CalcParticleDataの引数を保持する構造体
struct LPT_CalcArgs
{
    double CurrentTime; //!< 現在時刻
    int CurrentTimeStep; //!< 現在のタイムステップ
    double deltaT; //!< 時間積分幅 無次元
    double divT; //!< 粒子移動の積分に使う刻み幅の再分割数
    REAL_TYPE* FluidVelocity; //!< 流速データのポインタ
};
} // namespace LPT
#endif
