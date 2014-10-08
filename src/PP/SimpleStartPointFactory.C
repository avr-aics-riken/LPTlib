/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include "SimpleStartPointFactory.h"
#include "StartPointAll.h"
namespace PPlib
{
  Point* PointFactory::create(REAL_TYPE Coord1[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    Point* tmpStartPoint = new Point;
      tmpStartPoint->SetCoord1(Coord1);
      tmpStartPoint->SetSumStartPoints(1);
      tmpStartPoint->SetStartTime(StartTime);
      tmpStartPoint->SetReleaseTime(ReleaseTime);
      tmpStartPoint->SetTimeSpan(TimeSpan);
      tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);

      return tmpStartPoint;
  }

  Line* LineFactory::create(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int SumStartPoints, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    Line* tmpStartPoint = new Line;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetCoord2(Coord2);
    tmpStartPoint->SetSumStartPoints(SumStartPoints);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);

    return tmpStartPoint;
  }

  Rectangle* RectangleFactory::create(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    if(Coord1[0] == Coord2[0]) {
      if(NumStartPoints[0] != 1)
        return NULL;
    } else if(Coord1[1] == Coord2[1]) {
      if(NumStartPoints[1] != 1)
        return NULL;
    } else if(Coord1[2] == Coord2[2]) {
      if(NumStartPoints[2] != 1)
        return NULL;
    } else {
      return NULL;
    }

    Rectangle * tmpStartPoint = new Rectangle;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetCoord2(Coord2);
    tmpStartPoint->SetNumStartPoints(NumStartPoints);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);

    return tmpStartPoint;
  }

  Cuboid* CuboidFactory::create(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    Cuboid* tmpStartPoint = new Cuboid;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetCoord2(Coord2);
    tmpStartPoint->SetNumStartPoints(NumStartPoints);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);

    return tmpStartPoint;
  }

  Circle *CircleFactory::create(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime)
  {
    Circle * tmpStartPoint = new Circle;
    tmpStartPoint->SetCoord1(Coord1);
    tmpStartPoint->SetSumStartPoints(SumStartPoints);
    tmpStartPoint->SetRadius(Radius);
    tmpStartPoint->SetNormalVector(NormalVector);
    tmpStartPoint->SetStartTime(StartTime);
    tmpStartPoint->SetReleaseTime(ReleaseTime);
    tmpStartPoint->SetTimeSpan(TimeSpan);
    tmpStartPoint->SetParticleLifeTime(ParticleLifeTime);
    if( ! tmpStartPoint->Initialize()) {
      delete tmpStartPoint;
      tmpStartPoint=NULL;
    }

    return tmpStartPoint;
  }

} // namespace PPlib
