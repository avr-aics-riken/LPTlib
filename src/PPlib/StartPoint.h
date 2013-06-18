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

//TODO SimpleFactory化
//     StartPointはインターフェースクラスとし、Point型の開始点は
//     StartPointを継承したPointクラスに変更する

  //! @brief 開始点の情報を保持するクラス
  //! 単純な開始点座標1点のみを指定する場合はこのクラスを使用し
  //! 移動する場合やある領域内の複数の開始点を指定する場合は、以下の派生クラスを使用する
  //! MovingPoint, Line, Rectangel, Cuboid, Circle
  //! 全Rankが一旦インスタンスを生成し、データ分散の処理をした後で自Rankが担当するインスタンス以外を破棄する
  class StartPoint
  {
  protected:
        /**
	 * TODO 派生クラスへ移動
	 * 開始点を定義する座標
	 * 配列の先頭から順にx,y,zの各成分を意味する
	 * 開始点の種類によって指定する座標の意味が変わる
	 *   Point       : 開始点座標
	 *   MovingPoints: 現在時刻で有効な開始の座標
	 *   Line        : 開始点が存在する線分の端点
	 *   Rectangle   : 開始点が存在する矩形領域の頂点
	 *   Cuboid      : 開始点が存在する直方体の頂点
	 *   Circle      : 開始点が存在する円の中心
	 * MovingPointsの時はインスタンス生成時に値が設定されていても上書きされる
	 */
    REAL_TYPE Coord1[3];

    //! @brief 1つの設定で定義される開始点の数
    //! インスタンス生成時に設定され、変更はしない
    //! Point, MovingPointsの場合は必ず1
    //! Rectangle, Cuboidの場合は派生クラスのメンバのNumStartPoints[3]で一辺あたりの開始点数を指定するので
    //! そこから自動で計算する
    //! LineおよびCircleではユーザが指定した値を用いる
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
    //!  Constructor
      StartPoint()
    {
      Coord1[0] = -1.0;
      Coord1[1] = -1.0;
      Coord1[2] = -1.0;
      SumStartPoints = -1;
      StartTime = 0.0;
      ReleaseTime = 0.0;
      TimeSpan = 0.0;
      LatestEmitTime = -0.1;
      ID[0] = -1;
      ID[1] = -2;
      ParticleLifeTime = -0.1;
    }

    //! Copy Constructor
    StartPoint(const StartPoint & org)
    {
      for(int i = 0; i < 3; i++)
      {
        Coord1[i] = org.Coord1[i];
      }
      SumStartPoints = org.SumStartPoints;

      StartTime = org.StartTime;
      ReleaseTime = org.ReleaseTime;
      TimeSpan = org.TimeSpan;
      LatestEmitTime = org.LatestEmitTime;
      ID[0] = org.ID[0];
      ID[1] = org.ID[1];
      ParticleLifeTime = org.ParticleLifeTime;
    }

    //! 代入演算子オーバーロード
    StartPoint & operator=(const StartPoint & org)
    {
      for(int i = 0; i < 3; i++) {
        Coord1[i] = org.Coord1[i];
      }
      SumStartPoints = org.SumStartPoints;
      StartTime = org.StartTime;
      ReleaseTime = org.ReleaseTime;
      TimeSpan = org.TimeSpan;
      LatestEmitTime = org.LatestEmitTime;
      ID[0] = org.ID[0];
      ID[1] = org.ID[1];
      ParticleLifeTime = org.ParticleLifeTime;
      return *this;
    }

    virtual ~ StartPoint() {
    }

    //! @brief LatestEmitTimeからTimeSpan時間経過していた場合に、新しく粒子を放出する
    std::list < ParticleData * >*EmitNewParticle(const double &CurrentTime, const unsigned int &CurrentTimeStep);

    //! CurrentTimeがこの開始点の寿命を越えている場合(すなわち StartTime+ReleaseTime < CurrentTime の時)Trueを返す 
    bool CheckReleasetime(const double &CurrentTime);

    //! @brief 開始点を移動させる
    //! 基底クラスでは何もしない。
    //! 現時点ではMobingPointsのみがオーバーライドしている
    virtual void UpdateStartPoint(double CurrentTime)
    {
    };

    //! @brief 開始点設定をNumParts個の小領域に分割する。
    //! 余りが生じた場合は1つ余計にオブジェクトを生成し、そのオブジェクトに余り領域を入れて返す
    //! @attention 分割前のオブジェクトは残っているので、このメソッド実行後に破棄すること
    //! @param NumParts    [in]  余り領域を除いた分割後のオブジェクト数
    //! @ret   分割後の開始点オブジェクトを格納したコンテナ
    //! TODO  純粋仮想関数に変更
    virtual std::vector < StartPoint * >*Divider(const int &NumParts);

    //! @brief 格子点(粒子の発生位置)の座標を引数で指定したvectorに格納する
    //! @param Coords [out] 格子点座標
    //! TODO  純粋仮想関数に変更
    virtual void GetGridPointCoord(std::vector < DSlib::DV3 > &Coords);

    //! @brief 2点間を指定された数で等分した位置の座標を返す
    //! @param Coords    [out] 座標をx1,x2,......xnの順にn個収めた配列
    //! @param NumPoints [in]  分割数
    //! @param Coord1    [in]  端点の座標その1
    //! @param Coord2    [in]  端点の座標その2
    void DividePoints(std::vector < DSlib::DV3 > *Coords, const int &NumPoints, DSlib::DV3 & Coord1, DSlib::DV3 & Coord2);

    //! Setter/Getter
    // TODO 不要なSetter/Getterを削除
    void SetID(int *ID)
    {
      for(int i = 0; i < 2; i++) {
        this->ID[i] = ID[i];
    }};
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

    // TODO 派生クラスへ移動
    void SetCoord1(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        this->Coord1[i] = Coord[i];
    }};
    void GetCoord1(REAL_TYPE * Coord)
    {
      for(int i = 0; i < 3; i++) {
        Coord[i] = this->Coord1[i];
    }};

    // TODO 削除というかFactoryへ移動
    virtual void SetCoord2(REAL_TYPE * Coord)
    {
    };
    virtual void GetCoord2(REAL_TYPE * Coord)
    {
    };
    virtual void SetNumStartPoints(int *NumStartPoints)
    {
    };
    virtual void GetNumStartPoints(int *NumStartPoints)
    {
    };

    void SetSumStartPoints(const int &SumStartPoints)
    {
      this->SumStartPoints = SumStartPoints;
    };
    virtual int GetSumStartPoints()
    {
      return this->SumStartPoints;
    };

    void SetStartTime(const double &StartTime)
    {
      this->StartTime = StartTime;
    };
    double GetStartTime()
    {
      return this->StartTime;
    };

    double GetLatestEmitTime()
    {
      return this->LatestEmitTime;
    };  //ReadOnly

    void SetReleaseTime(const double &ReleaseTime)
    {
      this->ReleaseTime = ReleaseTime;
    };
    double GetReleaseTime()
    {
      return this->ReleaseTime;
    };
    double GetLifeTime()
    {
      return GetReleaseTime();
    };  //GetReleaseTimeに対する別名。粒子の寿命判定と共用するために用意している。

    void SetTimeSpan(const double &TimeSpan)
    {
      this->TimeSpan = TimeSpan;
    };
    double GetTimeSpan()
    {
      return this->TimeSpan;
    };

    void SetParticleLifeTime(const double &ParticleLifeTime)
    {
      this->ParticleLifeTime = ParticleLifeTime;
    };
    double GetParticleLifeTime()
    {
      return this->ParticleLifeTime;
    };

    //!  出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, StartPoint * obj);
    friend std::ostream & operator <<(std::ostream & stream, StartPoint & obj);
    friend std::istream & operator >>(std::istream & stream, StartPoint & obj);

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

  };

} // namespace PPlib
#endif
