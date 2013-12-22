#ifndef FILE_WRITER_H
#define FILE_WRITER_H
#include <iostream>
#include "LPT_ParticleInput.h"
#include "FileOutput.h"
#include "FileOutput_FVbin.h"
#include "FileOutput_FV14bin.h"
#include "FileOutput_FVtext.h"
#include "FileOutput_csv.h"
#include "FileOutput_vtp.h"

class BaseParticleFileWriter
{
public:
  virtual void operator()(std::list < PPlib::ParticleData * >& Particles, const unsigned int& time_step){};
};

/// vtk(ascii)形式で粒子データを出力する関数オブジェクト
class VtpAsciiWriter : public BaseParticleFileWriter
{
public:
  void operator()(std::list < PPlib::ParticleData * >& Particles, const unsigned int& time_step)
  {
    LPT::BaseFileOutput * vtk_writer = 
      new LPT::VTK_PolyDataFileFooter(
          new LPT::VTK_PointDataFooter(
            new LPT::VTK_DataArrayVectorByASCII <REAL_TYPE> ("Particle Velocity", &PPlib::ParticleData::Vx, &PPlib::ParticleData::Vy, &PPlib::ParticleData::Vz, 
              new LPT::VTK_PointDataHeader(
                new LPT::VTK_Points(
                  new LPT::VTK_PolyDataFileHeader(
                    new LPT::BaseFileOutput()
                    )
                  )
                )
              )
            )
          );
    vtk_writer->write(&Particles, LPT::FileManager::GetInstance()->GetFileNameWithTimeStep("vtp",(time_step)));
    delete vtk_writer;
  }
};

class FV14Writer: public BaseParticleFileWriter
{
  void operator()(std::list < PPlib::ParticleData * >& Particles, const unsigned int& time_step)
  {
    LPT::FV14_bin writer(LPT::FileManager::GetInstance()->GetFileNameWithTimeStep("fvp",(time_step)));

    writer.WriteFileHeader();

    writer.SetParticles(&Particles);
    writer.WriteRecordHeader();
    writer.WriteRecord();
  }
};

class FV13Writer: public BaseParticleFileWriter
{
  LPT::FV_ParticlePathBinary * writer;
public:
  FV13Writer()
  {
    writer = LPT::FV_ParticlePathBinary::GetInstance();
    writer->WriteFileHeader();
  }
  void operator()(std::list < PPlib::ParticleData * >& Particles, const unsigned int& time_step)
  {
    writer->SetParticles(&Particles);
    writer->WriteRecordHeader();
    writer->WriteRecord();
    writer->WriteRecordFooter();
  }
};

class BaseWriter
{
protected:
  BaseParticleFileWriter &write;
public:
  BaseWriter(BaseParticleFileWriter &arg_write):write(arg_write){}
  virtual void operator ()(std::set<unsigned int> &time_step_tree, std::list < std::ifstream * > &InputFileStream)=0; 
};

/// timestep毎に出力する
class TimeSlicedWriter:public BaseWriter
{
public:
  TimeSlicedWriter(BaseParticleFileWriter &arg_write):BaseWriter(arg_write){}
  void operator()(std::set<unsigned int> &time_step_tree, std::list < std::ifstream * > &InputFileStream)
  {
    LPT::FileManager::GetInstance()->SetMaxTimeStep(*(time_step_tree.rbegin()));
    for (std::set<unsigned int>::iterator it_time_step = time_step_tree.begin(); it_time_step != time_step_tree.end(); ++it_time_step)
    {
      //粒子データの読み込み
      std::list < PPlib::ParticleData * >Particles;
      for(std::list < std::ifstream * >::iterator it_input_file = InputFileStream.begin(); it_input_file != InputFileStream.end(); ++it_input_file)
      {
        (*it_input_file)->clear();
        (*it_input_file)->seekg(0);
        LPT::LPT_ParticleInput Input(*it_input_file);
        Input.SetParticles(&Particles);
        Input.ReadFileHeader();
        Input.ReadRecord(*it_time_step);
      }
      //ID順にソート
      Particles.sort(PPlib::CompareID());
    
      //コンストラクタで渡された関数オブジェクトを使ってファイル出力
      write(Particles, *it_time_step);
    }
  }
};

/// 全タイムステップの出力を1ファイルに追記で出力する
class BulkWriter: public BaseWriter
{
public:
  BulkWriter(BaseParticleFileWriter &arg_write):BaseWriter(arg_write){}
  void operator()(std::set<unsigned int> &time_step_tree, std::list < std::ifstream * > &InputFileStream)
  {
    for (std::set<unsigned int>::iterator it_time_step = time_step_tree.begin(); it_time_step != time_step_tree.end(); ++it_time_step)
    {
      //粒子データの読み込み
      std::list < PPlib::ParticleData * >Particles;
      for(std::list < std::ifstream * >::iterator it_input_file = InputFileStream.begin(); it_input_file != InputFileStream.end(); ++it_input_file)
      {
        (*it_input_file)->clear();
        (*it_input_file)->seekg(0);
        LPT::LPT_ParticleInput Input(*it_input_file);
        Input.SetParticles(&Particles);
        Input.ReadFileHeader();
        Input.ReadRecord(*it_time_step);
      }
      //ID順にソート
      Particles.sort(PPlib::CompareID());
    
      //fvp形式で出力
      write(Particles, *it_time_step);
    }  
  }
};


#endif
