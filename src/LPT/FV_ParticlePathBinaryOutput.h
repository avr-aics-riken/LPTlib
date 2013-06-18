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
        instance.Out2.open((FileManager::GetInstance()->GetFileName("fvp")).c_str(), std::ios::binary);
        instance.Particles = NULL;
      }
      return &instance;
    }

  public:

    void WriteFileHeader();
    void WriteRecordHeader();
    void WriteRecord();

  };

} //namespace LPT
#endif
