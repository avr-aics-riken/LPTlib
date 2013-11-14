#include "FileOutput_csv.h"

namespace LPT
{

  void CSV_Output::WriteFileHeader()
  {
    Out2 << "x,y,z,CurrentTime,CurrentTimeStep,U-Velocity,V-Velocity,W-Velocity,Rank,StartPointID,ParticleID" << std::endl;
  }

  void CSV_Output::WriteRecord()
  {
    for(std::list < PPlib::ParticleData * >::iterator it = Particles->begin(); it != Particles->end(); ++it) {
      Out2 << (*it)->Coord[0] << ",";
      Out2 << (*it)->Coord[1] << ",";
      Out2 << (*it)->Coord[2] << ",";
      Out2 << (*it)->CurrentTime << ",";
      Out2 << (*it)->CurrentTimeStep << ",";
      Out2 << (*it)->ParticleVelocity[0] << ",";
      Out2 << (*it)->ParticleVelocity[1] << ",";
      Out2 << (*it)->ParticleVelocity[2] << ",";
      Out2 << (*it)->StartPointID[0] << ",";
      Out2 << (*it)->StartPointID[1] << ",";
      Out2 << (*it)->ParticleID << ",";
      Out2 << std::endl;
    }
  }

} //namespace LPT
