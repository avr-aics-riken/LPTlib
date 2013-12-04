#include "DataBlock.h"
namespace DSlib
{
  //for vlen=3 only
  std::ostream & operator <<(std::ostream & stream, DataBlock obj)
  {
    stream << "BlockID     = " << obj.BlockID << std::endl;
    stream << "SubDomainID = " << obj.SubDomainID << std::endl;
    stream << "Origin      = " << obj.Origin[0] << "," << obj.Origin[1] << "," << obj.Origin[2] << std::endl;
    stream << "OriginCell  = " << obj.OriginCell[0] << "," << obj.OriginCell[1] << "," << obj.OriginCell[2] << std::endl;
    stream << "BlockSize   = " << obj.BlockSize[0] << "," << obj.BlockSize[1] << "," << obj.BlockSize[2] << std::endl;
    stream << "Pitch       = " << obj.Pitch[0] << "," << obj.Pitch[1] << "," << obj.Pitch[2] << std::endl;
    stream << "Time        = " << obj.Time << std::endl;
    return stream;
  }
} // namespace DSlib
