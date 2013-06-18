#ifndef LPT_LPT_ARGS_H
#define LPT_LPT_ARGS_H

namespace LPT
{
/**
 * @brief LPT_Initialize()の引数を保持する構造体
 */
  struct LPT_InitializeArgs
  {
    //! 計算領域のx方向のサイズ 単位はセル数
    int Nx;

    //! 計算領域のy方向のサイズ 単位はセル数
    int Ny;

    //! 計算領域のz方向のサイズ 単位はセル数
    int Nz;

    //! x軸方向の分割数(サブドメイン数)
    int NPx;

    //! y軸方向の分割数(サブドメイン数)
    int NPy;

    //! z軸方向の分割数(サブドメイン数)
    int NPz;

    //! 1サブドメインあたりのx軸方向のデータブロック数
    int NBx;

    //! 1サブドメインあたりのx軸方向のデータブロック数
    int NBy;

    //! 1サブドメインあたりのx軸方向のデータブロック数
    int NBz;

    //! x方向のセル幅
    REAL_TYPE dx;

    //! y方向のセル幅
    REAL_TYPE dy;

    //! z方向のセル幅
    REAL_TYPE dz;

    //!  原点のX座標
    REAL_TYPE OriginX;

    //!  原点のY座標
    REAL_TYPE OriginY;

    //!  原点のZ座標
    REAL_TYPE OriginZ;

    //! @brief ガイドセル(袖領域)のサイズ
    //! 全方向に同じサイズの袖領域を持つものとする
    int GuideCellSize;

    //! 流体計算で使うコミュニケータ
    MPI_Comm FluidComm;

    //! @brief 流体計算に参加するプロセス数
    //! NumFluidProc = NPx*NPy*NPz となっていること
    int NumFluidProc;

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

    //! @brief 粒子計算に参加するプロセス数の最大値
    //!  未指定の場合は、MPI_COMM_WORLD内のプロセス数とする
    int MaxNumParticleProcs;

    //! MigrationIntervalステップに1回、マイグレーションの判定を行なう
    int MigrationInterval;

    //! @brief ファイル入出力メソッドで使われるファイル名のprefix
    //! この後にRank番号を付け加えたものが実際のファイル名となる
      std::string OutputFileName;

    //!  FFV-Cで使用しているビットマスクの配列
    int *d_bcv;

    //!  PMlibの情報を出力するファイル名
      std::string PMlibOutputFileName;

    //! PMlibの詳細情報を出力するファイル名
      std::string PMlibDetailedOutputFileName;

    //! 代入オペレータ
    void operator=(const LPT_InitializeArgs & org)
    {
      Nx = org.Nx;
      Ny = org.Ny;
      Nz = org.Nz;
      NPx = org.NPx;
      NPy = org.NPy;
      NPz = org.NPz;
      NBx = org.NBx;
      NBy = org.NBy;
      NBz = org.NBz;
      dx = org.dx;
      dy = org.dy;
      dz = org.dz;
      OriginX = org.OriginX;
      OriginY = org.OriginY;
      OriginZ = org.OriginZ;
      CacheSize = org.CacheSize;
      CommBufferSize = org.CommBufferSize;
      NumParticleProcs = org.NumParticleProcs;
      MaxNumParticleProcs = org.MaxNumParticleProcs;
      MigrationInterval = org.MigrationInterval;
      OutputFileName = org.OutputFileName;
      PMlibOutputFileName = org.PMlibOutputFileName;
      PMlibDetailedOutputFileName = org.PMlibDetailedOutputFileName;
    }

    //! 入出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, LPT_InitializeArgs args);
    friend std::istream & operator >>(std::istream & stream, LPT_InitializeArgs & args);

    //! Constructor
      LPT_InitializeArgs()
    {
      FluidComm = MPI_COMM_WORLD;
      MPI_Comm_size(FluidComm, &NumFluidProc);
      CacheSize = 1024;
      CommBufferSize = 512;
      NumParticleProcs = NumFluidProc;
      MaxNumParticleProcs = NumFluidProc;
      MigrationInterval = 100;
      OutputFileName = "ParticleData";
      PMlibOutputFileName = "PMlibOutput.txt";
      PMlibDetailedOutputFileName = "PMlibDetailedOutput.txt";
    }
  };

  //! @brief LPT_CalcParticleDataの引数を保持する構造体
  struct LPT_CalcArgs
  {
    //! 現在時刻
    double CurrentTime;

    //! 現在のタイムステップ
    unsigned int CurrentTimeStep;

    //! 時間積分幅 無次元
    double deltaT;

    //! @brief 粒子移動の積分に使う刻み幅の再分割数
    //! PP_Transport()内では、deltaT/divT を積分幅として扱う
    double divT;

    //! 流速データのポインタ
    REAL_TYPE *FluidVelocity;

    //! 参照速度へのポインタ
    REAL_TYPE *v00;

    //! 代入オペレータ
    void operator=(const LPT_CalcArgs & org)
    {
      CurrentTime = org.CurrentTime;
      deltaT = org.deltaT;
      divT = org.divT;
      FluidVelocity = org.FluidVelocity;
    }
  };

} // namespace LPT
#endif
