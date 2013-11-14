#include <iostream>
#include <algorithm>

#include "DecompositionManager.h"
#include "Communicator.h"
#include "LPT_LogOutput.h"

namespace DSlib
{

  void DecompositionManager::Decomposer(const int Length, const int NumBlocks, std::vector < int >*Parts)
  {
    int Numpart = Length / NumBlocks;
    int Reminder = Length % NumBlocks;

      Parts->resize(NumBlocks);
      Parts->assign(NumBlocks, Numpart);

      std::vector < int >::iterator it = Parts->begin();
    while(Reminder > 0)
    {
      ++(*it);
      ++it;
      --Reminder;
    }
  }

  template < typename T > void DecompositionManager::IndexConvert1Dto3D(const T Index1D, int *Index3D, const int NumBlockX, const int NumBlockY)
  {
    Index3D[0] = (Index1D % (NumBlockX * NumBlockY)) % NumBlockX;
    Index3D[1] = (Index1D % (NumBlockX * NumBlockY)) / NumBlockX;
    Index3D[2] = (Index1D / (NumBlockX * NumBlockY));
  }

  void DecompositionManager::DumpSubDomainBoundary()
  {
    LPT::LPT_LOG::GetInstance()->INFO("SubDomainBoundaryX = ", SubDomainBoundaryX, NPx + 1);
    LPT::LPT_LOG::GetInstance()->INFO("SubDomainBoundaryY = ", SubDomainBoundaryY, NPy + 1);
    LPT::LPT_LOG::GetInstance()->INFO("SubDomainBoundaryZ = ", SubDomainBoundaryZ, NPz + 1);
  }

  void DecompositionManager::DumpBlockBoundary()
  {
    LPT::LPT_LOG::GetInstance()->INFO("BlockBoundaryX = ", BlockBoundaryX, NBx * NPx + 1);
    LPT::LPT_LOG::GetInstance()->INFO("BlockBoundaryY = ", BlockBoundaryY, NBy * NPy + 1);
    LPT::LPT_LOG::GetInstance()->INFO("BlockBoundaryZ = ", BlockBoundaryZ, NBz * NPz + 1);
    LPT::LPT_LOG::GetInstance()->INFO("RealBlockBoundaryX = ", &RealBlockBoundaryX[0], NBx * NPx + 1);
    LPT::LPT_LOG::GetInstance()->INFO("RealBlockBoundaryY = ", &RealBlockBoundaryY[0], NBy * NPy + 1);
    LPT::LPT_LOG::GetInstance()->INFO("RealBlockBoundaryZ = ", &RealBlockBoundaryZ[0], NBz * NPz + 1);
  }

  void DecompositionManager::Initialize(const REAL_TYPE & arg_Nx, const REAL_TYPE & arg_Ny, const REAL_TYPE & arg_Nz, const REAL_TYPE & arg_NPx, const REAL_TYPE & arg_NPy, const REAL_TYPE & arg_NPz, const REAL_TYPE & arg_NBx, const REAL_TYPE & arg_NBy,
                                        const REAL_TYPE & arg_NBz, const REAL_TYPE & arg_OriginX, const REAL_TYPE & arg_OriginY, const REAL_TYPE & arg_OriginZ, const REAL_TYPE & arg_dx, const REAL_TYPE & arg_dy, const REAL_TYPE & arg_dz,
                                        const int &arg_GuideCellSize)
  {
    initialized = true;
    Nx = arg_Nx;
    Ny = arg_Ny;
    Nz = arg_Nz;
    NPx = arg_NPx;
    NPy = arg_NPy;
    NPz = arg_NPz;
    NBx = arg_NBx;
    NBy = arg_NBy;
    NBz = arg_NBz;
    OriginX = arg_OriginX;
    OriginY = arg_OriginY;
    OriginZ = arg_OriginZ;
    dx = arg_dx;
    dy = arg_dy;
    dz = arg_dz;

    GuideCellSize = arg_GuideCellSize;
    std::vector < int >Parts;

    LPT::LPT_LOG::GetInstance()->LOG("calc SubDomainBoundary X");
    SubDomainBoundaryX = new int[NPx + 1];
    Decomposer(Nx, NPx, &Parts);
    SubDomainBoundaryX[0] = 0;
    for(int i = 0; i < NPx; i++) {
      SubDomainBoundaryX[i+1] = SubDomainBoundaryX[i] + Parts[i];
    }

    LPT::LPT_LOG::GetInstance()->LOG("calc SubDomainBoundary Y");
    SubDomainBoundaryY = new int[NPy + 1];
    Decomposer(Ny, NPy, &Parts);
    SubDomainBoundaryY[0] = 0;
    for(int i = 0; i < NPy; i++) {
      SubDomainBoundaryY[i+1] = SubDomainBoundaryY[i] + Parts[i];
    }

    LPT::LPT_LOG::GetInstance()->LOG("calc SubDomainBoundary Z");
    SubDomainBoundaryZ = new int[NPz + 1];
    Decomposer(Nz, NPz, &Parts);
    SubDomainBoundaryZ[0] = 0;
    for(int i = 0; i < NPz; i++) {
      SubDomainBoundaryZ[i+1] = SubDomainBoundaryZ[i] + Parts[i];
    }

    DumpSubDomainBoundary();

    LPT::LPT_LOG::GetInstance()->LOG("calc BlockBoundary X");
    BlockBoundaryX = new int[NBx * NPx + 1];
    for(int j = 0; j < NPx; j++) {
      Decomposer((SubDomainBoundaryX[j + 1] - SubDomainBoundaryX[j]), NBx, &Parts);
      BlockBoundaryX[j * NBx] = SubDomainBoundaryX[j];
      for(int i = 1; i <= NBx; i++) {
        BlockBoundaryX[j * NBx + i] = BlockBoundaryX[j * NBx + i - 1] + Parts[i - 1];
      }
    }

    RealBlockBoundaryX.resize(NBx * NPx + 1);
    for(int i = 0; i < NBx * NPx + 1; ++i) {
      RealBlockBoundaryX[i] = OriginX+dx * BlockBoundaryX[i];
    }

    LPT::LPT_LOG::GetInstance()->LOG("calc BlockBoundary Y");
    BlockBoundaryY = new int[NBy * NPy + 1];
    for(int j = 0; j < NPy; j++) {
      Decomposer((SubDomainBoundaryY[j + 1] - SubDomainBoundaryY[j]), NBy, &Parts);
      BlockBoundaryY[j * NBy] = SubDomainBoundaryY[j];
      for(int i = 1; i <= NBy; i++) {
        BlockBoundaryY[j * NBy + i] = BlockBoundaryY[j * NBy + i - 1] + Parts[i - 1];
      }
    }
    RealBlockBoundaryY.resize(NBy * NPy + 1);
    for(int i = 0; i < NBy * NPy + 1; ++i) {
      RealBlockBoundaryY[i] = OriginY+dy * BlockBoundaryY[i];
    }

    LPT::LPT_LOG::GetInstance()->LOG("calc BlockBoundary Z");
    BlockBoundaryZ = new int[NBz * NPz + 1];
    for(int j = 0; j < NPz; j++) {
      Decomposer((SubDomainBoundaryZ[j + 1] - SubDomainBoundaryZ[j]), NBz, &Parts);
      BlockBoundaryZ[j * NBz] = SubDomainBoundaryZ[j];
      for(int i = 1; i <= NBz; i++) {
        BlockBoundaryZ[j * NBz + i] = BlockBoundaryZ[j * NBz + i - 1] + Parts[i - 1];
      }
    }
    RealBlockBoundaryZ.resize(NBz * NPz + 1);
    for(int i = 0; i < NBz * NPz + 1; ++i) {
      RealBlockBoundaryZ[i] = OriginZ+dz * BlockBoundaryZ[i];
    }

    LPT::LPT_LOG::GetInstance()->LOG("calc LargestBlockSize");
    LargestBlockSize = (GetBlockSizeX(0) + 2 * GetGuideCellSize()) * (GetBlockSizeY(0) + 2 * GetGuideCellSize()) * (GetBlockSizeZ(0) + 2 * GetGuideCellSize());

    DumpBlockBoundary();
  }

  void DecompositionManager::FindNeighborBlockID(const long &id, std::set < long >*Neighbors)
  {
    int BlockID3D[3];
    int tmpBlockID3D[3];
    long tmp_id;

    IndexConvert1Dto3D(id, BlockID3D, NBx * NPx, NBy * NPy);
    for(int k = -1; k <= 1; k++) {
      for(int j = -1; j <= 1; j++) {
        for(int i = -1; i <= 1; i++) {
          tmpBlockID3D[0] = BlockID3D[0] + i;
          tmpBlockID3D[1] = BlockID3D[1] + j;
          tmpBlockID3D[2] = BlockID3D[2] + k;
          if(((0 <= tmpBlockID3D[0]) && (tmpBlockID3D[0] < NBx * NPx)) && ((0 <= tmpBlockID3D[1]) && (tmpBlockID3D[1] < NBy * NPy)) && ((0 <= tmpBlockID3D[2]) && (tmpBlockID3D[2] < NBz * NPz))) {
            tmp_id = Convert3Dto1D(tmpBlockID3D[0], tmpBlockID3D[1], tmpBlockID3D[2], NBx * NPx, NBy * NPy);
            Neighbors->insert(tmp_id);
          }
        }
      }
    }
  }

  int DecompositionManager::FindSubDomainIDByBlock(const long &id)
  {
    int BlockID3D[3];
    int SubDomainID3D[3];

    IndexConvert1Dto3D(id, BlockID3D, NBx * NPx, NBy * NPy);

    SubDomainID3D[0] = BlockID3D[0] / NBx;
    SubDomainID3D[1] = BlockID3D[1] / NBy;
    SubDomainID3D[2] = BlockID3D[2] / NBz;

    return Convert3Dto1Dint(SubDomainID3D[0], SubDomainID3D[1], SubDomainID3D[2], NPx, NPy);
  }

  long DecompositionManager::FindBlockIDByCoordBinary(const DV3 & Coord)
  {
    int BlockID3D[3];

    if(CheckBounds(Coord) != 0) {
      return -1;
    }

    std::vector < REAL_TYPE >::iterator itX = std::upper_bound(RealBlockBoundaryX.begin(), RealBlockBoundaryX.end(), Coord.x);
    --itX;
    BlockID3D[0] = std::distance(RealBlockBoundaryX.begin(), itX);

    std::vector < REAL_TYPE >::iterator itY = std::upper_bound(RealBlockBoundaryY.begin(), RealBlockBoundaryY.end(), Coord.y);
    --itY;
    BlockID3D[1] = std::distance(RealBlockBoundaryY.begin(), itY);

    std::vector < REAL_TYPE >::iterator itZ = std::upper_bound(RealBlockBoundaryZ.begin(), RealBlockBoundaryZ.end(), Coord.z);
    --itZ;
    BlockID3D[2] = std::distance(RealBlockBoundaryZ.begin(), itZ);

    return Convert3Dto1Dlong(BlockID3D[0], BlockID3D[1], BlockID3D[2], NBx * NPx, NBy * NPy);

  }

  long DecompositionManager::FindBlockIDByCoordLinear(const DV3 & Coord)
  {
    if(CheckBounds(Coord) != 0) {
      return -1;
    }

    int BlockID3D[3];

    for(int i = 1; i <= NBx * NPx; i++) {
      if(Coord[0] < OriginX + BlockBoundaryX[i] * dx) {
        BlockID3D[0] = i - 1;
        break;
      }
    }
    for(int i = 1; i <= NBy * NPy; i++) {
      if(Coord[1] < OriginY + BlockBoundaryY[i] * dy) {
        BlockID3D[1] = i - 1;
        break;
      }
    }
    for(int i = 1; i <= NBz * NPz; i++) {
      if(Coord[2] < OriginZ + BlockBoundaryZ[i] * dz) {
        BlockID3D[2] = i - 1;
        break;
      }
    }
    return Convert3Dto1Dlong(BlockID3D[0], BlockID3D[1], BlockID3D[2], NBx * NPx, NBy * NPy);

  }

  int DecompositionManager::CheckBounds(DV3 Coords)
  {
    int retval = CheckBoundX(Coords.x) + CheckBoundY(Coords.y) + CheckBoundZ(Coords.z);
      if(retval % 10 == 1)
        LPT::LPT_LOG::GetInstance()->WARN("X-coordinate is too small: ", Coords.x);
      if(retval % 10 == 2)
        LPT::LPT_LOG::GetInstance()->WARN("X-coordinate is too large: ", Coords.x);
      if((retval / 10) % 10 == 1)
        LPT::LPT_LOG::GetInstance()->WARN("Y-coordinate is too small: ", Coords.y);
      if((retval / 10) % 10 == 2)
        LPT::LPT_LOG::GetInstance()->WARN("Y-coordinate is too large: ", Coords.y);
      if(retval / 100 == 1)
        LPT::LPT_LOG::GetInstance()->WARN("Z-coordinate is too small: ", Coords.z);
      if(retval / 100 == 2)
        LPT::LPT_LOG::GetInstance()->WARN("Z-coordinate is too large: ", Coords.z);
    return retval;
  }

  int DecompositionManager::CheckBoundX(REAL_TYPE XCoord)
  {
    if(XCoord < OriginX)
      return 1;
    if(XCoord > OriginX + dx * Nx)
      return 2;
    return 0;
  }

  int DecompositionManager::CheckBoundY(REAL_TYPE YCoord)
  {
    if(YCoord < OriginY)
      return 10;
    if(YCoord > OriginY + dy * Ny)
      return 20;
    return 0;
  }

  int DecompositionManager::CheckBoundZ(REAL_TYPE ZCoord)
  {
    if(ZCoord < OriginZ)
      return 100;
    if(ZCoord > OriginZ + dz * Nz)
      return 200;
    return 0;
  }

} // namespace DSlib
