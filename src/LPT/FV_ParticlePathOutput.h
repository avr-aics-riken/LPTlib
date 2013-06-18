#ifndef FV_PARTICLE_PATH_OUTPUT_H
#define FV_PARTICLE_PATH_OUTPUT_H
#include <list>
#include "FileManager.h"
#include "FileOutput.h"

namespace LPT
{
  //!  @brief FieldView13 ParticlePath形式(text)で粒子データを出力するクラス
  class FV_ParticlePath:public FileOutput
  {
  public:
    FV_ParticlePath()
    {
      Out2.open((FileManager::GetInstance()->GetFileName("fvp")).c_str());
    }
    void WriteFileHeader();
    void WriteRecordHeader();
    void WriteRecord();

  };

} //namespace LPT
#endif
