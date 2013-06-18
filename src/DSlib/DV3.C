#include "DV3.h"

namespace DSlib
{
  std::ostream & operator <<(std::ostream & stream, const DV3 & obj)
  {
    return stream << obj.x << "," << obj.y << "," << obj.z;
  }
} // namespace DSlib
