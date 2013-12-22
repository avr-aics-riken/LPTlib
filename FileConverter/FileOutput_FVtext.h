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
      Out.open((FileManager::GetInstance()->GetFileName("fvp")).c_str());
    }
    ~FV_ParticlePath()
    {
    }

    void WriteFileHeader()
    {
      Out << "FVPARTICLES 2 1" << std::endl;
      Out << "Tag Names" << std::endl;
      Out << "0" << std::endl;
      Out << "Variable Names" << std::endl;
      Out << "1" << std::endl;
      Out << "Time" << std::endl;
    }

    void WriteRecordHeader()
    {
      Out << Particles->size() << std::endl;
    }

    void WriteRecord()
    {
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        Out << (*it)->x << " " << (*it)->y << " " << (*it)->z << " " << (*it)->CurrentTime << std::endl;
      }
    }
  };

} //namespace LPT
#endif
