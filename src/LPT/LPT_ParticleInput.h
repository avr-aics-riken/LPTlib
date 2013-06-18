#ifndef LPT_PARTICLE_INPUT_H
#define LPT_PARTICLE_INPUT_H
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include "FileManager.h"
//ParticleData.hは粒子データフォーマットのバージョン番号を取得するためにincludeしている
#include "ParticleData.h"

namespace LPT
{
  //! @brief ParticleDataを特定する3つのIDメンバ変数をまとめて保持する構造体
  //! StartPointID[0], StartPointID[1], ParticleID をメンバとして保持する
  //! 比較演算子をオーバーロードして、STLのアルゴリズムを使うことが目的
  struct ID
  {
    int StartPointID_0;
    int StartPointID_1;
    int ParticleID;

    bool operator ==(const ID & obj) const
    {
      return (this->StartPointID_0 == obj.StartPointID_0 && this->StartPointID_1 == obj.StartPointID_1 && this->ParticleID == obj.ParticleID);
    }

    bool operator <(const ID & obj)const
    {
      return (this->ParticleID < obj.ParticleID ? true : this->StartPointID_1 < obj.StartPointID_1 ? true : this->StartPointID_0 < obj.StartPointID_0 ? true : false);
    }
  };

  //! @brief LPT native binary形式の粒子データを読み取る
  class LPT_ParticleInput
  {
    /// 入力ストリームへのポインタ
    std::ifstream * In;

    /// Byte Order Mark
    int BOM[4];

    /// ParticleData のフォーマットのバージョン番号
    double ParticleDataFormatVersion;

    /// REAL_TYPEのサイズを格納する変数
    size_t SizeOfRealType;

    /// 読み込んだ粒子データを格納するリストへのポインタ
      std::list < PPlib::ParticleData * >*Particles;

    /// レコード長
    unsigned long RecordSize;

  public:
      LPT_ParticleInput(std::ifstream * argIn)
    {
      In = argIn;
      Particles = NULL;
      BOM[0] = 0xff;
      BOM[1] = 0xef;
      BOM[2] = 0xff;
      BOM[3] = 0xef;
      ParticleDataFormatVersion = PARTICLE_DATA_FORMAT_VERSION;
      SizeOfRealType = sizeof(REAL_TYPE);
    }

    //! @brief ファイルのヘッダ部に書かれたBOMを読み込んで実行中の処理系とendianが同じかどうか確認する
    //! @attention endianが異なる場合はAbort()
    void DetermineEndian();

    //! @brief ファイルのヘッダ部のBOM以外の部分を読んでデータフォーマットや、REAL_TYPEのサイズに不整合が無いか確認する
    //! @attention 異常があった場合はAbort()
    void ReadFileHeader();

    void ReadRecordHeader();

    //! ファイル内の粒子データを読み込む
    void ReadRecord();

    //! ファイル内の指定されたIDの粒子データを読み込む
    //! @param [in] ID 読み込み対象の粒子ID
    void ReadRecord(const ID & ParticleID);

    //! ファイル内の指定されたTimeStepの粒子データを読み込む
    //! @param [in] TimeStep 読み込み対象の粒子TimeStep
    void ReadRecord(const int &TimeStep);

    //! ファイル内に書かれている粒子データのIDを引数で渡されたsetに追記する 
    //! @param IDs [inout] 粒子データのIDを格納したコンテナへのポインタ 
    void ReadIDs(std::set < ID > *IDs);

    //! ファイル内に書かれている粒子データのTimeStepを引数で渡されたsetに追記する
    //!  @param TimeSteps [inout] 粒子データのTimeStepを格納したコンテナへのポインタ
    void ReadTimeSteps(std::set < unsigned int >*TimeSteps);

    void SetParticles(std::list < PPlib::ParticleData * >*Particles)
    {
      this->Particles = Particles;
    };
  };

} //namespace LPT
#endif
