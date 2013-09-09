#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <utility>
#include <cmath>
#include <typeinfo>

#include "StartPoint.h"
#include "ParticleData.h"
#include "Line.h"
#include "Rectangle.h"
#include "Circle.h"
#include "Cuboid.h"
#include "FileManager.h"
#include "DV3.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, StartPoint * obj) {
    stream << obj->TextPrint(stream);
    return stream;
  }

  void StartPoint::EmitNewParticle(std::list < ParticleData * >* ParticleList, const double &CurrentTime, const unsigned int &CurrentTimeStep) {
    static int id = 0;
    bool DoEmit = false;

    if(StartTime <= CurrentTime) {
      if(LatestEmitTime >= 0) {
        if(LatestEmitTime + TimeSpan <= CurrentTime) {
          DoEmit = true;
        }
      } else {
        DoEmit = true;
      }
    }

    std::list < PPlib::ParticleData * > tmpParticleList;
    if(DoEmit) {
      try {
        for(int i = 0; i < GetSumStartPoints(); i++) {
          PPlib::ParticleData * tmp = new ParticleData;
          tmpParticleList.push_back(tmp);
        }
      }
      catch(std::bad_alloc) {
        for(std::list < PPlib::ParticleData * >::iterator it = tmpParticleList.begin(); it != tmpParticleList.end(); it++) {
          delete *it;
        }
        std::cerr << "faild to allocate memory for ParticleData. particle emittion is skiped for this time step" << std::endl;
        return;
      }

      std::vector < DSlib::DV3 > Coords;
      GetGridPointCoord(Coords);

      std::vector < DSlib::DV3 >::iterator itCoords = Coords.begin();
      for(std::list < PPlib::ParticleData * >::iterator it = tmpParticleList.begin(); it != tmpParticleList.end(); it++) {
        (*it)->StartPointID[0] = ID[0];
        (*it)->StartPointID[1] = ID[1];
        (*it)->ParticleID = id++;
        (*it)->StartTime = CurrentTime;
        (*it)->LifeTime = ParticleLifeTime;
        (*it)->CurrentTime = CurrentTime;
        (*it)->CurrentTimeStep = CurrentTimeStep;

        (*it)->Coord[0] = (*itCoords).x;
        (*it)->Coord[1] = (*itCoords).y;
        (*it)->Coord[2] = (*itCoords).z;
        ++itCoords;
      }
      this->LatestEmitTime = CurrentTime;
      ParticleList->splice(ParticleList->end(), tmpParticleList);
    }
  }

  void StartPoint::DividePoints(std::vector < DSlib::DV3 > *Coords, const int &NumPoints, DSlib::DV3 & Coord1, DSlib::DV3 & Coord2)
  {
    if(NumPoints == 1) {
      DSlib::DV3 tmpCoord;
      tmpCoord.x = Coord1.x;
      tmpCoord.y = Coord1.y;
      tmpCoord.z = Coord1.z;
      Coords->push_back(tmpCoord);
    } else {
      for(int i = 0; i < NumPoints; i++) {
        DSlib::DV3 tmpCoord;
        tmpCoord.x = Coord1.x == Coord2.x ? Coord1.x : Coord1.x + (Coord2.x - Coord1.x) / (NumPoints - 1) * i;
        tmpCoord.y = Coord1.y == Coord2.y ? Coord1.y : Coord1.y + (Coord2.y - Coord1.y) / (NumPoints - 1) * i;
        tmpCoord.z = Coord1.z == Coord2.z ? Coord1.z : Coord1.z + (Coord2.z - Coord1.z) / (NumPoints - 1) * i;
        Coords->push_back(tmpCoord);
      }
    }
  }

  bool StartPoint::CheckReleasetime(const double &CurrentTime)
  {
    if(ReleaseTime <= 0) {
      return false;
    } else {
      return (StartTime + ReleaseTime) > CurrentTime ? false : true;
    }
  }

} // namespace PPlib
