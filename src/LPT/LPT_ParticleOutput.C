#include "LPT_ParticleOutput.h"

namespace LPT
{

  void LPT_ParticleOutput::WriteFileHeader()
  {
    Out.write((char *)BOM, sizeof(BOM));
    Out.write((char *)&ParticleDataFormatVersion, sizeof(double));
    Out.write((char *)&SizeOfRealType, sizeof(size_t));
  }

  void LPT_ParticleOutput::WriteRecordHeader()
  {
    unsigned long ParticleSize = Particles.size();

    if(ParticleSize > 0) {
      Out.write((char *)&ParticleSize, sizeof(unsigned long));
    }
  }

  void LPT_ParticleOutput::WriteRecord()
  {
    for(std::list<PPlib::ParticleData*>::iterator it = Particles.begin(); it != Particles.end(); ++it) {
      (*it)->BinaryWrite(Out);
    }
    Particles.clear();
  }

} //namespace LPT
