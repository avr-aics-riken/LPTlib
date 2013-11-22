#include <string>
#include <vector>
#include <list>
#include "FileOutput_FVbin.h"

namespace LPT
{
  void FV_ParticlePathBinary::WriteFileHeader()
  {
    int FV_MAGIC = 0x00010203;
    int MajorVersion = 1.1; //It's not float. please see FieldView Reference Manual appendixK, p496.
    int MinorVersion = 1;
    char WriteBuffer[80];
      std::string FVPARTICLE("FVPARTICLES");
      std::vector < std::string > VariableNames;
      VariableNames.push_back("U-Velocity");
      VariableNames.push_back("V-Velocity");
      VariableNames.push_back("W-Velocity");
      VariableNames.push_back("Rank");  //その粒子を放出した開始点を持っていたプロセスのRank番号
      VariableNames.push_back("StartPoint_id");
      VariableNames.push_back("Particle_id");

      Out2.write((char *)&FV_MAGIC, sizeof(int));
      strncpy(WriteBuffer, FVPARTICLE.c_str(), 80);
      Out2.write((char *)WriteBuffer, sizeof(char) * 80);
      Out2.write((char *)&MajorVersion, sizeof(int));
      Out2.write((char *)&MinorVersion, sizeof(int));
    int NumVariables = VariableNames.size();
      Out2.write((char *)&NumVariables, sizeof(int));
    for(std::vector < std::string >::iterator it = VariableNames.begin(); it != VariableNames.end(); ++it)
    {
      strncpy(WriteBuffer, (*it).c_str(), 80);
      Out2.write((char *)WriteBuffer, sizeof(char) * 80);
    }
  }

  void FV_ParticlePathBinary::WriteRecordHeader()
  {
/* 
 * FV13.2のParticlePathファイルの仕様上タイムステップは必ず1から始まる必要がある
 * そこで第0ステップのデータが来たら第1ステップとして扱い
 * さらに第1ステップのデータが来たら出力をスキップする処理を追加している
 */
    static bool Output0thStep = false;
    int TimeStep = (*(Particles->begin())).second->CurrentTimeStep;

    if(TimeStep == 0) {
      Output0thStep = true;
      ++TimeStep;
    } else if(TimeStep == 1 && Output0thStep == true) {
      Skip1stStep = true;
      return;
    } else if(TimeStep > 1 && Skip1stStep == true) {
      Skip1stStep = false;
    }
    float Time = (*(Particles->begin())).second->CurrentTime;
    int NumParticle = Particles->size();

    Out2.write((char *)&TimeStep, sizeof(int));
    Out2.write((char *)&Time, sizeof(float));
    Out2.write((char *)&NumParticle, sizeof(int));
  }

  void FV_ParticlePathBinary::WriteRecord()
  {
    if(Skip1stStep) {
      return;
    }

    int ID = 1;

    for(std::multimap<long, PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
      float x            = (float)(*it).second->Coord[0];
      float y            = (float)(*it).second->Coord[1];
      float z            = (float)(*it).second->Coord[2];
      float u            = (float)(*it).second->ParticleVelocity[0];
      float v            = (float)(*it).second->ParticleVelocity[1];
      float w            = (float)(*it).second->ParticleVelocity[2];
      float Rank         = (float)(*it).second->StartPointID[0];
      float StartPointID = (float)(*it).second->StartPointID[1];
      float ParticleID   = (float)(*it).second->ParticleID;

      Out2.write((char *)&ID, sizeof(int)); //ID
      ++ID;
      Out2.write((char *)&x, sizeof(float));
      Out2.write((char *)&y, sizeof(float));
      Out2.write((char *)&z, sizeof(float));
      Out2.write((char *)&u, sizeof(float));
      Out2.write((char *)&v, sizeof(float));
      Out2.write((char *)&w, sizeof(float));
      Out2.write((char *)&Rank, sizeof(float));
      Out2.write((char *)&StartPointID, sizeof(float));
      Out2.write((char *)&ParticleID, sizeof(float));
    }
  }

} //namespace LPT
