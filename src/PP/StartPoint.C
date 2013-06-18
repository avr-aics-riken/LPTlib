#include <iostream>
#include <fstream>
#include <new>
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

  std::ostream & operator <<(std::ostream & stream, StartPoint & obj)
  {
    stream << "Coord1           = " << obj.Coord1[0] << "," << obj.Coord1[1] << "," << obj.Coord1[2] << std::endl;
    stream << "SumStartPoints   = " << obj.GetSumStartPoints() << std::endl;
    stream << "StartTime        = " << obj.StartTime << std::endl;
    stream << "ReleaseTime      = " << obj.ReleaseTime << std::endl;
    stream << "TimeSpan         = " << obj.TimeSpan << std::endl;
    stream << "ParticleLifeTime = " << obj.ParticleLifeTime << std::endl;
    stream << "LatestEmitTime   = " << obj.LatestEmitTime << std::endl;
    stream << "ID               = " << obj.ID[0] << "," << obj.ID[1] << std::endl;
    return stream;
  }

  std::istream & operator >>(std::istream & stream, StartPoint & obj)
  {
    stream >> obj.Coord1[0] >> obj.Coord1[1] >> obj.Coord1[2];
    stream >> obj.SumStartPoints;
    stream >> obj.StartTime >> obj.ReleaseTime >> obj.TimeSpan >> obj.ParticleLifeTime;

    return stream;
  }

  std::ostream & operator <<(std::ostream & stream, StartPoint * obj) {
    //TODO publicなメンバ関数Debug()を用意して、この中ではDebug()を呼ぶように変更する
    // つまり
    //   stream << obj->Debug();
    // だけにする
    if(typeid(Line) == typeid(*obj)) {
      stream << std::endl;
      stream << "Line" << std::endl;
      stream << *(dynamic_cast < Line * >(obj));
    } else if(typeid(Rectangle) == typeid(*obj)) {
      stream << std::endl;
      stream << "Rectangle" << std::endl;
      stream << *(dynamic_cast < Rectangle * >(obj));
    } else if(typeid(Cuboid) == typeid(*obj)) {
      stream << std::endl;
      stream << "Cuboid" << std::endl;
      stream << *(dynamic_cast < Cuboid * >(obj));
    } else if(typeid(Circle) == typeid(*obj)) {
      stream << std::endl;
      stream << "Circle" << std::endl;
      stream << *(dynamic_cast < Circle * >(obj));
    } else {
      stream << std::endl;
      stream << "StartPoint" << std::endl;
      stream << *obj;
    }
    return stream;
  }

  std::list < PPlib::ParticleData * >*StartPoint::EmitNewParticle(const double &CurrentTime, const unsigned int &CurrentTimeStep) {
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

    std::list < PPlib::ParticleData * >*ParticleList = new std::list < PPlib::ParticleData * >;
    if(DoEmit) {
      try {
        for(int i = 0; i < GetSumStartPoints(); i++) {
          PPlib::ParticleData * tmp = new ParticleData;
          ParticleList->push_back(tmp);
        }
      }
      catch(std::bad_alloc) {
        for(std::list < PPlib::ParticleData * >::iterator it = ParticleList->begin(); it != ParticleList->end(); it++) {
          delete *it;
        }
        delete ParticleList;

        std::cerr << "faild to allocate memory for ParticleData. particle emittion is skiped for this time step" << std::endl;
        return NULL;
      }

      std::vector < DSlib::DV3 > Coords;
      GetGridPointCoord(Coords);

      std::vector < DSlib::DV3 >::iterator itCoords = Coords.begin();
      for(std::list < PPlib::ParticleData * >::iterator it = ParticleList->begin(); it != ParticleList->end(); it++) {
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
      return ParticleList;
    } else {
      return NULL;
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

  void StartPoint::GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)
  {
    DSlib::DV3 tmpCoord;
    tmpCoord.x = Coord1[0];
    tmpCoord.y = Coord1[1];
    tmpCoord.z = Coord1[2];
    Coords.push_back(tmpCoord);
  }

  bool StartPoint::CheckReleasetime(const double &CurrentTime)
  {
    if(ReleaseTime <= 0) {
      return false;
    } else {
      return (StartTime + ReleaseTime) > CurrentTime ? false : true;
    }
  }

  std::vector < StartPoint * >*StartPoint::Divider(const int &NumParts) {
    std::vector < StartPoint * >*NewStartPoints = new std::vector < StartPoint * >;
    StartPoint *tmpStartPoint = new StartPoint;

    tmpStartPoint = this;
    NewStartPoints->push_back(tmpStartPoint);
    return NewStartPoints;
  }

} // namespace PPlib
