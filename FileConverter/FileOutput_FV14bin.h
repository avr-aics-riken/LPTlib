#ifndef FILE_OUTPUT_FV14_BIN_H
#define FILE_OUTPUT_FV14_BIN_H
#include "FileManager.h"
#include "FileOutput.h"

namespace LPT
{
  //! @brief FieldView14 ParticlePath形式(Binary)で粒子データを出力するクラス
  class FV14_bin:public FileOutput
  {
    FV14_bin(){};
  public:
    FV14_bin(std::string filename)
    {
      Out.open(filename.c_str(), std::ios::binary);
      std::cerr <<"writing: "<<filename<<std::endl;
    }
    void WriteFileHeader()
    {
      //Prepare file header
      std::vector < std::string > VariableNames;
      VariableNames.push_back("U-Velocity");
      VariableNames.push_back("V-Velocity");
      VariableNames.push_back("W-Velocity");
      VariableNames.push_back("Rank");  //その粒子を放出した開始点を持っていたプロセスのRank番号
      VariableNames.push_back("StartPoint_id");
      VariableNames.push_back("Particle_id");
    

      //Write file header
      WriteVariable(0x0010203); //FV_MAGIC
      {
        char write_buffer[80]="FVPARTICLES";
        Out.write((char *)write_buffer, sizeof(char) * 80);
      }
      WriteVariable(3);  //Major version
      WriteVariable(1);  //Minor version
      //FieldViewのリファレンスマニュアルにはここに0を出力するように記載されているが
      //実際には0があると正常に読めないので削除した 2014/1/7
      //WriteVariable(0);  //reserved 
      WriteVariable((int)VariableNames.size());
      for(std::vector < std::string >::iterator it = VariableNames.begin(); it != VariableNames.end(); ++it)
      {
        char write_buffer[80];
        strncpy(write_buffer, (*it).c_str(), 80);
        Out.write((char *)write_buffer, sizeof(char) * 80);
      }
    }

    void WriteRecordHeader()
    {
      WriteVariable((int)Particles->size());
    }

    void WriteRecord()
    {
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((float)(*it)->x);
        WriteVariable((float)(*it)->y);
        WriteVariable((float)(*it)->z);
      }

      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((float)(*it)->Vx);
      }
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((float)(*it)->Vy);
      }
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((float)(*it)->Vz);
      }
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((float)(*it)->StartPointID1);
      }
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((float)(*it)->StartPointID2);
      }
      for(std::list<PPlib::ParticleData*>::iterator it = Particles->begin(); it != Particles->end(); ++it) {
        WriteVariable((float)(*it)->ParticleID);
      }
    }

  private:
    template <typename T>
    void WriteVariable(const T& var)
    {
      Out.write((char *)&var, sizeof(var));
    }

  };
}

#endif
