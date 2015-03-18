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
//@attention LPT_LOG class内でMPI_Managerを呼んでいるので、このclass内ではLPT_LOGは使えない
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

    void Init(const MPI_Comm& comm_particle = MPI_COMM_WORLD, const MPI_Comm& comm_fluid = MPI_COMM_WORLD)
    {
        this->comm_particle = comm_particle;
        this->comm_fluid    = comm_fluid;

        MPI_Comm_rank(MPI_COMM_WORLD, &myrank_in_world);
        MPI_Comm_size(MPI_COMM_WORLD, &nproc_in_world);
        if(this->comm_particle != MPI_COMM_NULL)
        {
            MPI_Comm_rank(this->comm_particle, &myrank_in_particle);
            MPI_Comm_size(this->comm_particle, &nproc_in_particle);
        }else{
            myrank_in_particle = -1;
        }

        if(this->comm_fluid != MPI_COMM_NULL)
        {
            MPI_Comm_rank(this->comm_fluid, &myrank_in_fluid);
            MPI_Comm_size(this->comm_fluid, &nproc_in_fluid);
        }else{
            myrank_in_fluid = -1;
        }
        //nproc_in_fluidをRank0からBcast
        MPI_Bcast(&nproc_in_fluid,    1, MPI_INT, 0,                MPI_COMM_WORLD);

        //nproc_in_particleを最大RankからBcast
        MPI_Bcast(&nproc_in_particle, 1, MPI_INT, nproc_in_world-1, MPI_COMM_WORLD);

        //rank map tableの初期化
        rank_table_particle_to_world = new int[nproc_in_particle];
        for(int i = 0; i < nproc_in_particle; i++)
        {
            rank_table_particle_to_world[i] = -1;
        }
        rank_table_fluid_to_world = new int[nproc_in_fluid];
        for(int i = 0; i < nproc_in_fluid; i++)
        {
            rank_table_fluid_to_world[i] = -1;
        }

        //rank map tableの作成
        int* work = new int[nproc_in_world];

        MPI_Allgather(&myrank_in_particle, 1, MPI_INT, work, 1, MPI_INT, MPI_COMM_WORLD);
        for(int i = 0; i < nproc_in_world; i++)
        {
            if(work[i] != -1)
            {
                rank_table_particle_to_world[work[i]] = i;
            }
        }
        MPI_Allgather(&myrank_in_fluid, 1, MPI_INT, work, 1, MPI_INT, MPI_COMM_WORLD);
        for(int i = 0; i < nproc_in_world; i++)
        {
            if(work[i] != -1)
            {
                rank_table_fluid_to_world[work[i]] = i;
            }
        }

        // rank map tableに問題が無いか確認 (デバッグ用)
        for(int i = 0; i < nproc_in_particle; i++)
        {
            if(rank_table_particle_to_world[i] == -1)
            {
                std::cerr<<"rank table for particle_comm is worng!"<<std::endl;
            }
        }
        for(int i = 0; i < nproc_in_fluid; i++)
        {
            if(rank_table_fluid_to_world[i] == -1)
            {
                std::cerr<<"rank table for fluid_comm is worng!"<<std::endl;
            }
        }
        delete[] work;
        initialized = true;
    }

    MPI_Comm comm_particle;                //!< 粒子計算を担当するプロセスが所属するコミュニケータ
    MPI_Comm comm_fluid;                   //!< 流体計算を担当するプロセスが所属するコミュニケータ
    int      myrank_in_particle;           //!< comm_particle内での自RankのRank番号
    int      nproc_in_particle;            //!< comm_particle内のプロセス数
    int      myrank_in_fluid;              //!< comm_fluid内での自RankのRank番号
    int      nproc_in_fluid;               //!< comm_fluid内のプロセス数
    int      myrank_in_world;              //!< MPI_COMM_WORLD内での自RankのRank番号
    int      nproc_in_world;               //!< MPI_COMM_WORLD内のプロセス数
    int*     rank_table_particle_to_world; //!< comm_particle内でのrank番号とMPI_COMM_WORLD内でのrank番号のテーブル
    int*     rank_table_fluid_to_world;    //!< comm_particle内でのrank番号とMPI_COMM_WORLD内でのrank番号のテーブル

public:
    //! comm_particle内でのRank番号を返す
    int get_myrank_p(void){return this->myrank_in_particle;}

    //! comm_particle内のプロセス数を返す
    int get_nproc_p(void){return this->nproc_in_particle;}

    //! comm_fluid内でのRank番号を返す
    int get_myrank_f(void){return this->myrank_in_fluid;}

    //! comm_fluid内のプロセス数を返す
    int get_nproc_f(void){return this->nproc_in_fluid;}

    //! MPI_COMM_WORLD内でのRank番号を返す
    int get_myrank_w(void){return this->myrank_in_world;}

    //! MPI_COMM_WORLD内のプロセス数を返す
    int get_nproc_w(void){return this->nproc_in_world;}

    //! 粒子計算プロセス用のコミュニケータを返す
    MPI_Comm get_comm_p(void){return this->comm_particle;}

    //! 流体計算プロセス用のコミュニケータを返す
    MPI_Comm get_comm_f(void){return this->comm_fluid;}

    //! 初期化済かどうかを返す
    bool is_ready(void){return this->initialized;}

    //! 粒子計算を担当するプロセスかどうかを返す
    bool is_particle_proc(void){return myrank_in_particle != -1;}

    //! 流体計算を担当するプロセスかどうかを返す
    bool is_fluid_proc(void){return myrank_in_fluid != -1;}

    //! 引数で渡されたcomm_particle内のrank番号をcomm_world内でのrank番号に変換して返す
    int get_rank_p2w(int rank){return this->rank_table_particle_to_world[rank];}

    //! 引数で渡されたcomm_fluid内のrank番号をcomm_world内でのrank番号に変換して返す
    int get_rank_f2w(int rank){return this->rank_table_fluid_to_world[rank];}

private:
    bool initialized;
};
}
#endif