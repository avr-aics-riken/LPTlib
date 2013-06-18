#include <iostream>
#include <cstdlib>
#include <cmath>

#include "Interpolator.h"
#include "DecompositionManager.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
  bool Interpolator::setup(DSlib::DataBlock * DataBlock)
  {
    if(DataBlock == NULL)
      return false;
    m_dims[0] = DataBlock->BlockSize[0];
    m_dims[1] = DataBlock->BlockSize[1];
    m_dims[2] = DataBlock->BlockSize[2];
    p_vecd = DataBlock->Data;
    m_vecLen = 3; //TODO DataBlockから取ってくるようにする?
    m_halo = DSlib::DecompositionManager::GetInstance()->GetGuideCellSize();
    m_orig[0] = DataBlock->Origin[0];
    m_orig[1] = DataBlock->Origin[1];
    m_orig[2] = DataBlock->Origin[2];
    m_pitch[0] = DataBlock->Pitch[0];
    m_pitch[1] = DataBlock->Pitch[1];
    m_pitch[2] = DataBlock->Pitch[2];

    if(m_pitch[0] <= 0.0 || m_pitch[1] <= 0.0 || m_pitch[2] <= 0.0)
      return false;
    if(m_dims[0] * m_dims[1] * m_dims[2] < 1)
      return false;
    return true;
  }

  bool Interpolator::InterpolateData(const REAL_TYPE x_I[3], REAL_TYPE dval[3])
  {
    if(!p_vecd)
      return false;

    REAL_TYPE X[8];
    int i = int (x_I[0]);
    int j = int (x_I[1]);
    int k = int (x_I[2]);

    if(x_I[0] > m_dims[0] - 1) {
      i = m_dims[0] - 1;
      LPT::LPT_LOG::GetInstance()->WARN("using extrapolation:x_i[0]  is too large");
      LPT::LPT_LOG::GetInstance()->LOG("x_I[0] = ", x_I[0]);
      LPT::LPT_LOG::GetInstance()->LOG("m_dims[0] - 1 = ", m_dims[0] - 1);

    } else if(x_I[0] < 0) {
      i = 0;
      LPT::LPT_LOG::GetInstance()->WARN("using extrapolation:x_i[0]  is too small");
      LPT::LPT_LOG::GetInstance()->LOG("x_I[0] = ", x_I[0]);
    }

    if(x_I[1] > m_dims[1] - 1) {
      j = m_dims[1] - 1;
      LPT::LPT_LOG::GetInstance()->WARN("using extrapolation:x_i[1]  is too large");
      LPT::LPT_LOG::GetInstance()->LOG("x_I[1] = ", x_I[1]);
      LPT::LPT_LOG::GetInstance()->LOG("m_dims[1] - 1 = ", m_dims[1] - 1);
    } else if(x_I[1] < 0) {
      j = 0;
      LPT::LPT_LOG::GetInstance()->LOG("x_I[1] = ", x_I[1]);
      LPT::LPT_LOG::GetInstance()->WARN("using extrapolation:x_i[1]  is too small");
    }

    if(x_I[2] > m_dims[2] - 1) {
      k = m_dims[2] - 1;
      LPT::LPT_LOG::GetInstance()->WARN("using extrapolation:x_i[2]  is too large");
      LPT::LPT_LOG::GetInstance()->LOG("x_I[2] = ", x_I[2]);
      LPT::LPT_LOG::GetInstance()->LOG("m_dims[2] - 1 = ", m_dims[2] - 1);
    } else if(x_I[2] < 0) {
      k = 0;
      LPT::LPT_LOG::GetInstance()->LOG("x_I[2] = ", x_I[2]);
      LPT::LPT_LOG::GetInstance()->WARN("using extrapolation:x_i[2]  is too small");
    }

    REAL_TYPE ip = x_I[0] - (REAL_TYPE) i;
    REAL_TYPE jp = x_I[1] - (REAL_TYPE) j;
    REAL_TYPE kp = x_I[2] - (REAL_TYPE) k;
    REAL_TYPE im = (REAL_TYPE) (i + 1) - x_I[0];
    REAL_TYPE jm = (REAL_TYPE) (j + 1) - x_I[1];
    REAL_TYPE km = (REAL_TYPE) (k + 1) - x_I[2];

    for(int dkind = 0; dkind < m_vecLen; dkind++) {
      X[0] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i, j, k, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i,  j,  k  )
      X[1] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i + 1, j, k, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i+1,j,  k  )
      X[2] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i + 1, j + 1, k, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i+1,j+1,k  )
      X[3] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i, j + 1, k, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i,  j+1,k  )
      X[4] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i, j, k + 1, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i,  j,  k+1)
      X[5] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i + 1, j, k + 1, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i+1,j  ,k+1)
      X[6] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i + 1, j + 1, k + 1, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i+1,j+1,k+1)
      X[7] = (REAL_TYPE) p_vecd[DSlib::DecompositionManager::Convert4Dto1D(i, j + 1, k + 1, dkind, m_dims[0], m_dims[1], m_dims[2])]; // (i,  j+1,k+1)
      dval[dkind] = (REAL_TYPE) (im * jm * km * X[0] + ip * jm * km * X[1] + ip * jp * km * X[2] + im * jp * km * X[3] + im * jm * kp * X[4] + ip * jm * kp * X[5] + ip * jp * kp * X[6] + im * jp * kp * X[7]);
    }
    return true;
  }

  void Interpolator::ConvItoX(const REAL_TYPE x_I[3], REAL_TYPE x[3])
  {
    x[0] = m_orig[0] + (x_I[0] - (m_halo - 0.5)) * m_pitch[0];
    x[1] = m_orig[1] + (x_I[1] - (m_halo - 0.5)) * m_pitch[1];
    x[2] = m_orig[2] + (x_I[2] - (m_halo - 0.5)) * m_pitch[2];
  }

  void Interpolator::ConvXtoI(const REAL_TYPE x[3], REAL_TYPE x_i[3])
  {
    x_i[0] = ((x[0] - m_orig[0]) / m_pitch[0] + (m_halo - 0.5));
    x_i[1] = ((x[1] - m_orig[1]) / m_pitch[1] + (m_halo - 0.5));
    x_i[2] = ((x[2] - m_orig[2]) / m_pitch[2] + (m_halo - 0.5));
  }

} // namespace PPlib
