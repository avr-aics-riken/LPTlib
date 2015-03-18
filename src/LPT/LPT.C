/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <algorithm>
#include <list>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <omp.h>

#include "LPT.h"
#include "DSlib.h"
#include "PPlib.h"
#include "Communicator.h"
#include "StartPointAll.h"
#include "DecompositionManager.h"
#include "ParticleData.h"
#include "CommDataBlock.h"
#include "LPT_LogOutput.h"
#include "PP_Transport.h"
#include "PMlibWrapper.h"
#include "PDMlib.h"
#include "MPI_Manager.h"

namespace LPT
{
std::ostream& operator<<(std::ostream& stream, LPT_InitializeArgs args)
{
    stream<<std::endl;
    stream<<"Nx,       Ny,       Nz       = "<<args.Nx<<","<<args.Ny<<","<<args.Nz<<std::endl;
    stream<<"NPx,      NPy,      NPz      = "<<args.NPx<<","<<args.NPy<<","<<args.NPz<<std::endl;
    stream<<"NBx,      NBy,      NBz      = "<<args.NBx<<","<<args.NBy<<","<<args.NBz<<std::endl;
    stream<<"dx,       dy,       dz       = "<<args.dx<<","<<args.dy<<","<<args.dz<<std::endl;
    stream<<"OriginX,  OriginY,  OriginZ  = "<<args.OriginX<<","<<args.OriginY<<","<<args.OriginZ<<std::endl;
    stream<<"GuideCellSize                = "<<args.GuideCellSize<<std::endl;
    stream<<"RefLength, RefVelocity       = "<<args.RefLength<<","<<args.RefVelocity<<std::endl;
    stream<<"CurrentTime, CurrentTimeStep = "<<args.CurrentTime<<","<<args.CurrentTimeStep<<std::endl;
    stream<<"MigrateOnRestart             = "<<std::boolalpha<<args.MigrateOnRestart<<std::endl;
    stream<<"MigrationInterval            = "<<args.MigrationInterval<<std::endl;
    stream<<"CacheSize                    = "<<args.CacheSize<<std::endl;
    stream<<"MaxRequestSize               = "<<args.MaxRequestSize<<std::endl;
    stream<<"NumInitialParticleProcs      = "<<args.NumInitialParticleProcs<<std::endl;
    stream<<"OutputDimensional            = "<<std::boolalpha<<args.OutputDimensional<<std::endl;
    return stream;
}

bool LPT::LPT_SetStartPoint(REAL_TYPE Coord1[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    StartPoints.push_back(PPlib::PointFactory(Coord1, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
}

bool LPT::LPT_SetStartPointLine(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int SumStartPoints, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    StartPoints.push_back(PPlib::LineFactory(Coord1, Coord2, SumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
}

bool LPT::LPT_SetStartPointRectangle(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    PPlib::StartPoint* tmpStartPoint = PPlib::RectangleFactory(Coord1, Coord2, NumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime);
    if(tmpStartPoint == NULL)return false;

    StartPoints.push_back(tmpStartPoint);
    return true;
}

bool LPT::LPT_SetStartPointCuboid(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    StartPoints.push_back(PPlib::CuboidFactory(Coord1, Coord2, NumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
}

bool LPT::LPT_SetStartPointCircle(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    StartPoints.push_back(PPlib::CircleFactory(Coord1, SumStartPoints, Radius, NormalVector, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
}

bool LPT::LPT_SetStartPointMovingPoints(const int& NumPoints, REAL_TYPE* Coords, double* Times, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
{
    StartPoints.push_back(PPlib::MovingPointsFactory(NumPoints, Coords, Times, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    return true;
}

int LPT::LPT_OutputParticleData(const int& TimeStep, const double& Time, REAL_TYPE v00[4])
{
    if(!initialized)return 1;

    PMlibWrapper& PM    = PMlibWrapper::GetInstance();
    PM.start("FileOutput");
    size_t NumParticles = ptrPPlib->Particles.size();
    //粒子を持っていなければ、ファイル出力せずに終了
    if(NumParticles <= 0)
    {
        PM.stop("FileOutput");
        return 0;
    }
    //粒子座標の出力
    REAL_TYPE* rwork = NULL;
    try
    {
        rwork = new REAL_TYPE[NumParticles*3];
    }
    catch(std::bad_alloc)
    {
        std::cerr<<"faild to allocate memory for OutputBuffer. ParticleData output is skipped."<<std::endl;
        return 1;
    }
    size_t index = 0;
    if(OutputDimensional)
    {
        for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
        {
            rwork[index++] = (*it)->x*RefLength;
            rwork[index++] = (*it)->y*RefLength;
            rwork[index++] = (*it)->z*RefLength;
        }
    }else{
        for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
        {
            rwork[index++] = (*it)->x;
            rwork[index++] = (*it)->y;
            rwork[index++] = (*it)->z;
        }
    }
    PDMlib::PDMlib::GetInstance().Write("Coordinate", NumParticles, rwork, (REAL_TYPE*)NULL, 3, TimeStep, Time);

    //粒子速度の出力
    PPlib::ParticleData* front = *(ptrPPlib->Particles.begin());
    float Vx;
    float Vy;
    float Vz;
    //v00[0]はフラグなので注意
    Vx = front->Vx-v00[1];
    Vy = front->Vy-v00[2];
    Vz = front->Vz-v00[3];
    REAL_TYPE u          = std::sqrt(Vx*Vx+Vy*Vy+Vz*Vz);
    REAL_TYPE vMinMax[8] = {u, u, Vx, Vx, Vy, Vy, Vz, Vz};
    index = 0;
    for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
    {
        //v00[0]はフラグなので注意
        Vx             = (*it)->Vx-v00[1];
        Vy             = (*it)->Vy-v00[2];
        Vz             = (*it)->Vz-v00[3];
        rwork[index++] = Vx;
        rwork[index++] = Vy;
        rwork[index++] = Vz;
        u              = std::sqrt(Vx*Vx+Vy*Vy+Vz*Vz);
        if(vMinMax[0] < u)vMinMax[0] = u;
        if(vMinMax[1] > u)vMinMax[1] = u;
        if(vMinMax[2] < Vx)vMinMax[2] = Vx;
        if(vMinMax[3] > Vx)vMinMax[3] = Vx;
        if(vMinMax[4] < Vy)vMinMax[4] = Vy;
        if(vMinMax[5] > Vy)vMinMax[5] = Vy;
        if(vMinMax[6] < Vz)vMinMax[6] = Vz;
        if(vMinMax[7] > Vz)vMinMax[7] = Vz;
    }
    if(OutputDimensional)
    {
        for(size_t i = 0; i < index; i++)
        {
            rwork[i] *= RefVelocity;
        }
        for(int i = 0; i < 8; i++)
        {
            vMinMax[i] *= RefVelocity;
        }
    }
    PDMlib::PDMlib::GetInstance().Write("Velocity", NumParticles, rwork, vMinMax, 3, TimeStep, Time);
    delete[] rwork;

    //IDはまとめて3要素をベクトルとして出力
    // rworkの領域が確保できていれば、こっちのnewは失敗しないはず
    int* iwork = new int[ptrPPlib->Particles.size()*3];
    index = 0;
    for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
    {
        iwork[index++] = (*it)->StartPointID1;
        iwork[index++] = (*it)->StartPointID2;
        iwork[index++] = (*it)->ParticleID;
    }
    PDMlib::PDMlib::GetInstance().Write("ID", NumParticles, iwork, (int*)NULL, 3, TimeStep, Time);
    delete[] iwork;

    const double RefTime = RefLength/RefVelocity;
    //粒子が放出された時刻
    // rworkの領域が確保できていれば、こっちのnewは失敗しないはず
    double* dwork = new double[ptrPPlib->Particles.size()];
    index = 0;
    if(OutputDimensional)
    {
        for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
        {
            dwork[index++] = (*it)->StartTime*RefTime;
        }
    }else{
        for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
        {
            dwork[index++] = (*it)->StartTime;
        }
    }
    PDMlib::PDMlib::GetInstance().Write("StartTime", NumParticles, dwork, (double*)NULL, 1, TimeStep, Time);

    //粒子の寿命
    index = 0;
    if(OutputDimensional)
    {
        for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
        {
            dwork[index++] = (*it)->LifeTime*RefTime;
        }
    }else{
        for(PPlib::ParticleContainer::iterator it = ptrPPlib->Particles.begin(); it != ptrPPlib->Particles.end(); ++it)
        {
            dwork[index++] = (*it)->LifeTime;
        }
    }
    PDMlib::PDMlib::GetInstance().Write("LifeTime", NumParticles, dwork, (double*)NULL, 1, TimeStep, Time);
    delete[] dwork;

    PM.stop("FileOutput");
    return 0;
}

int LPT::LPT_Initialize(LPT_InitializeArgs args)
{
    /*
     * LPTlibが保持する各クラスは以下の順に初期化する必要がある
     *  (1) MPI_Manager
     *  (2) PMlibWrapper, LPT_LOG
     *  (3) DecompositionManager, DSlib, PPlib
     *  (4) Communicator
     */
    if(initialized)
    {
        std::cerr<<"LPT_Initialize() is called more than 1 time"<<std::endl;
        return 1;
    }

    //MPI Manager クラスの初期化
    MPI_Manager::GetInstance()->Init(args.ParticleComm, args.FluidComm);

    //PMlibWrpperクラスの初期化
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.Initialize(args.PMlibOutputFileName, args.PMlibDetailedOutputFileName);

    PM.start("Initialize");

    //ログ出力クラスの初期化
    LPT_LOG::GetInstance()->Init(args.OutputFileName);
    LPT_LOG::GetInstance()->INFO("LPT_Initialize called");
    int ibuff[10]      = {args.Nx, args.Ny, args.Nz, args.NPx, args.NPy, args.NPz, args.NBx, args.NBy, args.NBz, args.GuideCellSize};
    REAL_TYPE rbuff[6] = {args.OriginX, args.OriginY, args.OriginZ, args.dx, args.dy, args.dz};

    MPI_Bcast(ibuff, 10, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(rbuff, 6,  MPI_INT, 0, MPI_COMM_WORLD);

    args.Nx            = ibuff[0];
    args.Ny            = ibuff[1];
    args.Nz            = ibuff[2];
    args.NPx           = ibuff[3];
    args.NPy           = ibuff[4];
    args.NPz           = ibuff[5];
    args.NBx           = ibuff[6];
    args.NBy           = ibuff[7];
    args.NBz           = ibuff[8];
    args.GuideCellSize = ibuff[9];
    args.OriginX       = rbuff[0];
    args.OriginY       = rbuff[1];
    args.OriginZ       = rbuff[2];
    args.dx            = rbuff[3];
    args.dy            = rbuff[4];
    args.dz            = rbuff[5];

    LPT_LOG::GetInstance()->INFO("LPT_Args = ", args);
    //LPTクラスの引数を取り出す
    RefLength         = args.RefLength;
    RefVelocity       = args.RefVelocity;
    OutputDimensional = args.OutputDimensional;
    const double RefTime = RefLength/RefVelocity;

    //DecompositionManagerクラスの初期化
    ptrDM = DSlib::DecompositionManager::GetInstance();
    ptrDM->Initialize(args.Nx, args.Ny, args.Nz, args.NPx, args.NPy, args.NPz, args.NBx, args.NBy, args.NBz, args.OriginX, args.OriginY, args.OriginZ, args.dx, args.dy, args.dz, args.GuideCellSize);
    LPT_LOG::GetInstance()->LOG("DecompositionManager initialized");

    int vlen                   = 3;
    const int MaxDataBlockSize = vlen*(ptrDM->GetInstance()->GetLargestBlockSize());

    //DSlibクラスの初期化
    ptrDSlib = DSlib::DSlib::GetInstance();
    ptrDSlib->Initialize(args.CacheSize*1024*1024/MaxDataBlockSize);
    LPT_LOG::GetInstance()->LOG("DSlib initialized");

    //PPlibクラスの初期化
    ptrPPlib = PPlib::PPlib::GetInstance();
    LPT_LOG::GetInstance()->LOG("PPlib initialized");

    //Comunicatorクラスの初期化
    ptrComm = new DSlib::Communicator(args.MaxRequestSize, MaxDataBlockSize);
//    ptrComm = new DSlib::Communicator(10, MaxDataBlockSize); //for rerun feature test
    LPT_LOG::GetInstance()->LOG("Communicator initialized");

    //d_bcv(FFVC内でのd_bcd)の30bit目からmask情報を取り出す
    if(MPI_Manager::GetInstance()->is_fluid_proc())
    {
        int myrank = MPI_Manager::GetInstance()->get_myrank_f();
        int N      = (ptrDM->GetSubDomainSizeX(myrank)+args.GuideCellSize*2)*(ptrDM->GetSubDomainSizeY(myrank)+args.GuideCellSize*2)*(ptrDM->GetSubDomainSizeZ(myrank)+args.GuideCellSize*2);
        Mask = new int[N];
        if(args.d_bcv != NULL)
        {
            for(int i = 0; i < N; ++i)
            {
                Mask[i] = ((args.d_bcv)[i]>>30)&0x1;
            }
        }else{
            for(int i = 0; i < N; ++i)
            {
                Mask[i] = 1;
            }
        }
        LPT_LOG::GetInstance()->LOG("Mask array initialized");
    }

    const bool restart = args.CurrentTimeStep > 0;
    if(MPI_Manager::GetInstance()->is_particle_proc())
    {
        //開始点情報をPPlibに渡す
        if(!restart)
        {
            //通常実行
            LPT_LOG::GetInstance()->INFO("Get start points from API");

            ptrPPlib->StartPoints = StartPoints;

            if(MPI_Manager::GetInstance()->get_myrank_p() == 0)
            {
                if(OutputDimensional)
                {
                    ptrPPlib->WriteStartPoints("LPTlibStartPoints.txt", RefLength, RefTime);
                }else{
                    ptrPPlib->WriteStartPoints("LPTlibStartPoints.txt", 1.0, 1.0);
                }
            }
            //PDMlibのセットアップ
            PDMlib::PDMlib::GetInstance().Init(args.argc, args.argv, args.OutputFileName+".dfi");
            PDMlib::PDMlib::GetInstance().SetBaseFileName(args.OutputFileName);
            PDMlib::PDMlib::GetInstance().SetComm(args.ParticleComm);

            PDMlib::ContainerInfo ID         = {"ID", "ID", "zip", PDMlib::INT32, "id", 3, PDMlib::NIJK};
            PDMlib::ContainerInfo Coordinate = {"Coordinate", "Coordinate", "fpzip", PDMlib::FLOAT, "coord", 3, PDMlib::NIJK};
            PDMlib::ContainerInfo Velocity   = {"Velocity", "Velocity", "fpzip", PDMlib::FLOAT, "vel", 3, PDMlib::NIJK};
            // REAL_TYPEが8byteだったら座標と速度コンテナをDOUBLEに変更
            if(sizeof(REAL_TYPE) == 8)
            {
                Coordinate.Type = PDMlib::DOUBLE;
                Velocity.Type   = PDMlib::DOUBLE;
            }
            PDMlib::ContainerInfo StartTime = {"StartTime", "starttime", "fpzip", PDMlib::DOUBLE, "start", 1};
            PDMlib::ContainerInfo LifeTime  = {"LifeTime", "lifetime", "fpzip", PDMlib::DOUBLE, "life", 1};
            PDMlib::PDMlib::GetInstance().AddContainer(ID);
            PDMlib::PDMlib::GetInstance().AddContainer(Coordinate);
            PDMlib::PDMlib::GetInstance().AddContainer(Velocity);
            PDMlib::PDMlib::GetInstance().AddContainer(StartTime);
            PDMlib::PDMlib::GetInstance().AddContainer(LifeTime);
            /*
             * BlockID, CurrentTime, CurrentTimeStepはリスタート時は不要なのでファイルには出力しない
             * PDMlib::ContainerInfo BlockID         = {"BlockID",         "BlockID",                            "zip",   PDMlib::INT64,  "temp", 1};
             * PDMlib::ContainerInfo CurrentTime     = {"CurrentTime",     "CurrentTime",                        "fpzip",  PDMlib::DOUBLE, "temp", 1};
             * PDMlib::ContainerInfo CurrentTimeStep = {"CurrentTimeStep", "CurrentTimeStep",                    "zip",   PDMlib::INT32,  "temp", 1};
             * PDMlib::PDMlib::GetInstance().AddContainer(BlockID);
             * PDMlib::PDMlib::GetInstance().AddContainer(CurrentTime);
             * PDMlib::PDMlib::GetInstance().AddContainer(CurrentTimeStep);
             */
            double bbox[6] = {args.OriginX, args.OriginY, args.OriginZ, args.OriginX+args.Nx*args.dx, args.OriginY+args.Ny*args.dy, args.OriginZ+args.Nz*args.dz};
            for(int i = 0; i < 6; i++)
            {
                bbox[i] *= args.RefLength;
            }
            PDMlib::PDMlib::GetInstance().SetBoundingBox(bbox);
            LPT_LOG::GetInstance()->LOG("PDMlib Initialized");
        }else{
            //restart実行
            LPT_LOG::GetInstance()->INFO("Read start points and Particle Data from file");
            if(OutputDimensional)
            {
                ptrPPlib->ReadStartPoints("LPTlibStartPoints.txt", RefLength, RefTime);
            }else{
                ptrPPlib->ReadStartPoints("LPTlibStartPoints.txt", 1.0, 1.0);
            }

            //PDMlibを使ってリスタートデータを読み込む
            PDMlib::PDMlib::GetInstance().Init(args.argc, args.argv, args.OutputFileName+".dfi", args.OutputFileName+".dfi");
            PDMlib::PDMlib::GetInstance().SetBaseFileName(args.OutputFileName);
            PDMlib::PDMlib::GetInstance().SetComm(args.ParticleComm);

            // コンテナの一覧を取得
            std::vector<PDMlib::ContainerInfo> containers = PDMlib::PDMlib::GetInstance().GetContainerInfo();

            int*                               ID         = NULL;
            REAL_TYPE*                         coord      = NULL;
            REAL_TYPE*                         v          = NULL;
            double*                            start      = NULL;
            double*                            life       = NULL;

            // 読み込み対象のデータポインタをPDMlibに登録する
            PDMlib::PDMlib::GetInstance().RegisterContainer("ID", &ID);
            PDMlib::PDMlib::GetInstance().RegisterContainer("Coordinate", &coord);
            PDMlib::PDMlib::GetInstance().RegisterContainer("Velocity", &v);
            PDMlib::PDMlib::GetInstance().RegisterContainer("StartTime", &start);
            PDMlib::PDMlib::GetInstance().RegisterContainer("LifeTime", &life);

            // データを読み込む
            size_t NumParticles = PDMlib::PDMlib::GetInstance().ReadAll(&args.CurrentTimeStep, args.MigrateOnRestart, "Coordinate");
            if(OutputDimensional)
            {
                for(size_t i = 0; i < 3*NumParticles; i++)
                {
                    coord[i] /= RefLength;
                    v[i]     /= RefVelocity;
                }
                for(size_t i = 0; i < NumParticles; i++)
                {
                    start[i] /= RefTime;
                    life[i]  /= RefTime;
                }
            }

            //粒子オブジェクトを作成して値を代入
            for(size_t i = 0; i < NumParticles; i++)
            {
                PPlib::ParticleData* tmp = new PPlib::ParticleData;
                tmp->StartPointID1   = ID[3*i+0];
                tmp->StartPointID2   = ID[3*i+1];
                tmp->ParticleID      = ID[3*i+2];
                tmp->x               = coord[3*i+0];
                tmp->y               = coord[3*i+1];
                tmp->z               = coord[3*i+2];
                tmp->Vx              = v[3*i+0];
                tmp->Vy              = v[3*i+1];
                tmp->Vz              = v[3*i+2];
                tmp->StartTime       = start[i];
                tmp->LifeTime        = life[i];
                tmp->CurrentTimeStep = args.CurrentTimeStep;
                tmp->CurrentTime     = args.CurrentTime;
                tmp->BlockID         = DSlib::DecompositionManager::GetInstance()->FindBlockIDByCoordLinear(tmp->x, tmp->y, tmp->z);
                ptrPPlib->Particles.insert(tmp);
            }
            delete[] ID;
            delete[] coord;
            delete[] v;
            delete[] start;
            delete[] life;
        }
    }
    // clear and minimize LPT.StartPoints
    std::vector<PPlib::StartPoint*>().swap(StartPoints);

    //開始点の分散処理
    int NumInitialParticleProcs = MPI_Manager::GetInstance()->get_nproc_p();
    if(args.NumInitialParticleProcs > 0 && args.NumInitialParticleProcs < NumInitialParticleProcs)
    {
        NumInitialParticleProcs = args.NumInitialParticleProcs;
    }
    ptrPPlib->DistributeStartPoints(NumInitialParticleProcs);
    if(!restart)ptrPPlib->OutputStartPoints(RefLength);
    LPT_LOG::GetInstance()->LOG("Distribute StartPoints done");

    //開始点情報の出力
    LPT_LOG::GetInstance()->LOG("StartPoint for this Rank");
    if(OutputDimensional)
    {
        for(std::vector<PPlib::StartPoint*>::iterator it = ptrPPlib->StartPoints.begin(); it != ptrPPlib->StartPoints.end(); ++it)
        {
            LPT_LOG::GetInstance()->INFO("", (*it)->TextPrint(RefLength, RefTime));
        }
    }else{
        for(std::vector<PPlib::StartPoint*>::iterator it = ptrPPlib->StartPoints.begin(); it != ptrPPlib->StartPoints.end(); ++it)
        {
            LPT_LOG::GetInstance()->INFO("", (*it)->TextPrint(1.0, 1.0));
        }
    }

    if(ptrDSlib == NULL || ptrDM == NULL || ptrPPlib == NULL || ptrComm == NULL)
    {
        LPT_LOG::GetInstance()->ERROR("instantiation failed!!");
        return -1;
    }

    //再送フラグの通信用領域の初期化
    //フラグの送信経路は以下のとおり
    // [全粒子計算プロセス] --MPI_Put--> [粒子計算プロセスのRank0] --MPI_Bcast--> [COMM_WORLD内の全プロセス]
    // このため、root_rank_for_rerun_flagの情報はCOMM_WORLD内の全プロセスが保持する必要がある
    root_rank_for_rerun_flag = MPI_Manager::GetInstance()->get_rank_p2w(0);
    if(MPI_Manager::GetInstance()->is_particle_proc())
    {
        if(MPI_Manager::GetInstance()->get_myrank_w() == root_rank_for_rerun_flag)
        {
            MPI_Win_create(&work_for_rerun_flag, 1, sizeof(int), MPI_INFO_NULL, MPI_Manager::GetInstance()->get_comm_p(), &window_for_rerun_flag);
        }else{
            MPI_Win_create(&work_for_rerun_flag, 0, sizeof(int), MPI_INFO_NULL, MPI_Manager::GetInstance()->get_comm_p(), &window_for_rerun_flag);
        }
        MPI_Win_fence(0, window_for_rerun_flag);
        LPT_LOG::GetInstance()->LOG("MPI_Win_create for rerun flag done!");
    }

    initialized = true;
    MPI_Barrier(MPI_COMM_WORLD);
    LPT_LOG::GetInstance()->INFO("LPT_Initialize done");
    LPT_LOG::GetInstance()->FLUSH();
    PM.stop("Initialize");
    return 0;
}

int LPT::LPT_Post(void)
{
    if(!initialized)return 1;

    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start("Post");
    delete ptrComm;
    delete[] Mask;
    if(MPI_Manager::GetInstance()->is_particle_proc())
    {
        MPI_Win_free(&window_for_rerun_flag);
    }

    PM.stop("Post");
    PM.Finalize();
    LPT_LOG::GetInstance()->INFO("LPT_Post finished.");
    return 0;
}

int LPT::LPT_CalcParticleData(LPT_CalcArgs args)
{
    static bool error_message_loged = false;
    if(!initialized)
    {
        if(!error_message_loged)
        {
            std::cerr<<"LPT_CalcParticleData is called before LPT_Initialize()"<<std::endl;
            error_message_loged = true;
        }
        return 1;
    }
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start("PrepareCalc");
    const double RefTime = RefLength/RefVelocity;
    LPT_LOG::GetInstance()->INFO("Current Time = ", args.CurrentTime*RefTime);
    LPT_LOG::GetInstance()->INFO("Current Time Step = ", args.CurrentTimeStep);
    PM.stop("PrepareCalc");

    //寿命を過ぎた開始点を破棄
    ptrPPlib->DestroyExpiredStartPoints(args.CurrentTime);

    //新規粒子の放出
    //開始点がMovingPointsの場合は座標のアップデートも行う
    ptrPPlib->EmitNewParticles(args.CurrentTime, args.CurrentTimeStep);

    //寿命を過ぎた粒子を破棄
    ptrPPlib->DestroyExpiredParticles(args.CurrentTime);

    //粒子位置および周辺のデータブロックをRequestQueueに登録
    ptrPPlib->MakeRequestQueues(ptrDSlib);

    PPlib::PP_Transport Transport;
    std::vector<std::list<PPlib::ParticleData*>*> calced;
    omp_lock_t calced_particles_lock;
    omp_init_lock(&calced_particles_lock);

    std::vector<PPlib::ParticleData*> moved;
    omp_lock_t moved_particles_lock;
    omp_init_lock(&moved_particles_lock);
    int need_to_rerun = 0;

    int fence         = 0;
    do
    {
        std::list<DSlib::CommDataBlockManager*> RecvBuff;
        std::list<DSlib::CommDataBlockManager*> SendBuff;

        //データブロックのリクエストを送り、非同期受信処理を行う
        need_to_rerun = ptrComm->CommRequest2(ptrDSlib, &RecvBuff, fence++);

        //必要なだけキャッシュ領域を空ける
        ptrDSlib->DiscardCacheEntry2(ptrDSlib->get_num_requested_block_id());

        //データブロックの送信開始
        ptrComm->SendDataBlock(args.FluidVelocity, Mask, 3, &SendBuff);

        //再送が必要な場合はフラグを粒子計算プロセスのRank0へ送る
        if(need_to_rerun && MPI_Manager::GetInstance()->is_particle_proc())
        {
            MPI_Put(&need_to_rerun, 1, MPI_INT, 0, 0, 1, MPI_INT, window_for_rerun_flag);
        }

        PM.start("CalcParticle");
        int polling_counter = NumPolling;
        //polling & calc PP_Transport
        #pragma omp parallel private(Transport)
        {
            #pragma omp single
            while(!RecvBuff.empty())
            {
                polling_counter--;
                for(std::list<DSlib::CommDataBlockManager*>::iterator it_RecvBuff = RecvBuff.begin(); it_RecvBuff != RecvBuff.end();)
                {
                    if(is_arrived(*it_RecvBuff, polling_counter))
                    {
                        long ArrivedBlockID = ptrDSlib->AddCachedBlocks((*it_RecvBuff), args.CurrentTime);
                        ptrDSlib->DeleteRequestedBlocks(ArrivedBlockID);
                        delete(*it_RecvBuff);
                        it_RecvBuff = RecvBuff.erase(it_RecvBuff);
                        PM.start("PP_Transport");
                        #pragma omp task firstprivate(ArrivedBlockID)
                        {
                            std::list<PPlib::ParticleData*>* work = ptrPPlib->Particles.find(ArrivedBlockID);
                            if(work != NULL)
                            {
                                for(std::list<PPlib::ParticleData*>::iterator it_Particle = work->begin(); it_Particle != work->end();)
                                {
                                    int ierr = Transport.Calc(*it_Particle, args.deltaT, args.divT, args.CurrentTime, args.CurrentTimeStep);
                                    LPT_LOG::GetInstance()->LOG("return value from PP_Transport::Calc() = ", ierr);
                                    if(ierr == 0 || ierr == 3 || ierr == 4 || ierr == 5)
                                    {
                                        ++it_Particle;
                                    }else if(ierr == 1){
                                        LPT_LOG::GetInstance()->INFO("Delete particle due to out of bounds: ID = ", (*it_Particle)->GetAllID());
                                        delete *it_Particle;
                                        it_Particle = work->erase(it_Particle);
                                    }else if(ierr == 2){
                                        omp_set_lock(&moved_particles_lock);
                                        moved.push_back(*it_Particle);
                                        LPT_LOG::GetInstance()->LOG("moved.size() = ", moved.size());
                                        omp_unset_lock(&moved_particles_lock);
                                        it_Particle = work->erase(it_Particle);
                                    }else{
                                        ++it_Particle;
                                        LPT_LOG::GetInstance()->ERROR("illegal return value from PP_Transport::Calc() : ParticleID = ", (*it_Particle)->GetAllID());
                                    }
                                }
                                if(work->size() > 0)
                                {
                                    omp_set_lock(&calced_particles_lock);
                                    calced.push_back(work);
                                    LPT_LOG::GetInstance()->LOG("calced.size() = ", calced.size());
                                    omp_unset_lock(&calced_particles_lock);
                                }
                            }
                        }   // omp end task
                        PM.stop("PP_Transport");
                    }else{
                        ++it_RecvBuff;
                    }
                }
            }   //omp end single
        }     //omp end parallel

        //計算済ブロックに含まれていた粒子をParticleContainerに戻す
        MoveBackToParticleContainer(calced, moved);

        //ここまでで計算できていなかった粒子を再計算
        ReCalcParticlesAll(Transport, args.deltaT, args.divT, args.CurrentTime, args.CurrentTimeStep);

        //データブロック転送を完了させて、送受信バッファを削除する
        DeleteCommBuff(&SendBuff, &RecvBuff);
        PM.stop("CalcParticle");

        //データブロックの再送フラグの通信を完了させる
        if(MPI_Manager::GetInstance()->is_particle_proc())
        {
            MPI_Win_fence(0, window_for_rerun_flag);
            if(MPI_Manager::GetInstance()->get_myrank_w() == root_rank_for_rerun_flag)
            {
                need_to_rerun       = work_for_rerun_flag;
                work_for_rerun_flag = 0;
            }
        }

        //再送フラグをCOMM_WORLD内で共有
        MPI_Bcast(&need_to_rerun, 1, MPI_INT, root_rank_for_rerun_flag, MPI_COMM_WORLD);
    }
    while(need_to_rerun);

    //キャッシュデータを全て削除
    ptrDSlib->PurgeAllCacheLists();
    return 0;
}

bool LPT::is_arrived(DSlib::CommDataBlockManager* RecvBuffManager, const int& polling_counter)
{
    //TODO
    // MPI_Test/MPI_Waitの切り替えをiteration回数ではなく
    // 送信完了の割合で指定できる方が良いかもしれない
    if(polling_counter < 1)
    {
        // 最後のPolling loopでは未受信のデータ転送を1つづつ完了させてから計算
        return RecvBuffManager->Wait();
    }else{
        return RecvBuffManager->Test();
    }
}

void LPT::DeleteCommBuff(std::list<DSlib::CommDataBlockManager*>* SendBuff, std::list<DSlib::CommDataBlockManager*>* RecvBuff)
{
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start("DelSendBuff");
    for(std::list<DSlib::CommDataBlockManager*>::iterator it_SendBuff = SendBuff->begin(); it_SendBuff != SendBuff->end();)
    {
        MPI_Status status0;
        MPI_Status status1;
        MPI_Wait(&((*it_SendBuff)->Request0), &status0);
        MPI_Wait(&((*it_SendBuff)->Request1), &status1);
        delete *it_SendBuff;
        it_SendBuff = SendBuff->erase(it_SendBuff);
    }
    for(std::list<DSlib::CommDataBlockManager*>::iterator it_RecvBuff = RecvBuff->begin(); it_RecvBuff != RecvBuff->end();)
    {
        delete *it_RecvBuff;
        it_RecvBuff = RecvBuff->erase(it_RecvBuff);
    }
    PM.stop("DelSendBuff");
}

void LPT::ReCalcParticlesAll(PPlib::PP_Transport& Transport, const double& deltaT, const int& divT, const double& CurrentTime, const int& CurrentTimeStep)
{
    PMlibWrapper& PM         = PMlibWrapper::GetInstance();
    PM.start("PP_Transport");
    long re_calced_particles = 0;
    for(PPlib::ParticleContainer::iterator it_Particle = ptrPPlib->Particles.begin(); it_Particle != ptrPPlib->Particles.end();)
    {
        int ierr = Transport.Calc(*it_Particle, deltaT, divT, CurrentTime, CurrentTimeStep);
        if(ierr == 0 || ierr == 3 || ierr == 4)
        {
            re_calced_particles++;
            ++it_Particle;
        }else if(ierr == 1){
            LPT_LOG::GetInstance()->LOG("Delete particle due to out of bounds: ID = ", (*it_Particle)->GetAllID());
            delete  *it_Particle;
            it_Particle = ptrPPlib->Particles.erase(it_Particle);
            re_calced_particles++;
        }else if(ierr == 2){
            LPT_LOG::GetInstance()->LOG("moved to another datablock: ID = ", (*it_Particle)->GetAllID());
            ptrPPlib->Particles.insert(*it_Particle);
            it_Particle = ptrPPlib->Particles.erase(it_Particle);
            re_calced_particles++;
        }else if(ierr == 5){
            ++it_Particle;
        }else{
            LPT_LOG::GetInstance()->ERROR("illegal return value from PP_Transport::Calc() : ParticleID = ", (*it_Particle)->GetAllID());
            ++it_Particle;
        }
    }
    LPT_LOG::GetInstance()->LOG("Number of Particle (re-calculated) = ", re_calced_particles);
    PM.stop("PP_Transport");
}

void LPT::MoveBackToParticleContainer(std::vector<std::list<PPlib::ParticleData*>*>& calced, std::vector<PPlib::ParticleData*>& moved)
{
    PMlibWrapper& PM = PMlibWrapper::GetInstance();
    PM.start("PP_Transport");
    //ParticleContainer::insert()の中でロックしているので、
    //ここの2つのループはOpenMP並列化しても性能向上は期待できない
    for(std::vector<std::list<PPlib::ParticleData*>*>::iterator it_ParticleList = calced.begin(); it_ParticleList != calced.end(); ++it_ParticleList)
    {
        ptrPPlib->Particles.insert(*it_ParticleList);
    }
    for(std::vector<PPlib::ParticleData*>::iterator it_Particle = moved.begin(); it_Particle != moved.end(); ++it_Particle)
    {
        ptrPPlib->Particles.insert(*it_Particle);
    }
    calced.clear();
    moved.clear();
    PM.stop("PP_Transport");
}
} // namespace LPT
