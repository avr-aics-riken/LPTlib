#include <sstream>
#include <cstdlib>

#include "LPT_ParticleInput.h"

namespace LPT
{
  void LPT_ParticleInput::DetermineEndian()
  {
    int FileBOM[4];
      In->read((char *)FileBOM, sizeof(int) * 4);
    if((BOM[0] != FileBOM[0]) || (BOM[1] != FileBOM[1]) || (BOM[2] != FileBOM[2]) || (BOM[3] != FileBOM[3]))
    {
      std::cerr << "LPT ERROR: Endian error" << std::endl;
      std::abort();
    }
  }

  void LPT_ParticleInput::ReadFileHeader()
  {
    DetermineEndian();
    double FileVersion;

    In->read((char *)&FileVersion, sizeof(double));
    if(FileVersion != ParticleDataFormatVersion) {
      std::cerr << "LPT ERROR: inconsistent File format version !" << std::endl;
      std::abort();
    }
    size_t tmp;

    In->read((char *)&tmp, sizeof(size_t));
    if(tmp != SizeOfRealType) {
      std::cerr << "LPT ERROR: inconsistent REAL_TYPE size !" << std::endl;
      std::abort();
    }

  }

  void LPT_ParticleInput::ReadRecordHeader()
  {
    In->read((char *)&RecordSize, sizeof(unsigned long));
  }

  void LPT_ParticleInput::ReadRecord()
  {
    while(!In->eof()) {
      ReadRecordHeader();
      for(int i = 0; i < RecordSize; i++) {
        PPlib::ParticleData * Particle = new PPlib::ParticleData;
        Particle->BinaryRead(*In);
        Particles->push_back(Particle);
      }
    }

  }

  void LPT_ParticleInput::ReadRecord(const ID & ParticleID)
  {
    while(!In->eof()) {
      ReadRecordHeader();
      for(int i = 0; i < RecordSize && !In->eof(); i++) {
        PPlib::ParticleData * Particle = new PPlib::ParticleData;
        Particle->BinaryRead(*In);
        if(ParticleID.StartPointID_0 == (Particle->StartPointID)[0] && ParticleID.StartPointID_1 == (Particle->StartPointID)[1] && ParticleID.ParticleID == Particle->ParticleID) {
          Particles->push_back(Particle);
        } else {
          delete Particle;
        }
      }
    }
  }

  void LPT_ParticleInput::ReadRecord(const int &TimeStep)
  {
    while(!In->eof()) {
      ReadRecordHeader();
      for(int i = 0; i < RecordSize && !In->eof(); i++) {
        PPlib::ParticleData * Particle = new PPlib::ParticleData;
        Particle->BinaryRead(*In);
        if(TimeStep == Particle->CurrentTimeStep) {
          Particles->push_back(Particle);
        } else {
          delete Particle;
        }
      }
    }
  }

  void LPT_ParticleInput::ReadIDs(std::set < ID > *IDs)
  {
    std::list < PPlib::ParticleData * >Particles;
    SetParticles(&Particles);
    while(!In->eof()) {
      ReadRecordHeader();
      for(int i = 0; i < RecordSize; i++) {
        PPlib::ParticleData Particle;
        Particle.BinaryRead(*In);

        ID *tmpID = new ID;

        tmpID->StartPointID_0 = Particle.StartPointID[0];
        tmpID->StartPointID_1 = Particle.StartPointID[1];
        tmpID->ParticleID = Particle.ParticleID;
        std::pair < std::set < ID >::iterator, bool > rt = IDs->insert(*tmpID);
        if(!rt.second) {
          delete tmpID;
        }
      }
    }
  }

  void LPT_ParticleInput::ReadTimeSteps(std::set < unsigned int >*TimeSteps)
  {
    std::list < PPlib::ParticleData * >Particles;
    SetParticles(&Particles);
    while(!In->eof()) {
      ReadRecordHeader();
      for(int i = 0; i < RecordSize; i++) {
        PPlib::ParticleData Particle;
        Particle.BinaryRead(*In);
        TimeSteps->insert(Particle.CurrentTimeStep);
      }
    }
  }

} //namespace LPT
