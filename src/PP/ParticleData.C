#include <iostream>
#include <iomanip>
#include <fstream>
#include "ParticleData.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, ParticleData & obj)
  {
    stream << "StartPointID                     = " << obj.StartPointID1 << "," << obj.StartPointID2 << std::endl;
    stream << "ParticleID                       = " << obj.ParticleID << std::endl;
    stream << "BlockID                          = " << obj.BlockID << std::endl;
    stream << "Coord                            = " << obj.x << "," << obj.y << "," << obj.z << std::endl;
    stream << "ParticleVelocity                 = " << obj.Vx << "," << obj.Vy << "," << obj.Vz << std::endl;
    stream << "StartTime, LifeTime, CurrentTime = " << obj.StartTime << "," << obj.LifeTime << "," << obj.CurrentTime << std::endl;
    return stream;
  }

  void ParticleData::BinaryWrite(std::ofstream & out)
  {
    out.write((char *)&(this->StartPointID1), sizeof(int));
    out.write((char *)&(this->StartPointID2), sizeof(int));
    out.write((char *)&(this->ParticleID), sizeof(int));
    out.write((char *)&(this->BlockID), sizeof(long));
    out.write((char *)&(this->x), sizeof(REAL_TYPE));
    out.write((char *)&(this->y), sizeof(REAL_TYPE));
    out.write((char *)&(this->z), sizeof(REAL_TYPE));
    out.write((char *)&(this->Vx), sizeof(REAL_TYPE));
    out.write((char *)&(this->Vy), sizeof(REAL_TYPE));
    out.write((char *)&(this->Vz), sizeof(REAL_TYPE));
    out.write((char *)&(this->StartTime), sizeof(double));
    out.write((char *)&(this->LifeTime), sizeof(double));
    out.write((char *)&(this->CurrentTime), sizeof(double));
    out.write((char *)&(this->CurrentTimeStep), sizeof(unsigned int));
  }

  void ParticleData::BinaryRead(std::ifstream & in)
  {
    in.read((char *)&(this->StartPointID1), sizeof(int));
    in.read((char *)&(this->StartPointID2), sizeof(int));
    in.read((char *)&(this->ParticleID), sizeof(int));
    in.read((char *)&(this->BlockID), sizeof(long));
    in.read((char *)&(this->x), sizeof(REAL_TYPE));
    in.read((char *)&(this->y), sizeof(REAL_TYPE));
    in.read((char *)&(this->z), sizeof(REAL_TYPE));
    in.read((char *)&(this->Vx), sizeof(REAL_TYPE));
    in.read((char *)&(this->Vy), sizeof(REAL_TYPE));
    in.read((char *)&(this->Vz), sizeof(REAL_TYPE));
    in.read((char *)&(this->StartTime), sizeof(double));
    in.read((char *)&(this->LifeTime), sizeof(double));
    in.read((char *)&(this->CurrentTime), sizeof(double));
    in.read((char *)&(this->CurrentTimeStep), sizeof(unsigned int));
  }

} // namespace PPlib
