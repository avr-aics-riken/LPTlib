#include <iostream>

#include "MovingPoints.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, const MovingPoints& obj)
  {
    stream << obj.TextPrint(stream);
    return stream;
  }

  void MovingPoints::UpdateStartPoint(double CurrentTime)
  {
  }
} // namespace PPlib
