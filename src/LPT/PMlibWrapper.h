/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PMLIB_WRAPPER_H
#define PMLIB_WRAPPER_H
#ifdef USE_PMLIB
#include <cstdlib>
#include "PerfMonitor.h"
#endif
#include "MPI_Manager.h"
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
        finalized   = false;
    }

    PMlibWrapper(const PMlibWrapper& obj);
    PMlibWrapper& operator=(const PMlibWrapper& obj);
    ~PMlibWrapper()
    {
        if(!finalized)
        {
            this->Finalize();
        }
    }

public:
    static PMlibWrapper& GetInstance()
    {
        static PMlibWrapper instance;

        return instance;
    }

    void Initialize(const std::string& argPMlibOutputFileName, const std::string& argPMlibDetailedOutputFileName)
    {
#ifdef USE_PMLIB
        if(initialized)return;

        initialized                 = true;
        PMlibOutputFileName         = argPMlibOutputFileName;
        PMlibDetailedOutputFileName = argPMlibDetailedOutputFileName;
        PM.setRankInfo(MPI_Manager::GetInstance()->get_myrank_p());
        PM.initialize();

        //排他区間
        PM.setProperties("Initialize",                 pm_lib::PerfMonitor::CALC);
        PM.setProperties("PrepareCalc",                pm_lib::PerfMonitor::CALC);
        PM.setProperties("DestroyStartPoints",         pm_lib::PerfMonitor::CALC);
        PM.setProperties("EmitParticle",               pm_lib::PerfMonitor::CALC);
        PM.setProperties("DestroyParticle",            pm_lib::PerfMonitor::CALC);
        PM.setProperties("MakeRequestQ",               pm_lib::PerfMonitor::CALC);
        PM.setProperties("SortParticle",               pm_lib::PerfMonitor::CALC);
        PM.setProperties("CalcNumComm",                pm_lib::PerfMonitor::CALC);
        PM.setProperties("CommNumComm",                pm_lib::PerfMonitor::COMM);
        PM.setProperties("PrepareComm",                pm_lib::PerfMonitor::CALC);
        PM.setProperties("AlltoAllRequest",            pm_lib::PerfMonitor::COMM);
        PM.setProperties("P2PRequest",                 pm_lib::PerfMonitor::COMM);
        PM.setProperties("CalcParticle",               pm_lib::PerfMonitor::CALC);
        PM.setProperties("Discard_Cache",              pm_lib::PerfMonitor::CALC);
        PM.setProperties("ExchangeParticleContainers", pm_lib::PerfMonitor::CALC);
        PM.setProperties("Post",                       pm_lib::PerfMonitor::CALC);
        PM.setProperties("FileOutput",                 pm_lib::PerfMonitor::CALC);

        // CalcParticleセクション内の詳細区間
        PM.setProperties("CommDataF2P",                pm_lib::PerfMonitor::COMM, false);
        PM.setProperties("MPI_Test",                   pm_lib::PerfMonitor::COMM, false);
        PM.setProperties("MPI_Wait",                   pm_lib::PerfMonitor::COMM, false);
        PM.setProperties("AddCache",                   pm_lib::PerfMonitor::CALC, false);
        PM.setProperties("PP_Transport",               pm_lib::PerfMonitor::CALC, false);
        PM.setProperties("MoveParticle",               pm_lib::PerfMonitor::CALC, false);
        PM.setProperties("DelSendBuff",                pm_lib::PerfMonitor::CALC, false);

        //! memo
        //PP_Transport::Calc()内で呼びだされるルーチンにも区間を設定したことがあったが
        //1call あたり1e-6以下の実行時間となっておりタイマの解像度以下だったため測定には使えかった。

        PM.setParallelMode("FlatMPI", 1, MPI_Manager::GetInstance()->get_nproc_p());
#endif
    }

private:
    ///PMlibのインスタンス
#ifdef USE_PMLIB
    pm_lib::PerfMonitor PM;
#endif

    //! PMlibの情報を出力するファイル名
    std::string PMlibOutputFileName;

    //! PMlibの詳細情報を出力するファイル名
    std::string PMlibDetailedOutputFileName;

public:
    //Public Method
    void Finalize(void)
    {
#ifdef USE_PMLIB
        if(finalized)return;

        finalized = true;

        PM.gather();
        if(MPI_Manager::GetInstance()->get_myrank_p() == 0)
        {
            FILE* fp1;
            FILE* fp2;

            fp1 = fopen(PMlibOutputFileName.c_str(), "w+");
            fp2 = fopen(PMlibDetailedOutputFileName.c_str(), "w+");

            std::string hostname("HOSTNAME");
            std::string operator_name("USER");
            if(std::getenv("HOSTNAME") != NULL)hostname = std::getenv("HOSTNAME");
            if(std::getenv("USER") != NULL)operator_name = std::getenv("USER");

            PM.print(fp1, hostname, operator_name);
            PM.printDetail(fp2);
            fclose(fp1);
            fclose(fp2);
        }
#endif
    }

    void start(const std::string& key)
    {
#ifdef USE_PMLIB
        PM.start(key);
#endif
    }

    void stop(const std::string& key, double flopPerTask = 0.0, unsigned iterationCount = 1)
    {
#ifdef USE_PMLIB
        PM.stop(key, flopPerTask, iterationCount);
#endif
    }
};
} //namespace LPT
#endif
