/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <iostream>
#include <cstdlib>
#include <cmath>

#include "Interpolator.h"
#include "DecompositionManager.h"
#include "LPT_LogOutput.h"
#include "DataBlock.h"

namespace PPlib
{
bool Interpolator::InterpolateData(const DSlib::DataBlock& DataBlock, const REAL_TYPE x_I[3], REAL_TYPE dval[3])
{
    if(!DataBlock.Data) return false;

    int       i = int(x_I[0]);
    int       j = int(x_I[1]);
    int       k = int(x_I[2]);

    REAL_TYPE ip = x_I[0]-(REAL_TYPE)i;
    REAL_TYPE jp = x_I[1]-(REAL_TYPE)j;
    REAL_TYPE kp = x_I[2]-(REAL_TYPE)k;
    REAL_TYPE im = (REAL_TYPE)(i+1)-x_I[0];
    REAL_TYPE jm = (REAL_TYPE)(j+1)-x_I[1];
    REAL_TYPE km = (REAL_TYPE)(k+1)-x_I[2];

#define INDEX(i, j, k, l) ((i)+(j)*DataBlock.BlockSize[0]+(k)*DataBlock.BlockSize[0]*DataBlock.BlockSize[1]+(l)*DataBlock.BlockSize[0]*DataBlock.BlockSize[1]*DataBlock.BlockSize[2])
    for(int l = 0; l < 3; l++)
    {
        dval[l] = ( im*jm*km*(DataBlock.Data[INDEX(i,   j,   k,   l)])
                   +ip*jm*km*(DataBlock.Data[INDEX(i+1, j,   k,   l)])
                   +ip*jp*km*(DataBlock.Data[INDEX(i+1, j+1, k,   l)])
                   +im*jp*km*(DataBlock.Data[INDEX(i,   j+1, k,   l)])
                   +im*jm*kp*(DataBlock.Data[INDEX(i,   j,   k+1, l)])
                   +ip*jm*kp*(DataBlock.Data[INDEX(i+1, j,   k+1, l)])
                   +ip*jp*kp*(DataBlock.Data[INDEX(i+1, j+1, k+1, l)])
                   +im*jp*kp*(DataBlock.Data[INDEX(i,   j+1, k+1, l)])
                   );
    }
#undef INDEX
    return true;
}

void Interpolator::ConvXtoI(const REAL_TYPE x_g[3], REAL_TYPE x_l[3], const REAL_TYPE orig[3], const REAL_TYPE pitch[3])
{
    static const int halo = DSlib::DecompositionManager::GetInstance()->GetGuideCellSize();
    x_l[0] = ((x_g[0]-orig[0])/pitch[0]+(halo-0.5));
    x_l[1] = ((x_g[1]-orig[1])/pitch[1]+(halo-0.5));
    x_l[2] = ((x_g[2]-orig[2])/pitch[2]+(halo-0.5));
}

void Interpolator::ConvItoX(const REAL_TYPE x_l[3], REAL_TYPE x_g[3], const REAL_TYPE orig[3], const REAL_TYPE pitch[3])
{
    static const int halo = DSlib::DecompositionManager::GetInstance()->GetGuideCellSize();
    x_g[0] = orig[0]+(x_l[0]-(halo-0.5))*pitch[0];
    x_g[1] = orig[1]+(x_l[1]-(halo-0.5))*pitch[1];
    x_g[2] = orig[2]+(x_l[2]-(halo-0.5))*pitch[2];
}
} // namespace PPlib
