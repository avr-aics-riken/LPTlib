#ifndef CSV_OUTPUT_H
#define CSV_OUTPUT_H

#include <list>
#include "FileManager.h"
#include "FileOutput.h"

namespace LPT
{
  //!  @brief CSV形式(カンマ区切り)で粒子データを出力するクラス
  class CSV_Output:public FileOutput
  {
  public:
    CSV_Output()
    {
      Out.open((FileManager::GetInstance()->GetFileName("csv")).c_str());
    }

    void WriteFileHeader()
    {
      Out << "x,y,z,CurrentTime,CurrentTimeStep,U-Velocity,V-Velocity,W-Velocity,Rank,StartPointID,ParticleID" << std::endl;
    }
    void WriteRecord()
    {
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        Out << (*it)->x << ",";
        Out << (*it)->y << ",";
        Out << (*it)->z << ",";
        Out << (*it)->CurrentTime << ",";
        Out << (*it)->CurrentTimeStep << ",";
        Out << (*it)->Vx << ",";
        Out << (*it)->Vy << ",";
        Out << (*it)->Vz << ",";
        Out << (*it)->StartPointID1 << ",";
        Out << (*it)->StartPointID2 << ",";
        Out << (*it)->ParticleID << ",";
        Out << std::endl;
      }
    }
  };

} //namespace LPT
#endif
