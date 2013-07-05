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

  void Point::GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)
  {
    DSlib::DV3 DV3Coord(Coord1[0], Coord1[1], Coord1[2]);
    Coords.push_back(DV3Coord);
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
