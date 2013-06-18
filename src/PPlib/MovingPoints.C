#include <iostream>

#include "MovingPoints.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, MovingPoints & obj)
  {
    stream << "Coord  1        = " << obj.Coords[0] << "," << obj.Coords[1] << "," << obj.Coords[2] << std::endl;
    stream << "Time  1         = " << obj.Time[0] << std::endl;
    stream << "StartTime       = " << obj.StartTime << std::endl;
    stream << "ReleaseTime     = " << obj.ReleaseTime << std::endl;
    stream << "TimeSpan        = " << obj.TimeSpan << std::endl;
    stream << "LatestEmitTime  = " << obj.LatestEmitTime << std::endl;
    stream << "ID              = " << obj.ID[0] << "," << obj.ID[1] << std::endl;
    return stream;
  }

  void MovingPoints::UpdateStartPoint(double CurrentTime)
  {
  }
} // namespace PPlib
