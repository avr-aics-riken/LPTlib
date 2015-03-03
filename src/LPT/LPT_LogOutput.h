/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef LPT_LOG_OUTPUT_H
#define LPT_LOG_OUTPUT_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "MPI_Manager.h"

namespace LPT
{
//! @brief メッセージ出力を行なうクラス
class LPT_LOG
{
    std::ofstream logfile;

private:
    //Singletonパターンを適用
    LPT_LOG(){}
    ~LPT_LOG(){}
    LPT_LOG(const LPT_LOG& obj);
    LPT_LOG& operator=(const LPT_LOG& obj);

public:
    static LPT_LOG* GetInstance()
    {
        static LPT_LOG instance;
        return &instance;
    }

    void Init(const std::string& BaseFileName)
    {
        int                myrank = MPI_Manager::GetInstance()->get_myrank();
        std::ostringstream oss;
        oss<<myrank;
        std::string        filename(BaseFileName);
        filename += "_";
        filename += oss.str();
        filename += ".log";
        logfile.open(filename.c_str());
    }

    void LOG(std::string message)
    {
#if defined(LPT_LOG_ENABLE) || defined(LPT_VERBOSE)
        logfile<<"LPT LOG  : "<<message<<"\n";
        logfile.flush();
#endif
    }

    template<typename T>
    void LOG(std::string message, T value)
    {
#if defined(LPT_LOG_ENABLE) || defined(LPT_VERBOSE)
        logfile<<"LPT LOG  : "<<message<<value<<"\n";
        logfile.flush();
#endif
    }

    template<typename T>
    void LOG(std::string message, T* value, int max)
    {
#if defined(LPT_LOG_ENABLE) || defined(LPT_VERBOSE)
        logfile<<"LPT LOG  : "<<message;
        for(int i = 0; i < max-1; i++)
        {
            logfile<<value[i]<<",";
        }
        logfile<<value[max-1]<<"\n";
        logfile.flush();
#endif
    }

    void INFO(std::string message)
    {
        logfile<<"LPT INFO : "<<message<<"\n";
#if defined(LPT_DEBUG) || defined(DEBUG)
        logfile.flush();
#endif
    }

    template<typename T>
    void INFO(std::string message, T value)
    {
        logfile<<"LPT INFO : "<<message<<value<<"\n";
#if defined(LPT_DEBUG) || defined(DEBUG)
        logfile.flush();
#endif
    }

    template<typename T>
    void INFO(std::string message, T* value, int max)
    {
        logfile<<"LPT INFO : "<<message;
        for(int i = 0; i < max-1; i++)
        {
            logfile<<value[i]<<",";
        }
        logfile<<value[max-1]<<"\n";
#if defined(LPT_DEBUG) || defined(DEBUG)
        logfile.flush();
#endif
    }

    void WARN(std::string message)
    {
        logfile<<"LPT WARN : "<<message<<"\n";
        logfile.flush();
    }

    template<typename T>
    void WARN(std::string message, T value)
    {
        logfile<<"LPT WARN : "<<message<<value<<"\n";
        logfile.flush();
    }

    void ERROR(std::string message)
    {
        logfile<<"LPT ERROR: "<<message<<std::endl;
        logfile.flush();
    }

    template<typename T>
    void ERROR(std::string message, T value)
    {
        logfile<<"LPT ERROR : "<<message<<value<<std::endl;
        logfile.flush();
    }
};
} // namespace LPT
#endif
