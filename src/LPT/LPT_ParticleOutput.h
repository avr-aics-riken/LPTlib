/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef LPT_PARTICLE_OUTPUT_H
#define LPT_PARTICLE_OUTPUT_H
#include <list>
#include <string>
#include "FileManager.h"

//ParticleData.hは粒子データフォーマットのバージョン番号
//(マクロ変数 PARTICLE_DATA_FORMAT_VERSION)を取得するためにincludeしている
#include "ParticleData.h"
#include "ParticleContainer.h"

namespace LPT
{
  //! @brief LPT native binary形式で粒子データを出力する
  class LPT_ParticleOutput
  {
    /// 出力先のファイルストリームへのポインタ
    std::ofstream Out;

    /// 出力する粒子データのリストへのポインタ
    std::list<PPlib::ParticleData *> Particles;

    /// Byte Order Mark (big/little Endian判別用のデータ) を格納する配列
    int BOM[4];

    ///  出力するParticleDataのバージョンを格納する変数
    double ParticleDataFormatVersion;

    /// REAL_TYPEのサイズを格納する変数
    size_t SizeOfRealType;

  private:
    //Singletonパターンを適用
    LPT_ParticleOutput()
    {
    };
    ~LPT_ParticleOutput()
    {
    };
    LPT_ParticleOutput(const LPT_ParticleOutput& obj);
    LPT_ParticleOutput & operator=(const LPT_ParticleOutput& obj);
  public:
    static LPT_ParticleOutput *GetInstance()
    {
      static LPT_ParticleOutput instance;
      static bool initialized = false;
      if(!initialized)
      {
        initialized = true;
        instance.Out.open((FileManager::GetInstance()->GetFileName("prt")).c_str(), std::ios::binary);
        instance.BOM[0] = 0xff;
        instance.BOM[1] = 0xef;
        instance.BOM[2] = 0xff;
        instance.BOM[3] = 0xef;
        instance.ParticleDataFormatVersion = PARTICLE_DATA_FORMAT_VERSION;
        instance.SizeOfRealType = sizeof(REAL_TYPE);
      }
      return &instance;
    }

    /// 出力する粒子データを受け取る
    void SetParticles(std::list<PPlib::ParticleData*>* arg_Particles) 
    {
      Particles.swap(*arg_Particles);
    }

    /// 出力する粒子データを受け取る
    void SetParticles(PPlib::ParticleContainer* arg_Particles) 
    {
      Particles.clear();
      for (PPlib::ParticleContainerIterator it=arg_Particles->begin(); it !=arg_Particles->end();++it)
      {
        Particles.push_back(*it);
      }
    };
    void WriteFileHeader();
    void WriteRecordHeader();
    void WriteRecord();
  };

} //namespace LPT
#endif
