/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <iostream>
#include <cmath>

#include "Interpolator.h"
#include "PP_Integrator.h"
#include "DataBlock.h"

namespace PPlib
{
bool PP_Integrator::GetIntegrand(const DSlib::DataBlock& DataBlock, const REAL_TYPE x_i[3], REAL_TYPE func[3])
{
    REAL_TYPE v[3];
    Interpolator::InterpolateData(DataBlock, x_i, v);

    func[0] = v[0]/DataBlock.Pitch[0];
    func[1] = v[1]/DataBlock.Pitch[1];
    func[2] = v[2]/DataBlock.Pitch[2];

    return true;
}

int PP_Integrator::RKG(const DSlib::DataBlock& DataBlock, const double t_step, REAL_TYPE x_i[3])
{
    const REAL_TYPE ck[4] = {(REAL_TYPE)2.0, (REAL_TYPE)1.0, (REAL_TYPE)1.0, (REAL_TYPE)2.0};
    const REAL_TYPE cq[4] = {(REAL_TYPE)0.5, (REAL_TYPE)1.0-(REAL_TYPE)sqrt(0.5), (REAL_TYPE)1.0+(REAL_TYPE)sqrt(0.5), (REAL_TYPE)0.5};
    const REAL_TYPE cx[4] = {(REAL_TYPE)0.5, (REAL_TYPE)1.0-(REAL_TYPE)sqrt(0.5), (REAL_TYPE)1.0+(REAL_TYPE)sqrt(0.5), (REAL_TYPE)0.5/3};

    REAL_TYPE       q[3]    = {0.0, 0.0, 0.0};
    REAL_TYPE       func[3] = {0.0, 0.0, 0.0};

    REAL_TYPE       k;
    REAL_TYPE       r;

    for(int l = 0; l < 4; l++)
    {
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
        GetIntegrand(DataBlock, x_i, func);
        for(int n = 0; n < 3; n++)
        {
            k = t_step*func[n];
            r = cx[l]*(k-ck[l]*q[n]);

            x_i[n] += r;
            q[n]   += 3.0*r-cq[l]*k;
        } // end of for(n)
    } // end of for(l)

    return 0;
}
} // namespace PPlib
