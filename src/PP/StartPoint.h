/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_START_POINT_H
#define PPLIB_START_POINT_H

#include <cmath>
#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <Utility.h>

namespace PPlib
{
//forward declaration
struct ParticleData;

//! @brief 開始点の情報を保持するクラス群の基底クラス
//
//! 全Rankが全てのインスタンスを1回生成し、データ分散の処理をした後で
//! 自Rankが担当するインスタンス以外を破棄する
//! 自Rankが担当する開始点領域のインスタンスはPPlib::StartPointsに登録して管理する
class StartPoint
{
    //!@attention コピー不可
    StartPoint(const StartPoint& org);
    StartPoint& operator=(const StartPoint& org);

public:
    //! Constructor
    StartPoint(){}

    //! Destructor
    virtual ~StartPoint(){}

    //! 開始点情報をテキストファイルとして返す
    virtual std::string TextPrint(void) const = 0;

    //! 入力ストリームからTextPrintで返された形式の開始点情報を読み込む
    virtual void ReadText(std::istream& stream) = 0;

    //! @brief 開始点を移動させる
    //! 基底クラスでは何もしない。
    //! 現時点ではMobingPointsのみがオーバーライドしている
    virtual void UpdateStartPoint(double CurrentTime){}

    //! @brief 開始点オブジェクトをMaxNumStartPointsで指定した開始点数以下のオブジェクトに分割する。
    //! 余りが生じた場合は1つ余計にオブジェクトを生成し、そのオブジェクトに余り領域を入れて返す
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param MaxNumStartPoints [in]  分割後のオブジェクトが持つ最大の開始点数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    virtual void Divider(std::vector<StartPoint*>* StartPoints, const int& MaxNumStartPoints) = 0;

    //! @brief 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    virtual void GetGridPointCoord(std::vector<REAL_TYPE>& Coords) = 0;

    //! @brief LatestEmitTimeからTimeSpan時間経過していた場合に、新しく粒子を放出する
    void EmitNewParticle(std::list<ParticleData*>* ParticleList, const double& CurrentTime, const int& CurrentTimeStep);

    //! CurrentTimeがこの開始点の寿命を越えている場合(すなわち StartTime+ReleaseTime < CurrentTime の時)Trueを返す
    bool CheckReleaseTime(const double& CurrentTime);

    void SetID(int* ID)
    {
        for(int i = 0; i < 2; i++)
        {
            this->ID[i] = ID[i];
        }
    }

    std::string GetID(void)
    {
        std::ostringstream oss;
        oss<<this->ID[0]<<","<<this->ID[1];
        return oss.str();
    }

    virtual int GetSumStartPoints(void){return this->SumStartPoints;}

    int GetID1(void){return this->ID[0];}

    int GetID2(void){return this->ID[1];}

    double GetStartTime(void){return this->StartTime;}

    double GetLatestEmitTime(void){return this->LatestEmitTime;}

    double GetReleaseTime(void){return this->ReleaseTime;}

    //! GetReleaseTimeに対する別名。粒子の寿命判定と共用するために用意している。
    double GetLifeTime(void){return GetReleaseTime();}

    double GetTimeSpan(void){return this->TimeSpan;}

    double GetParticleLifeTime(void){return this->ParticleLifeTime;}

    int GetLatestEmitParticleID(void){return this->LatestEmitParticleID;}

    //!  sort用のファンクタ
    bool operator<(const StartPoint& obj)
    {
        return SumStartPoints < obj.SumStartPoints;
    }

    bool operator>(const StartPoint& obj)
    {
        return SumStartPoints > obj.SumStartPoints;
    }

    static bool isGreater(StartPoint* obj1, StartPoint* obj2)
    {
        return obj1->SumStartPoints > obj2->SumStartPoints;
    }

protected:
    //! @brief 2点間を指定された数で等分した位置の座標を返す
    //! @param Coords    [out] 座標値をx1,y1,z1,x2,y2,z2, ... ,xn,yn,znの順に3n個収めたvector
    //! @param NumPoints [in]  分割数
    //! @param Coord1    [in]  端点の座標その1
    //! @param Coord2    [in]  端点の座標その2
    void DividePoints(std::vector<REAL_TYPE>* Coords, const int& NumPoints, const REAL_TYPE Coord1[3], const REAL_TYPE Coord2[3]);

    int SumStartPoints;              //!< 1つの設定で定義される開始点の数
                                     //!< インスタンス生成時に設定され、変更はしない
                                     //!< Point, MovingPointsの場合は必ず1
                                     //!< LineおよびCircleではユーザが指定した値を用いる
                                     //!< Rectangle, Cuboidの場合はFactoryMethod内で指定されたNumStartPoints[3]の値から計算する

    double StartTime;                //!< 開始点から粒子が放出され始める時刻(0以下の場合は解析開始時刻から放出開始)
    double ReleaseTime;              //!< 開始点が有効な期間 (0以下の場合は解析終了まで有効)
    double TimeSpan;                 //!< 開始点から粒子が放出される間隔
    double LatestEmitTime;           //!< 直近で粒子を放出した時刻
    double ParticleLifeTime;         //!< 開始点から放出される粒子の寿命
    int    LatestEmitParticleID;     //!< これまでに放出した粒子の数
    int    ID[2];                    //!< 開始点のID
                                     //!< 1要素目はこの開始点の処理を担当するRankのRank番号
                                     //!< 2要素目は0から昇順で付けられるID番号
                                     //!< LPT_Initialize()内で開始点の担当プロセスが決まってから値が設定される
};
} // namespace PPlib
#endif
