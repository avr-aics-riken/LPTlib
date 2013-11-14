#include "FileOutput_FVtext.h"

namespace LPT
{
  void FV_ParticlePath::WriteFileHeader()
  {
    Out2 << "FVPARTICLES 2 1" << std::endl;
    Out2 << "Tag Names" << std::endl;
    Out2 << "0" << std::endl;
    Out2 << "Variable Names" << std::endl;
    Out2 << "1" << std::endl;
    Out2 << "Time" << std::endl;
  }

  void FV_ParticlePath::WriteRecordHeader()
  {
    Out2 << Particles->size() << std::endl;
  }

  void FV_ParticlePath::WriteRecord()
  {
    for(std::list < PPlib::ParticleData * >::iterator it = Particles->begin(); it != Particles->end(); ++it) {
      Out2 << (*it)->Coord[0] << " " << (*it)->Coord[1] << " " << (*it)->Coord[2] << " " << (*it)->CurrentTime << std::endl;
    }
  }

} //namespace LPT
