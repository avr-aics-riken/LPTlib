#include <iostream>
#include <iomanip>
#include <fstream>
#include "ParticleData.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, ParticleData & obj)
  {
    stream << "StartPointID                     = " << obj.StartPointID[0] << "," << obj.StartPointID[1] << std::endl;
    stream << "ParticleID                       = " << obj.ParticleID << std::endl;
    stream << "BlockID                          = " << obj.BlockID << std::endl;
    stream << "Coord                            = " << obj.Coord[0] << "," << obj.Coord[1] << "," << obj.Coord[2] << std::endl;
    stream << "ParticleVelocity                 = " << obj.ParticleVelocity[0] << "," << obj.ParticleVelocity[1] << "," << obj.ParticleVelocity[2] << std::endl;
    stream << "StartTime, LifeTime, CurrentTime = " << obj.StartTime << "," << obj.LifeTime << "," << obj.CurrentTime << std::endl;
    return stream;
  }

  void ParticleData::BinaryWrite(std::ofstream & out)
  {
    out.write((char *)this->StartPointID, sizeof(int) * 2);
    out.write((char *)&(this->ParticleID), sizeof(int) * 1);
    out.write((char *)&(this->BlockID), sizeof(long) * 1);
    out.write((char *)this->Coord, sizeof(REAL_TYPE) * 3);
    out.write((char *)this->ParticleVelocity, sizeof(REAL_TYPE) * 3);
    out.write((char *)&(this->StartTime), sizeof(double) * 1);
    out.write((char *)&(this->LifeTime), sizeof(double) * 1);
    out.write((char *)&(this->CurrentTime), sizeof(double) * 1);
    out.write((char *)&(this->CurrentTimeStep), sizeof(unsigned int) * 1);
  }

  void ParticleData::BinaryRead(std::ifstream & in)
  {
    in.read((char *)this->StartPointID, sizeof(int) * 2);
    in.read((char *)&(this->ParticleID), sizeof(int) * 1);
    in.read((char *)&(this->BlockID), sizeof(long) * 1);
    in.read((char *)this->Coord, sizeof(REAL_TYPE) * 3);
    in.read((char *)this->ParticleVelocity, sizeof(REAL_TYPE) * 3);
    in.read((char *)&(this->StartTime), sizeof(double) * 1);
    in.read((char *)&(this->LifeTime), sizeof(double) * 1);
    in.read((char *)&(this->CurrentTime), sizeof(double) * 1);
    in.read((char *)&(this->CurrentTimeStep), sizeof(unsigned int) * 1);
  }

} // namespace PPlib
