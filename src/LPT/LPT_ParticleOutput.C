#include "LPT_ParticleOutput.h"

namespace LPT
{

  void LPT_ParticleOutput::WriteFileHeader()
  {
    Out2.write((char *)BOM, sizeof(BOM));
    Out2.write((char *)&ParticleDataFormatVersion, sizeof(double));
      Out2.write((char *)&SizeOfRealType, sizeof(size_t));
  }

  void LPT_ParticleOutput::WriteRecordHeader()
  {
    unsigned long ParticleSize = Particles->size();

    if(ParticleSize > 0) {
      Out2.write((char *)&ParticleSize, sizeof(unsigned long));
    }
  }

  void LPT_ParticleOutput::WriteRecord()
  {
    for(std::list < PPlib::ParticleData * >::iterator it = Particles->begin(); it != Particles->end(); ++it) {
      (*it)->BinaryWrite(Out2);
    }
  }

} //namespace LPT
