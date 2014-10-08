/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef DSLIB_DECOMPOSITION_MANAGER_H
#define DSLIB_DECOMPOSITION_MANAGER_H

#include <iostream>
#include <vector>
#include <set>
#include "LPT_LogOutput.h"

namespace DSlib
{
//forward declaration
  class CommDataBlockHeader;

  //! @brief サブドメインおよびデータブロックの分割情報を保持するクラス
  //!
  //! BlockIDおよびSubDomainIDは1次元のアドレスだが、このクラス内部では3次元のアドレスとして扱い、取り出す時に3Dto1Dの変換を行なう
  class DecompositionManager
  {
    //!  @brief x軸方向の領域分割の境目になる座標を保持する配列
    //!  BoundaryX[i]からBoundaryX[i+1]までがi番目のサブドメイン
    int* SubDomainBoundaryX;

    //! SubDomainBoundaryXと同様でY方向の値を保持する配列
    int* SubDomainBoundaryY;

    //! SubDomainBoundaryXと同様でZ方向の値を保持する配列
    int* SubDomainBoundaryZ;

    //!  @brief x軸方向のデータブロック分割の境目になるセルidを保持する
    //!  BlockBoundaryX[i]からBlockBoundaryX[i+1]までがi番目のブロック
    //!  ここでは、ブロックIDはグローバルID(サブドメインを跨いで通し番号)となっていることに注意
    int* BlockBoundaryX;

    //! BlockBoundaryXと同様
    int* BlockBoundaryY;

    //! BlockBoundaryXと同様
    int* BlockBoundaryZ;

    //! x軸方向のデータブロック分割の境目になる座標を保持する
    std::vector < REAL_TYPE > RealBlockBoundaryX;

    //!  RealBlockBoundaryXと同様
    std::vector < REAL_TYPE > RealBlockBoundaryY;

    //! RealBlockBoundaryXと同様
    std::vector < REAL_TYPE > RealBlockBoundaryZ;

    //!  計算領域のx方向のサイズ 単位はセル数
    int Nx;

    //! 計算領域のy方向のサイズ 単位はセル数 
    int Ny;

    //! 計算領域のz方向のサイズ 単位はセル数
    int Nz;

    //!  x軸方向の分割数(サブドメイン数)
    int NPx;

    //! y軸方向の分割数(サブドメイン数)
    int NPy;

    //! z軸方向の分割数(サブドメイン数)
    int NPz;

    //! 1サブドメインあたりのx軸方向のデータブロック数
    int NBx;

    //! 1サブドメインあたりのx軸方向のデータブロック数
    int NBy;

    //! 1サブドメインあたりのx軸方向のデータブロック数
    int NBz;

    //! 解析領域全体の原点のx座標
    REAL_TYPE OriginX;

    //! 解析領域全体の原点のy座標
    REAL_TYPE OriginY;

    //! 解析領域全体の原点のz座標
    REAL_TYPE OriginZ;

    //! x方向のセル幅
    REAL_TYPE dx;

    //! y方向のセル幅
    REAL_TYPE dy;

    //! z方向のセル幅
    REAL_TYPE dz;

    //! BlockID=0(全ブロック中最も大きいブロック)が持つセル数
    int LargestBlockSize;

    //! LengthをNumBlocksで分割し個々の領域のサイズをPartsに先頭から順に格納して返す
    void Decomposer(const int Length, const int NumBlocks, std::vector < int >*Parts);

    //! 流体から転送してくる袖領域のサイズx,y,z全方向で+-の両方にGuideCell数分の袖領域があることを示す
    int GuideCellSize;

    //! Initialize()が呼ばれたかどうかのフラグ
    bool initialized;

  private:
    //Singletonパターンを適用
    DecompositionManager()
    {
      initialized = false;
    }
    DecompositionManager(const DecompositionManager & obj);
    DecompositionManager & operator=(const DecompositionManager & obj);

    //! デストラクタ
     ~DecompositionManager()
    {
      delete[] SubDomainBoundaryX;
      delete[] SubDomainBoundaryY;
      delete[] SubDomainBoundaryZ;
      delete[] BlockBoundaryX;
      delete[] BlockBoundaryY;
      delete[] BlockBoundaryZ;
    }

  public:
    static DecompositionManager *GetInstance()
    {
      static DecompositionManager instance;

      return &instance;
    }

    //! 1次元のindexを3次元のindexに変換する
    template < typename T > static void IndexConvert1Dto3D(const T Index1D, int *Index3D, const int NumBlockX, const int NumBlockY);


    //! 3次元のindexを1次元のindex(int)に変換する
    template < typename  T > static int Convert3Dto1Dint(T i, T j, T k, T imax, T jmax)
    {
      return i + j * imax + k * imax * jmax;
    }

    //! 3次元のindexを1次元のindex(long)に変換する
    template < typename  T > static long Convert3Dto1Dlong(T i, T j, T k, T imax, T jmax)
    {
      return i + j * imax + k * imax * jmax;
    }

    //! 3次元のindexを1次元のindex(size_t)に変換する
    template < typename T > static size_t Convert3Dto1D(T i, T j, T k, T imax, T jmax)
    {
      return i + j * imax + k * imax * jmax;
    }

    //! 4次元のindexを1次元のindex(size_t)に変換する
    template < typename  T > static size_t Convert4Dto1D(T i, T j, T k, T l, T imax, T jmax, T kmax)
    {
      return i + j * imax + k * imax * jmax + l * imax * jmax * kmax;
    }

  private:
    //! @brief サブドメインの境界を出力する
    void DumpSubDomainBoundary();

    //! @brief ブロックの境界を出力する
    void DumpBlockBoundary();

    //! @brief 引数で渡されたx座標が解析領域外に出ていないか判定する 
    //! @retval   2 X-plus方向に出ている 
    //! @retval   1 X-minus方向に出ている 
    //! @retval   0 領域内                   
    int CheckBoundX(REAL_TYPE XCoord);

    //! @brief 引数で渡されたy座標が解析領域外に出ていないか判定する
    //! @retval  20 Y-plus方向に出ている
    //! @retval  10 Y-minus方向に出ている
    //! @retval   0 領域内
    int CheckBoundY(REAL_TYPE YCoord);

    //! @brief 引数で渡されたz座標が解析領域外に出ていないか判定する
    //! @retval 200 Z-plus方向に出ている
    //! @retval 100 Z-minus方向に出ている
    //! @retval   0 領域内
    int CheckBoundZ(REAL_TYPE ZCoord);

    int GetBlockIDX(const long &BlockID)
    {
      return (BlockID % (NBx * NPx * NBy * NPy)) % (NBx * NPx);
    };
    int GetBlockIDY(const long &BlockID)
    {
      return (BlockID % (NBx * NPx * NBy * NPy)) / (NBx * NPx);
    };
    int GetBlockIDZ(const long &BlockID)
    {
      return BlockID / (NBx * NPx * NBy * NPy);
    };
    int GetSubDomainIDX(const int &SubDomainID)
    {
      return (SubDomainID % (NPx * NPy)) % NPx;
    };
    int GetSubDomainIDY(const int &SubDomainID)
    {
      return (SubDomainID % (NPx * NPy)) / NPx;
    };
    int GetSubDomainIDZ(const int &SubDomainID)
    {
      return SubDomainID / (NPx * NPy);
    };

  public:
    //! @brief Nx,Ny,Nz,NPx,NPy,NPz,NBx,NBy,NBzの値を元に、{Block,SubDomain}Boundary? の値を設定する
    void Initialize(const REAL_TYPE & arg_Nx,
                    const REAL_TYPE & arg_Ny,
                    const REAL_TYPE & arg_Nz,
                    const REAL_TYPE & arg_NPx,
                    const REAL_TYPE & arg_NPy,
                    const REAL_TYPE & arg_NPz,
                    const REAL_TYPE & arg_NBx,
                    const REAL_TYPE & arg_NBy,
                    const REAL_TYPE & arg_NBz, const REAL_TYPE & arg_OriginX, const REAL_TYPE & arg_OriginY, const REAL_TYPE & arg_OriginZ, const REAL_TYPE & arg_dx, const REAL_TYPE & arg_dy, const REAL_TYPE & arg_dz, const int &arg_GuideCellSize);

    //! @brief 与えられたブロックIDの場所の流体計算を担当するプロセスのRank番号(=subdomain ID)を返す
    //! @param id [in] サブドメインIDを探したいデータブロックのID
    //! @retval 引数で渡したIDのデータブロックが存在するサブドメインのID
    int FindSubDomainIDByBlock(const long &id);

    //! @brief 与えられた座標を含むデータブロックのIDを返す(線形探索版)
    //! @param Coord [in] 座標
    //! @retval 引数で渡した座標を含むデータブロックのID
    long FindBlockIDByCoordLinear(REAL_TYPE Coord[3]);
    long FindBlockIDByCoordLinear(const REAL_TYPE& x, const REAL_TYPE& y, const REAL_TYPE& z)
    {
      REAL_TYPE Coord[3]={x,y,z};
      return FindBlockIDByCoordLinear(Coord);
    }

    //! @brief 与えられた座標を含むデータブロックのIDを返す(二分探索版)
    //! @param Coord [in] 座標
    //! @retval 引数で渡した座標を含むデータブロックのID
    long FindBlockIDByCoordBinary(REAL_TYPE Coord[3]);
    long FindBlockIDByCoordBinary(const REAL_TYPE& x, const REAL_TYPE& y, const REAL_TYPE& z)
    {
      REAL_TYPE Coord[3]={x,y,z};
      return FindBlockIDByCoordBinary(Coord);
    }

    //! @brief 与えられたブロックIDの周囲にあるブロックIDの配列を返す
    //! @param id        [in]  周辺のブロックを探したいデータブロックのID
    //! @param Neighbors [out] 周辺のブロックID
    void FindNeighborBlockID(const long &id, std::set < long >*Neighbors);

    //! @brief 引数で渡された座標が解析領域外に出ていないか判定する
    //! CheckBound{X,Y,Z}の戻り値を加算して返すので、戻り値の意味はそちらを参照のこと
    int CheckBounds(REAL_TYPE Coord[3]);


    //! Accessor
    REAL_TYPE Getdx()
    {
      return this->dx;
    }
    REAL_TYPE Getdy()
    {
      return this->dy;
    }
    REAL_TYPE Getdz()
    {
      return this->dz;
    }
    int GetLargestBlockSize()
    {
      return this->LargestBlockSize;
    };
    int GetGuideCellSize()
    {
      return this->GuideCellSize;
    };
    int GetBlockOriginCellX(const long &BlockID)
    {
      return BlockBoundaryX[GetBlockIDX(BlockID)];
    };
    int GetBlockOriginCellY(const long &BlockID)
    {
      return BlockBoundaryY[GetBlockIDY(BlockID)];
    };
    int GetBlockOriginCellZ(const long &BlockID)
    {
      return BlockBoundaryZ[GetBlockIDZ(BlockID)];
    };
    REAL_TYPE GetBlockOriginX(const long &BlockID)
    {
      return OriginX + BlockBoundaryX[GetBlockIDX(BlockID)] * dx;
    };
    REAL_TYPE GetBlockOriginY(const long &BlockID)
    {
      return OriginY + BlockBoundaryY[GetBlockIDY(BlockID)] * dy;
    };
    REAL_TYPE GetBlockOriginZ(const long &BlockID)
    {
      return OriginZ + BlockBoundaryZ[GetBlockIDZ(BlockID)] * dz;
    };

    int GetBlockSizeX(const long &BlockID)
    {
      return BlockBoundaryX[GetBlockIDX(BlockID) + 1] - BlockBoundaryX[GetBlockIDX(BlockID)];
    };
    int GetBlockSizeY(const long &BlockID)
    {
      return BlockBoundaryY[GetBlockIDY(BlockID) + 1] - BlockBoundaryY[GetBlockIDY(BlockID)];
    };
    int GetBlockSizeZ(const long &BlockID)
    {
      return BlockBoundaryZ[GetBlockIDZ(BlockID) + 1] - BlockBoundaryZ[GetBlockIDZ(BlockID)];
    };

    int GetSubDomainOriginCellX(const int &SubDomainID)
    {
        return SubDomainBoundaryX[GetSubDomainIDX(SubDomainID)];
    }
    int GetSubDomainOriginCellY(const int &SubDomainID)
    {
        return SubDomainBoundaryY[GetSubDomainIDY(SubDomainID)];
    }
    int GetSubDomainOriginCellZ(const int &SubDomainID)
    {
        return SubDomainBoundaryZ[GetSubDomainIDZ(SubDomainID)];
    }
    int GetSubDomainSizeX(const int &SubDomainID)
    {
      return SubDomainBoundaryX[GetSubDomainIDX(SubDomainID) + 1] - SubDomainBoundaryX[GetSubDomainIDX(SubDomainID)];
    };
    int GetSubDomainSizeY(const int &SubDomainID)
    {
      return SubDomainBoundaryY[GetSubDomainIDY(SubDomainID) + 1] - SubDomainBoundaryY[GetSubDomainIDY(SubDomainID)];
    };
    int GetSubDomainSizeZ(const int &SubDomainID)
    {
      return SubDomainBoundaryZ[GetSubDomainIDZ(SubDomainID) + 1] - SubDomainBoundaryZ[GetSubDomainIDZ(SubDomainID)];
    };

    int GetBlockLocalOffset(const long &BlockID, const int &SubDomainID)
    {
      return (BlockBoundaryX[GetBlockIDX(BlockID)] - SubDomainBoundaryX[GetSubDomainIDX(SubDomainID)])
        + (BlockBoundaryY[GetBlockIDY(BlockID)] - SubDomainBoundaryY[GetSubDomainIDY(SubDomainID)])
        * (GetSubDomainSizeX(GetSubDomainIDX(SubDomainID)) + GetGuideCellSize() * 2)
        + (BlockBoundaryZ[GetBlockIDZ(BlockID)] - SubDomainBoundaryZ[GetSubDomainIDZ(SubDomainID)])
        * (GetSubDomainSizeX(GetSubDomainIDX(SubDomainID)) + GetGuideCellSize() * 2)
        * (GetSubDomainSizeY(GetSubDomainIDY(SubDomainID)) + GetGuideCellSize() * 2);
    }

  };

} // namespace DSlib
#endif
