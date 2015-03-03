/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <iostream>
#include <cmath>

#include "StartPointCircle.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
std::string Circle::TextPrint(void) const
{
    std::ostringstream oss;
    oss<<typeid(*this).name()<<std::endl;
    oss<<"Coord1               = "<<this->Coord1[0]<<","<<this->Coord1[1]<<","<<this->Coord1[2]<<std::endl;
    oss<<"NormalVector         = "<<this->NormalVector[0]<<","<<this->NormalVector[1]<<","<<this->NormalVector[2]<<std::endl;
    oss<<"Radius               = "<<this->Radius<<std::endl;
    oss<<"SumStartPoints       = "<<this->SumStartPoints<<std::endl;
    oss<<"StartTime            = "<<this->StartTime<<std::endl;
    oss<<"ReleaseTime          = "<<this->ReleaseTime<<std::endl;
    oss<<"TimeSpan             = "<<this->TimeSpan<<std::endl;
    oss<<"LatestEmitTime       = "<<this->LatestEmitTime<<std::endl;
    oss<<"ID                   = "<<this->ID[0]<<","<<this->ID[1]<<std::endl;
    oss<<"LatestEmitParticleID = "<<this->LatestEmitParticleID<<std::endl;
    return oss.str();
}

void Circle::ReadText(std::istream& stream)
{
    std::string work;
    //Coord1
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->Coord1[0] = std::atof(work.c_str());
    std::getline(stream, work, ',');
    this->Coord1[1] = std::atof(work.c_str());
    std::getline(stream, work);
    this->Coord1[2] = std::atof(work.c_str());
    //NormalVector
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->NormalVector[0] = std::atof(work.c_str());
    std::getline(stream, work, ',');
    this->NormalVector[1] = std::atof(work.c_str());
    std::getline(stream, work);
    this->NormalVector[2] = std::atof(work.c_str());

    //Radius
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->Radius = std::atof(work.c_str());

    //SumStartPoints
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->SumStartPoints = std::atoi(work.c_str());

    //StartTime
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->StartTime = std::atof(work.c_str());
    //ReleaseTime
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->ReleaseTime = std::atof(work.c_str());
    //TimeSpan
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->TimeSpan = std::atof(work.c_str());
    //LatestEmitTime
    std::getline(stream, work, '=');
    std::getline(stream, work);
    this->LatestEmitTime = std::atof(work.c_str());
    //ID
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->ID[0] = std::atoi(work.c_str());
    std::getline(stream, work);
    this->ID[1] = std::atoi(work.c_str());

    //LatestEmitParticleID
    std::getline(stream, work, '=');
    std::getline(stream, work, ',');
    this->LatestEmitParticleID = std::atoi(work.c_str());
}

void Circle::NormalizeVector(REAL_TYPE* v)
{
    REAL_TYPE length = std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    if(length != 0.0)
    {
        v[0] = v[0]/length;
        v[1] = v[1]/length;
        v[2] = v[2]/length;
    }
}

void Circle::ConvertNormalVector(void)
{
    //zが負の時はNormalVectorを逆向きにする
    if(NormalVector[2] < 0)
    {
        NormalVector[0] = -NormalVector[0];
        NormalVector[1] = -NormalVector[1];
        NormalVector[2] = -NormalVector[2];
    }
    NormalizeVector(NormalVector);
}

void Circle::MakeOuterProducts(REAL_TYPE* OuterProducts)
{
    //z軸方向の単位ベクトルを Vz={0,0,1}とすると
    //OuterProducts[0] = Vz[1]*NormalVector[2]-Vz[2]*NormalVector[1];
    //OuterProducts[1] = Vz[2]*NormalVector[0]-Vz[0]*NormalVector[2];
    //OuterProducts[2] = Vz[0]*NormalVector[1]-Vz[1]*NormalVector[0];
    //
    //0となる項を消すと次行のようになる。

    OuterProducts[0] = -NormalVector[1];
    OuterProducts[1] =  NormalVector[0];
    OuterProducts[2] =  0;

    NormalizeVector(OuterProducts);
}

void Circle::MakeRotationMatrix(REAL_TYPE* OuterProducts)
{
    REAL_TYPE cos = (NormalVector[2]);
    REAL_TYPE sin = std::sqrt(1-cos*cos);

    R[0] = OuterProducts[0]*OuterProducts[0]*(1-cos)+cos;
    R[1] = OuterProducts[0]*OuterProducts[1]*(1-cos)-OuterProducts[2]*sin;
    R[2] = OuterProducts[0]*OuterProducts[2]*(1-cos)+OuterProducts[1]*sin;

    R[3] = OuterProducts[1]*OuterProducts[0]*(1-cos)+OuterProducts[2]*sin;
    R[4] = OuterProducts[1]*OuterProducts[1]*(1-cos)+cos;
    R[5] = OuterProducts[1]*OuterProducts[2]*(1-cos)-OuterProducts[0]*sin;

    R[6] = OuterProducts[2]*OuterProducts[0]*(1-cos)-OuterProducts[1]*sin;
    R[7] = OuterProducts[2]*OuterProducts[1]*(1-cos)+OuterProducts[0]*sin;
    R[8] = OuterProducts[2]*OuterProducts[2]*(1-cos)+cos;
}

bool Circle::isReasonable_N_and_a(void)
{
    REAL_TYPE dr = Radius/N;
    REAL_TYPE dl = 2*Radius*std::sin(2*M_PI/(a*N)/2);

    return 0.5 < dr/dl && dr/dl < 1.5;
}

bool Circle::Make_N_and_a(void)
{
    int NumStartPointsWithoutCenter = SumStartPoints-1;
    //半径方向の分割数(N)を決める
    for(int i = 1; i < NumStartPointsWithoutCenter; i++)
    {
        N = i;
        a = NumStartPointsWithoutCenter/(N*(N+1)/2);

        if(isReasonable_N_and_a())
        {
            LPT::LPT_LOG::GetInstance()->LOG("N = ", N);
            LPT::LPT_LOG::GetInstance()->LOG("a = ", a);
            break;
        }
    }
    SumStartPoints = CalcSumStartPoints();
    return isReasonable_N_and_a();
}

int Circle::CalcSumStartPoints()
{
    int tmpSumStartPoints = 0;
    for(int i = Istart; i <= Iend; i++)
    {
        for(int j = 0; j < i*a; j++)
        {
            double theta = 2*M_PI/(i*a)*j;
            if(theta_min <= theta && theta < theta_max)
            {
                ++tmpSumStartPoints;
            }
        }
    }
    //円の中心の座標はtheta_min=0の領域のみに含める
    if(theta_min == 0)
    {
        ++tmpSumStartPoints;
    }
    return tmpSumStartPoints;
}

bool Circle::Initialize(void)
{
    if(!Make_N_and_a())
    {
        LPT::LPT_LOG::GetInstance()->WARN("Couldn't find reasonable a and N");
        return false;
    }

    ConvertNormalVector();

    REAL_TYPE OuterProducts[3];
    MakeOuterProducts(OuterProducts);

    MakeRotationMatrix(OuterProducts);

    //開始点の密度 開始点数/円の面積(無次元) を計算してログに出力
    LPT::LPT_LOG::GetInstance()->INFO("Number of Start point = ", GetSumStartPoints());
    LPT::LPT_LOG::GetInstance()->INFO("Start point density   = ", GetSumStartPoints()/(M_PI*Radius*Radius));
    return true;
}

void Circle::GetGridPointCoord(std::vector<REAL_TYPE>& Coords)
{
    for(int i = Istart; i <= Iend; i++)
    {
        REAL_TYPE r = Radius/N*i;

        for(int j = 0; j < i*a; j++)
        {
            double theta = 2*M_PI/(i*a)*j;

            if(theta_min <= theta && theta < theta_max)
            {
                REAL_TYPE x = r*std::cos(theta);
                REAL_TYPE y = r*std::sin(theta);
                Coords.push_back(R[0]*x+R[1]*y+Coord1[0]);
                Coords.push_back(R[3]*x+R[4]*y+Coord1[1]);
                Coords.push_back(R[6]*x+R[7]*y+Coord1[2]);
            }
        }
    }
    //円の中心の座標はtheta_min=0の領域のみに含める
    if(theta_min == 0)
    {
        Coords.push_back(Coord1[0]);
        Coords.push_back(Coord1[1]);
        Coords.push_back(Coord1[2]);
    }
}

void Circle::Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints)
{
    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピー(へのポインタ)を
    //vectorに格納して終了
    if(MaxNumStartPoints >= GetSumStartPoints())
    {
        Circle* NewCircle = CircleFactory(this->Coord1, this->SumStartPoints, this->Radius, this->NormalVector, this->StartTime, this->ReleaseTime, this->TimeSpan, this->ParticleLifeTime);
        StartPoints->push_back(NewCircle);
        LPT::LPT_LOG::GetInstance()->LOG("GetSumStartPoints = ", GetSumStartPoints());
        LPT::LPT_LOG::GetInstance()->LOG("MaxNumStartPoints= ", MaxNumStartPoints);
        return;
    }else if(MaxNumStartPoints <= 0){
        // MaxNumStartPointsが0以下の時はエラーメッセージを出力して終了
        LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
        return;
    }

    //余りオブジェクトが持つ開始点数
    int NumReminder = GetSumStartPoints()%MaxNumStartPoints;

    //余りオブジェクトを除いた分割後のオブジェクトの数
    int NumParts = GetSumStartPoints()/MaxNumStartPoints;
    LPT::LPT_LOG::GetInstance()->LOG("NumParts = ", NumParts);
    LPT::LPT_LOG::GetInstance()->LOG("Reminder = ", NumReminder);

    //分割後のオブジェクトが持つ開始点数
    int NumGridPoints = (GetSumStartPoints()-NumReminder)/NumParts;

    //余りオブジェクトの中心角はオブジェクトに含まれる開始点数の比を元に
    //初期値を決めて、オブジェクトに含まれる開始点数がNumReminderよりも少ない場合は
    //すこしづつ中心角を広げる
    //逆に余りオブジェクトに余計に開始点が含まれている場合はそのままにする
    double ReminderCentralAngle = 0.0;
    if(NumReminder != 0)
    {
        ReminderCentralAngle = ((double)NumReminder/(double)SumStartPoints)*2*M_PI;
        Circle* NewCircle = CircleFactory(this->Coord1, this->SumStartPoints, this->Radius, this->NormalVector, this->StartTime, this->ReleaseTime, this->TimeSpan, this->ParticleLifeTime);
        NewCircle->theta_min = 2*M_PI-ReminderCentralAngle;
        double  tick = (2*M_PI-ReminderCentralAngle)/NumParts/NumGridPoints;

        while(NumReminder > NewCircle->CalcSumStartPoints())
        {
            NewCircle->theta_min -= tick;
        }
        LPT::LPT_LOG::GetInstance()->LOG("theta_min = ", NewCircle->theta_min);
        LPT::LPT_LOG::GetInstance()->LOG("theta_max = ", NewCircle->theta_max);
        LPT::LPT_LOG::GetInstance()->LOG("ReminderCentralAngle = ", ReminderCentralAngle);
        NewCircle->SumStartPoints = NewCircle->CalcSumStartPoints();
        StartPoints->push_back(NewCircle);
    }

    const double CentralAngle = (2*M_PI-ReminderCentralAngle)/NumParts;
    LPT::LPT_LOG::GetInstance()->LOG("ReminderCentralAngle = ", ReminderCentralAngle);
    LPT::LPT_LOG::GetInstance()->LOG("CentralAngle = ", CentralAngle);
    for(int i = 0; i < NumParts; ++i)
    {
        Circle* NewCircle = CircleFactory(this->Coord1, this->SumStartPoints, this->Radius, this->NormalVector, this->StartTime, this->ReleaseTime, this->TimeSpan, this->ParticleLifeTime);

        NewCircle->theta_min = i*CentralAngle;
        NewCircle->theta_max = (i+1)*CentralAngle;
        LPT::LPT_LOG::GetInstance()->LOG("theta_min = ", NewCircle->theta_min);
        LPT::LPT_LOG::GetInstance()->LOG("theta_max = ", NewCircle->theta_max);
        NewCircle->SumStartPoints = NewCircle->CalcSumStartPoints();
        StartPoints->push_back(NewCircle);
    }
}
} // namespace PPlib