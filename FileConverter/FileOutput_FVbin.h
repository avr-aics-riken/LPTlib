/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef FV_PARTICLE_PATH_BINARY_OUTPUT_H
#define FV_PARTICLE_PATH_BINARY_OUTPUT_H
#include "FileManager.h"
#include "FileOutput.h"

namespace LPT
{
  //! @brief FieldView13 ParticlePath形式(Binary)で粒子データを出力するクラス
  class FV_ParticlePathBinary:public FileOutput
  {
  private:
    bool Skip1stStep;
    //Singletonパターンを適用
    FV_ParticlePathBinary():Skip1stStep(false)
    {
    }
    ~FV_ParticlePathBinary()
    {
    }
    FV_ParticlePathBinary(const FileOutput & obj);
    FV_ParticlePathBinary & operator=(const FileOutput & obj);
  public:
    static FV_ParticlePathBinary *GetInstance()
    {
      static FV_ParticlePathBinary instance;
      static bool initialized = false;
      if(!initialized)
      {
        initialized = true;
        instance.Out.open((FileManager::GetInstance()->GetFileName("fvp")).c_str(), std::ios::binary);
        instance.Particles = NULL;
      }
      return &instance;
    }

  public:
    void WriteFileHeader()
    {
      //Prepare file header
      std::vector < std::string > VariableNames;
      VariableNames.push_back("U-Velocity");
      VariableNames.push_back("V-Velocity");
      VariableNames.push_back("W-Velocity");
      VariableNames.push_back("Rank");  //その粒子を放出した開始点を持っていたプロセスのRank番号
      VariableNames.push_back("StartPoint_id");
      VariableNames.push_back("Particle_id");

      //Write file header
      WriteVariable(0x0010203); //FV_MAGIC
      {
        char write_buffer[80]="FVPARTICLES";
        Out.write((char *)write_buffer, sizeof(char) * 80);
      }
      WriteVariable(1);  //Major version
      WriteVariable(1);  //Minor version
      WriteVariable((int)VariableNames.size());
      for(std::vector < std::string >::iterator it = VariableNames.begin(); it != VariableNames.end(); ++it)
      {
        char write_buffer[80];
        strncpy(write_buffer, (*it).c_str(), 80);
        Out.write((char *)write_buffer, sizeof(char) * 80);
      }
    }
    void WriteRecordHeader()
    {
    /* 
     * FV13.2のParticlePathファイルの仕様上タイムステップは必ず1から始まる必要がある
     * そこで第0ステップのデータが来たら第1ステップとして扱い
     * さらに第1ステップのデータが来たら出力をスキップする処理を追加している
     */
      static bool Output0thStep = false;

      int TimeStep = (*Particles->begin())->CurrentTimeStep;
      if(TimeStep == 0) {
        Output0thStep = true;
        ++TimeStep;
      } else if(TimeStep == 1 && Output0thStep == true) {
        Skip1stStep = true;
        return;
      } else if(TimeStep > 1 && Skip1stStep == true) {
        Skip1stStep = false;
      }

      WriteVariable((int)TimeStep);
      WriteVariable((float)(*Particles->begin())->CurrentTime);
      WriteVariable((int)Particles->size());
    }
    void WriteRecord()
    {
      if(Skip1stStep)
      {
        return;
      }
      int ID = 1;
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((int)ID++);
        WriteVariable((float)(*it)->x);
        WriteVariable((float)(*it)->y);
        WriteVariable((float)(*it)->z);
        WriteVariable((float)(*it)->Vx);
        WriteVariable((float)(*it)->Vy);
        WriteVariable((float)(*it)->Vz);
        WriteVariable((float)(*it)->StartPointID1);
        WriteVariable((float)(*it)->StartPointID2);
        WriteVariable((float)(*it)->ParticleID);
      }
    }
  private:
    template <typename T>
    void WriteVariable(const T& var)
    {
      Out.write((char *)&var, sizeof(var));
    }

  };

} //namespace LPT
#endif
