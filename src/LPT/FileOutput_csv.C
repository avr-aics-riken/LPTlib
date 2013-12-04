#include "FileOutput_csv.h"

namespace LPT
{

  void CSV_Output::WriteFileHeader()
  {
    Out2 << "x,y,z,CurrentTime,CurrentTimeStep,U-Velocity,V-Velocity,W-Velocity,Rank,StartPointID,ParticleID" << std::endl;
  }

  void CSV_Output::WriteRecord()
  {
    for(std::multimap<long, PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
      Out2 << (*it).second->Coord[0] << ",";
      Out2 << (*it).second->Coord[1] << ",";
      Out2 << (*it).second->Coord[2] << ",";
      Out2 << (*it).second->CurrentTime << ",";
      Out2 << (*it).second->CurrentTimeStep << ",";
      Out2 << (*it).second->ParticleVelocity[0] << ",";
      Out2 << (*it).second->ParticleVelocity[1] << ",";
      Out2 << (*it).second->ParticleVelocity[2] << ",";
      Out2 << (*it).second->StartPointID[0] << ",";
      Out2 << (*it).second->StartPointID[1] << ",";
      Out2 << (*it).second->ParticleID << ",";
      Out2 << std::endl;
    }
  }

} //namespace LPT
