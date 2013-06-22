#ifndef PPLIB_SIMPLE_STARTPOINT_FACTORY_H
#define PPLIB_SIMPLE_STARTPOINT_FACTORY_H


namespace PPlib
{
  //forward decralation
  class Point;
  class Line;
  class Rectangle;
  class Cuboid;
  class Circle;

  class PointFactory
  {
  public:
    static Point *create(REAL_TYPE Coord1[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
  };
  class LineFactory
  {
  public:
    static Line *create(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int SumStartPoints, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
  };
  class RectangleFactory
  {
  public:
    static Rectangle *create(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
  };
  class CuboidFactory
  {
  public:
    static Cuboid *create(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
  };
  class CircleFactory
  {
  public:
    static Circle *create(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
  };
} // namespace PPlib
#endif
