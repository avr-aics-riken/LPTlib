#include <iostream>
#include <cmath>

#include "Interpolator.h"
#include "PP_Integrator.h"

namespace PPlib
{
  bool PP_Integrator::GetIntegrand(Interpolator & gus, const REAL_TYPE x_i[3], REAL_TYPE func[3])
  {
    REAL_TYPE v[3];
      gus.InterpolateData(x_i, v);

      func[0] = v[0] / gus.GetPitchX();
      func[1] = v[1] / gus.GetPitchY();
      func[2] = v[2] / gus.GetPitchZ();

      return true;
  }

  int PP_Integrator::RKG(Interpolator & gus, const double t_step, REAL_TYPE x_i[3])
  {
    const REAL_TYPE ck[4] = { 2.0, 1.0, 1.0, 2.0 };
    const REAL_TYPE cq[4] = { 0.5, 1.0 - sqrt(0.5), 1.0 + sqrt(0.5), 0.5 };
    const REAL_TYPE cx[4] = { 0.5, 1.0 - sqrt(0.5), 1.0 + sqrt(0.5), 0.5 / 3 };

    if(gus.GetPitchX() == 0.0 || gus.GetPitchY() == 0.0 || gus.GetPitchZ() == 0.0)
      return -3;

    REAL_TYPE q[3] = { 0.0, 0.0, 0.0 };
    REAL_TYPE func[3] = { 0.0, 0.0, 0.0 };

    REAL_TYPE k;
    REAL_TYPE r;

    for(int l = 0; l < 4; l++) {
#ifdef __INTEL_COMPILER
#pragma forceinline recursive
#endif
      GetIntegrand(gus, x_i, func);
      for(int n = 0; n < 3; n++) {
        k = t_step * func[n];
        r = cx[l] * (k - ck[l] * q[n]);

        x_i[n] += r;
        q[n] += 3.0 * r - cq[l] * k;

      } // end of for(n)
    } // end of for(l)

    return 0;
  }

} // namespace PPlib
