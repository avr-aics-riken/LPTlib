/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include "Point.h"
#include "ParticleData.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, const Point& obj)
  {
    stream << obj.TextPrint(stream);
    return stream;
  }

  std::istream & operator >>(std::istream & stream, Point& obj)
  {
    stream >> obj.Coord1[0] >> obj.Coord1[1] >> obj.Coord1[2];
    stream >> obj.SumStartPoints;
    stream >> obj.StartTime >> obj.ReleaseTime >> obj.TimeSpan >> obj.ParticleLifeTime;

    return stream;
  }

  void Point::GetGridPointCoord(std::vector < REAL_TYPE > &Coords)
  {
    Coords.push_back(Coord1[0]);
    Coords.push_back(Coord1[1]);
    Coords.push_back(Coord1[2]);
  }

  void Point::Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints)
  {
    if(MaxNumStartPoints >= GetSumStartPoints()) {
      Point *NewPoint= new Point(*this);
      StartPoints->push_back(NewPoint);
    } else {
      // MaxNumStartPointsがGetSumStartPoints() (=必ず1)より小さい場合はwarnningを出力するだけ
      LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
    }
    return;
  }


} // namespace PPlib
