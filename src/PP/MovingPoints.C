/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

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
