#include <sstream>
#include <fstream>
#include <cstdlib>

#include "FileManager.h"

namespace LPT
{
  std::string FileManager::BaseFileName = "ParticleOutput";

  std::string FileManager::GetFileName(const std::string & suffix)
  {
    int MyRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
    std::ostringstream oss;
    oss << GetBaseFileName() << "_" << MyRank << "." << suffix;
    return oss.str();
  }
} // namespace LPT
