#ifndef PMLIB_WRAPPER_H
#define PMLIB_WRAPPER_H
#ifdef USE_PMLIB
#include <cstdlib>
#include "PerfMonitor.h"
#endif
namespace LPT
{
//! @brief PMlibのインスタンスを一つ保持し、GetInstanceを経由したグローバルなアクセスを提供する
//  
//  コンパイル時にUSE_PMLIBが指定されていなければ、GetInstance以外の全メソッドは何もしない
  class PMlibWrapper
  {
    //Singletonパターン  start
    /// Initialize()が呼ばれたかどうかのフラグ
    bool initialized;

    /// Finalize()が呼ばれたかどうかのフラグ
    bool finalized;

  private:
      PMlibWrapper()
    {
      initialized = false;
      finalized = false;
    }
    PMlibWrapper(const PMlibWrapper & obj);
    PMlibWrapper & operator=(const PMlibWrapper & obj);
    ~PMlibWrapper()
    {
      if(!finalized)
      {
        this->Finalize();
      }
    }

  public:
    static PMlibWrapper & GetInstance()
    {
      static PMlibWrapper instance;

      return instance;
    }
    void Initialize(const std::string & argPMlibOutputFileName, const std::string & argPMlibDetailedOutputFileName)
    {
#ifdef USE_PMLIB
     if(initialized) return;
     initialized=true;
     PMlibOutputFileName=argPMlibOutputFileName;
     PMlibDetailedOutputFileName=argPMlibDetailedOutputFileName;
     int MyRank;
     int NumProc;
     MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
     MPI_Comm_size(MPI_COMM_WORLD, &NumProc);
     PM.setRankInfo(MyRank);
     PM.initialize(tm_END_);

     //排他区間
     PM.setProperties(tm_Initialize,                 "Initialize",                 pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_PrepareCalc,                "PrepareCalc",                pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_DestroyStartPoints,         "DestroyStartPoints",         pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_EmitParticle,               "EmitParticle",               pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_DestroyParticle,            "DestroyParticle",            pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_MakeRequestQ,               "MakeRequestQ",               pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_SortParticle,               "SortParticle",               pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_CalcNumComm,                "CalcNumComm",                pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_CommNumComm,                "CommNumComm",                pm_lib::PerfMonitor::COMM);
     PM.setProperties(tm_PrepareComm,                "PrepareComm"  ,              pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_AlltoAllRequest,            "AlltoAllRequest",            pm_lib::PerfMonitor::COMM);
     PM.setProperties(tm_P2PRequest,                 "P2PRequest",                 pm_lib::PerfMonitor::COMM);
     PM.setProperties(tm_CalcParticle,               "CalcParticle",               pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_Discard_Cache,              "Discard_Cache",              pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_ExchangeParticleContainers, "ExchangeParticleContainers", pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_Post,                       "Post",                       pm_lib::PerfMonitor::CALC);
     PM.setProperties(tm_FileOutput,                 "FileOutput",                 pm_lib::PerfMonitor::CALC);

     // CalcParticleセクション内の詳細区間
     PM.setProperties(tm_CommDataF2P,         "CommDataF2P",        pm_lib::PerfMonitor::COMM, false);
     PM.setProperties(tm_MPI_Test,            "MPI_Test",           pm_lib::PerfMonitor::COMM, false);
     PM.setProperties(tm_MPI_Wait,            "MPI_Wait",           pm_lib::PerfMonitor::COMM, false);
     PM.setProperties(tm_AddCache,            "AddCache",           pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_PP_Transport,        "PP_Transport",       pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_MoveParticle,        "MoveParticle",       pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_DelSendBuff,         "DelSendBuff",        pm_lib::PerfMonitor::CALC, false);

     //PP_Transport内の詳細区間
     /* 全て1e-6/call 以下の実行時間となっていたためこの区間の設定を削除
     PM.setProperties(tm_Transport1,                  "PPTransport",                pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_FindBlockID,       "FindBlockID_in_PPTransport", pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_Load,              "Load",                       pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_SetupInterPorator, "SetupInterPorator",          pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_ConvXtoI,          "ConvXtoI",                   pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_Interpolate,       "Interpolate",                pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_RKG,               "Runge_Kutta",                pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_ConvItoX,          "ConvItoX",                   pm_lib::PerfMonitor::CALC, false);
     PM.setProperties(tm_Transport_Post,              "PPTransport_Post",           pm_lib::PerfMonitor::CALC, false);
     */
     PM.setParallelMode("FlatMPI", 1, NumProc);
#endif
    }
  private:
    //Singletonパターン  end

    ///PMlibのインスタンス
#ifdef USE_PMLIB
    pm_lib::PerfMonitor PM;
#endif

    //! PMlibの情報を出力するファイル名
    std::string PMlibOutputFileName;

    //! PMlibの詳細情報を出力するファイル名
    std::string PMlibDetailedOutputFileName;


public:
  enum timing_key {
    tm_Initialize,
    tm_PrepareCalc,
    tm_DestroyStartPoints,
    tm_EmitParticle,
    tm_DestroyParticle,
    tm_MakeRequestQ,
    tm_SortParticle,
    tm_CalcNumComm,
    tm_CommNumComm,
    tm_PrepareComm,
    tm_AlltoAllRequest,
    tm_P2PRequest,
    tm_CalcParticle,
    tm_Discard_Cache,
    tm_ExchangeParticleContainers,
    tm_Post,
    tm_FileOutput,
    tm_CommDataF2P,
    tm_MPI_Test,   
    tm_MPI_Wait,   
    tm_AddCache,
    tm_PP_Transport,
    tm_MoveParticle,
    tm_DelSendBuff,
    tm_END_
  };

    //Public Method
    void Finalize(void)
    {
#ifdef USE_PMLIB
      if(finalized)
        return;
      finalized = true;
      int MyRank;

      MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
      PM.gather();
      if(MyRank == 0) {
        FILE *fp1;
        FILE *fp2;

        fp1 = fopen(PMlibOutputFileName.c_str(), "w+");
        fp2 = fopen(PMlibDetailedOutputFileName.c_str(), "w+");

        std::string hostname("HOSTNAME");
        std::string operator_name("USER");
        if(std::getenv("HOSTNAME") != NULL ) hostname=std::getenv("HOSTNAME");
        if(std::getenv("USER") != NULL) operator_name = std::getenv("USER");

        PM.print(fp1, hostname, operator_name);
        PM.printDetail(fp2);
        fclose(fp1);
        fclose(fp2);
      }
#endif
    }

    void start(unsigned key)
    {
#ifdef USE_PMLIB
      PM.start(key);
#endif
    }

    void stop(unsigned key, double flopPerTask = 0.0, unsigned iterationCount = 1) {
#ifdef USE_PMLIB
      PM.stop(key, flopPerTask, iterationCount);
#endif
    }
  };

} //namespace LPT
#endif
