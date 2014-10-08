/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

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
      RecordSize=0;
    }
    ~LPT_ParticleInput()
    {
    }

    //! @brief ファイルのヘッダ部に書かれたBOMを読み込んで実行中の処理系とendianが同じかどうか確認する
    //! @attention endianが異なる場合はAbort()
    void DetermineEndian();

    //! @brief ファイルのヘッダ部のBOM以外の部分を読んでデータフォーマットや、REAL_TYPEのサイズに不整合が無いか確認する
    //! @attention 異常があった場合はAbort()
    void ReadFileHeader();

    //! @brief レコードのヘッダ部を読み込んで、レコード長(=1レコード内の粒子データ数）を返す
    //! レコード長はメンバ変数のRecordSizeにも保存する
    unsigned long ReadRecordHeader();

    //! ファイル内の粒子データを読み込む
    void ReadRecord();

    //! ファイル内の指定されたTimeStepの粒子データを読み込む
    //! @param [in] TimeStep 読み込み対象の粒子TimeStep
    void ReadRecord(const int &TimeStep);

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
