/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef MPI_MANAGER_H
#define MPI_MANAGER_H
#include <mpi.h>
namespace LPT
{
//!@brief MPIプロセス数、Rank番号などの情報を一元管理するクラス
class MPI_Manager
{
private:
    //Singletonパターンを適用
    MPI_Manager() : initialized(false){}
    MPI_Manager(const MPI_Manager& obj);
    MPI_Manager& operator=(const MPI_Manager& obj);
    ~MPI_Manager(){}

public:
    static MPI_Manager* GetInstance()
    {
        static MPI_Manager instance;
        return &instance;
    }

    void Init(const MPI_Comm& comm_lpt = MPI_COMM_WORLD, const MPI_Comm& comm_fluid = MPI_COMM_WORLD)
    {
        this->comm_lpt   = comm_lpt;
        this->comm_fluid = comm_fluid;
        MPI_Comm_rank(this->comm_lpt, &myrank);
        MPI_Comm_size(this->comm_lpt,   &nproc);
        MPI_Comm_size(this->comm_fluid, &nproc_in_fluid);
        MPI_Comm_size(MPI_COMM_WORLD,   &nproc_in_world);
        initialized = true;
    }

    MPI_Comm comm_lpt;           //!< 粒子計算を担当するプロセスが所属するコミュニケータ
    MPI_Comm comm_fluid;         //!< 流体計算を担当するプロセスが所属するコミュニケータ
    int      myrank;             //!< comm_lpt内での自RankのRank番号
    int      nproc;              //!< comm_lpt内のプロセス数
    int      nproc_in_fluid;     //!< comm_fluid内のプロセス数
    int      nproc_in_world;     //!< CommWorld内のプロセス数

public:
    int get_myrank(void){return this->myrank;}
    int get_nproc(void){return this->nproc;}
    int get_nproc_in_fulid(void){return this->nproc_in_fluid;}
    int get_nproc_in_world(void){return this->nproc_in_world;}
    MPI_Comm get_comm_lpt(void){return this->comm_lpt;}
    MPI_Comm get_comm_fluid(void){return this->comm_fluid;}
    bool is_ready(void){return this->initialized;}

private:
    bool initialized;
};
}
#endif
