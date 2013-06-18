#ifndef PPLIB_PARTICLE_DATA_H
#define PPLIB_PARTICLE_DATA_H

///@attention PARTICLE_DATA_FORMAT_VERSIONは、double型の変数
//            したがって、1.0.0のような形式のバージョン番号は使用できない
#define PARTICLE_DATA_FORMAT_VERSION 2.0

#include <iostream>
#include <sstream>
#include <string>

namespace PPlib
{
  //! @brief 粒子データを保持するための構造体
  //! インスタンスを生成した後はPPlib::Particlesにポインタを登録し、そちらで管理する。
  //! インスタンスを破棄する時は、そちらのエントリも必ず削除すること
  struct ParticleData
  {
  public:
    //! @brief この粒子を放出した開始点のID
    //! 1要素目はStartPoint::Rank
    //! 2要素目はStartPoint::ID
    int StartPointID[2];

    //! @brief 粒子のID
    //! 開始点から放出された順番を示す
    //! 同一の開始点から放出された粒子の間では一意になる
    int ParticleID;

    //! 粒子座標
    REAL_TYPE Coord[3];

    //! 粒子速度
    REAL_TYPE ParticleVelocity[3];

    //! 粒子が放出された時刻
    double StartTime;

    //! 粒子の寿命(放出された時刻からLiftime時間経過後に消滅する
    double LifeTime;

    //! 粒子が存在する位置のブロックID
    long BlockID;

    //! どの時点の粒子データかを格納する
    double CurrentTime;

    //! どの時点の粒子データかを格納する(タイムステップ)
    unsigned int CurrentTimeStep;

    //!  引数で指定されたファイルストリームに対して、粒子データを出力
    void BinaryWrite(std::ofstream & out);

    //!  引数で指定されたファイルストリームにから粒子データを読み込む
    void BinaryRead(std::ifstream & in);

    //! @briefGetter
    //!  publicなメンバなので本来不要だが、LifeTime classへのインターフェースとして用意している
    double GetStartTime()
    {
      return this->StartTime;
    };
    double GetLifeTime()
    {
      return this->LifeTime;
    };
    double GetID()
    {
      return this->ParticleID;
    };

    //! @brief StartPointID[0], [1], ParticleIDをカンマで結合した文字列を返す
    //! このIDは全体を通して一意になるが値として扱えないので注意
    std::string GetAllID(void) {
      std::ostringstream oss;
      oss << this->StartPointID[0] << "," << this->StartPointID[1] << "," << GetID();
      return (oss.str());
    };

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, ParticleData & obj);

    //! 比較演算子のオーバーロード
    bool operator==(const ParticleData & obj)
    {
      return (this->ParticleID == obj.ParticleID &&
              this->StartPointID[0] == obj.StartPointID[0] &&
              this->StartPointID[1] == obj.StartPointID[1] && this->Coord[0] == obj.Coord[0] && this->Coord[1] == obj.Coord[1] && this->Coord[2] == obj.Coord[2] && this->StartTime == obj.StartTime) ? true : false;
    }

  };

//!BlockID順にソートするためのファンクタ
  struct CompareBlockID
  {
    bool operator() (const ParticleData * left, const ParticleData * right)const
    {
      return (left->BlockID < right->BlockID) ? true : false;
    }
  };

//!タイムステップ順にソートするためのファンクタ
  struct CompareTimeStep
  {
    bool operator() (const ParticleData * left, const ParticleData * right)const
    {
      return (left->CurrentTimeStep < right->CurrentTimeStep) ? true : false;
    }
  };

//!粒子のID順にソートするためのファンクタ
  struct CompareID
  {
    bool operator() (const ParticleData * left, const ParticleData * right)const
    {
      bool retval;
      if((left->StartPointID)[0] == (right->StartPointID[0])) {
        if((left->StartPointID)[1] == (right->StartPointID[1])) {
          retval = left->ParticleID < right->ParticleID ? true : false;
        } else
        {
          retval = (left->StartPointID)[1] < (right->StartPointID[1]) ? true : false;
        }
      } else
      {
        retval = (left->StartPointID)[0] < (right->StartPointID[0]) ? true : false;
      }
      return retval;
    }
  };

} // namespace PPlib
#endif
