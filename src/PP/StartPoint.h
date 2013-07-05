#ifndef PPLIB_START_POINT_H
#define PPLIB_START_POINT_H

#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <vector>
#include "DV3.h"

namespace PPlib
{
//forward declaration
  struct ParticleData;

  //! @brief 開始点の情報を保持するクラス群の基底クラス
  //! 全Rankが一旦インスタンスを生成し、データ分散の処理をした後で自Rankが担当するインスタンス以外を破棄する
  class StartPoint
  {
  protected:
    //! @brief 1つの設定で定義される開始点の数
    //! インスタンス生成時に設定され、変更はしない
    //! Point, MovingPointsの場合は必ず1
    //! LineおよびCircleではユーザが指定した値を用いる
    //! Rectangle, Cuboidの場合はインスタンス生成時に指定されるNumStartPoints[3]の値から
    //! 計算し自動的に値を代入する。
    int SumStartPoints;

    //! @brief 開始点から粒子が放出され始める時刻
    //! 0以下の場合は解析開始時刻から放出を開始する
    double StartTime;

    //! @brief 開始点が有効な期間
    //! StartTimeからReleaseTime時間が経過するとその開始点設定は破棄される
    //! 0以下の場合は解析終了まで有効
    double ReleaseTime;

    //! @brief 開始点から粒子が放出される間隔
    //! 現在時刻が (前回の放出時刻 + TimeSpan) 以降の時のみ粒子を放出する
    double TimeSpan;

    //!  直近で粒子を放出した時刻
    double LatestEmitTime;

    //! 開始点から放出される粒子の寿命
    double ParticleLifeTime;

    //! @brief 開始点のID
    //! 1要素目はこの開始点の処理を担当するRankのRank番号
    //! 2要素目は0から昇順で付けられるID番号
    //!  LPT_Initialize()内で開始点の担当プロセスが決まってから値が設定される
    int ID[2];

  public:
    //! CurrentTimeがこの開始点の寿命を越えている場合(すなわち StartTime+ReleaseTime < CurrentTime の時)Trueを返す 
    bool CheckReleasetime(const double &CurrentTime);

    //! @brief 2点間を指定された数で等分した位置の座標を返す
    //! @param Coords    [out] 座標をx1,x2,......xnの順にn個収めた配列
    //! @param NumPoints [in]  分割数
    //! @param Coord1    [in]  端点の座標その1
    //! @param Coord2    [in]  端点の座標その2

    void DividePoints(std::vector < DSlib::DV3 > *Coords, const int &NumPoints, DSlib::DV3 & Coord1, DSlib::DV3 & Coord2);

    //! @brief LatestEmitTimeからTimeSpan時間経過していた場合に、新しく粒子を放出する
    void EmitNewParticle(std::list < ParticleData * >* ParticleList, const double &CurrentTime, const unsigned int &CurrentTimeStep);

    //! テキスト出力を行う
    virtual std::ostream & TextPrint(std::ostream & stream) const =0;

    //! @brief 開始点を移動させる
    //! 基底クラスでは何もしない。
    //! 現時点ではMobingPointsのみがオーバーライドしている
    virtual void UpdateStartPoint(double CurrentTime)
    {
    };

    //! @brief 開始点オブジェクトをMaxNumStartPointsで指定した開始点数以下のオブジェクトに分割する。
    //! 余りが生じた場合は1つ余計にオブジェクトを生成し、そのオブジェクトに余り領域を入れて返す
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param MaxNumStartPoints [in]  分割後のオブジェクトが持つ最大の開始点数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    virtual void Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints)=0;

    //! @brief 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    virtual void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)=0;


    //! Setter/Getter
    void SetSumStartPoints(const int &SumStartPoints)
    {
      this->SumStartPoints = SumStartPoints;
    };
    void SetStartTime(const double &StartTime)
    {
        this->StartTime = StartTime;
    };
    void SetReleaseTime(const double &ReleaseTime)
    {
        this->ReleaseTime = ReleaseTime;
    };
    void SetTimeSpan(const double &TimeSpan)
    {
        this->TimeSpan = TimeSpan;
    };
    void SetParticleLifeTime(const double &ParticleLifeTime)
    {
        this->ParticleLifeTime = ParticleLifeTime;
    };
    void SetID(int *ID)
    {
      for(int i = 0; i < 2; i++)
      {
        this->ID[i] = ID[i];
      }
    };

    // TODO この辺使ってないはず・・・
    void GetID(int *ID)
    {
      for(int i = 0; i < 2; i++) {
        ID[i] = this->ID[i];
    }};

    std::string GetID(void)
    {
      std::ostringstream oss;
      oss << this->ID[0] << "," << this->ID[1];
      return (oss.str());
    };

    virtual void GetNumStartPoints(int *NumStartPoints)
    {
    };
    virtual int GetSumStartPoints()
    {
      return this->SumStartPoints;
    };
    double GetStartTime()
    {
      return this->StartTime;
    };
    double GetLatestEmitTime()
    {
      return this->LatestEmitTime;
    };  //ReadOnly

    double GetReleaseTime()
    {
      return this->ReleaseTime;
    };
    double GetLifeTime()
    {
      return GetReleaseTime();
    };  //GetReleaseTimeに対する別名。粒子の寿命判定と共用するために用意している。
    double GetTimeSpan()
    {
      return this->TimeSpan;
    };
    double GetParticleLifeTime()
    {
      return this->ParticleLifeTime;
    };

    //! Destructor
    virtual ~ StartPoint() {
    }

    //!  Constructor
      StartPoint()
    {
      SumStartPoints = -1;
      StartTime = 0.0;
      ReleaseTime = 0.0;
      TimeSpan = 0.0;
      LatestEmitTime = -0.1;
      ParticleLifeTime = -0.1;
      ID[0] = -1;
      ID[1] = -2;
    }

    //! Copy Constructor
    StartPoint(const StartPoint & org)
    {
      SumStartPoints = org.SumStartPoints;
      StartTime = org.StartTime;
      ReleaseTime = org.ReleaseTime;
      TimeSpan = org.TimeSpan;
      LatestEmitTime = org.LatestEmitTime;
      ParticleLifeTime = org.ParticleLifeTime;
      ID[0] = org.ID[0];
      ID[1] = org.ID[1];
    }

    //! 代入演算子オーバーロード
    StartPoint & operator=(const StartPoint & org)
    {
      SumStartPoints = org.SumStartPoints;
      StartTime = org.StartTime;
      ReleaseTime = org.ReleaseTime;
      TimeSpan = org.TimeSpan;
      LatestEmitTime = org.LatestEmitTime;
      ParticleLifeTime = org.ParticleLifeTime;
      ID[0] = org.ID[0];
      ID[1] = org.ID[1];
      return *this;
    }

    //!  比較演算子オーバーロード (sortのため)
    bool operator<(const StartPoint & obj)
    {
      return SumStartPoints < obj.SumStartPoints;
    }
    bool operator>(const StartPoint & obj)
    {
      return SumStartPoints > obj.SumStartPoints;
    }
    static bool isGreater(StartPoint * obj1, StartPoint * obj2)
    {
      return obj1->SumStartPoints > obj2->SumStartPoints;
    }

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, StartPoint* obj);

  };

} // namespace PPlib
#endif
