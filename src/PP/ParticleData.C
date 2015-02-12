/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include "ParticleData.h"

namespace PPlib
{
std::ostream& operator<<(std::ostream& stream, ParticleData& obj)
{
    stream<<"StartPointID                     = "<<obj.StartPointID1<<","<<obj.StartPointID2<<std::endl;
    stream<<"ParticleID                       = "<<obj.ParticleID<<std::endl;
    stream<<"BlockID                          = "<<obj.BlockID<<std::endl;
    stream<<"Coord                            = "<<obj.x<<","<<obj.y<<","<<obj.z<<std::endl;
    stream<<"ParticleVelocity                 = "<<obj.Vx<<","<<obj.Vy<<","<<obj.Vz<<std::endl;
    stream<<"StartTime, LifeTime, CurrentTime = "<<obj.StartTime<<","<<obj.LifeTime<<","<<obj.CurrentTime<<std::endl;
    return stream;
}
} // namespace PPlib