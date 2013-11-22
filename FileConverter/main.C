#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <cstdlib>
#include <unistd.h>
#include "ParticleData.h"
#include "FileManager.h"
#include "LPT_ParticleInput.h"
#include "FileOutput.h"
#include "FileOutput_FVbin.h"
#include "FileOutput_FVtext.h"
#include "FileOutput_csv.h"
#include "FileOutput2.h"

//引数で指定されたディレクトリにある *.prt ファイルを読み込み
//自Rankが担当するファイル名をlistに入れて返す
std::list<std::string> * make_input_file_list(std::string dirname)
{
  DIR *dp;
  if((dp = opendir(dirname.c_str())) == NULL)
  {
    std::cerr << "Couldn't open " << dirname << std::endl;
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  struct dirent *entry;
  std::list < std::string > input_files;
  entry = readdir(dp);
  while(entry != NULL)
  {
    std::string filename(entry->d_name);
    if(filename.find(".prt") != std::string::npos)
    {
      input_files.push_back(dirname + filename);
    }
    entry = readdir(dp);
  }
  closedir(dp);
  input_files.sort();

  std::list < std::string > * my_input_files = new std::list<std::string>;

  int nproc, myrank;
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  int counter = 0;
  for(std::list < std::string >::iterator it = input_files.begin(); it != input_files.end(); ++it)
  {
    if(counter % nproc == myrank)
    {
      my_input_files->push_back(*it);
    }
    ++counter;
  }
  return my_input_files;
}

/// vtk(ascii)形式で粒子データをtimestep毎に出力する
void write_vtk_file(std::set<unsigned int> &time_step_tree, std::list < std::ifstream * > &InputFileStream)
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

    LPT::BaseFileOutput * vtk_writer = 
      new LPT::VTK_PolyDataFileFooter(
          new LPT::VTK_PointDataFooter(
            new LPT::VTK_DataArrayParticleVelocity(
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
    vtk_writer->write(&Particles, LPT::FileManager::GetInstance()->GetFileNameWithTimeStep("vtp",(*it_time_step)));
    delete vtk_writer;
  }
}

// FieldView13のParticlePath形式でファイルを出力する
void write_fv13(std::set<unsigned int> &time_step_tree, std::list < std::ifstream * > &InputFileStream)
{
  LPT::FileOutput *writer = LPT::FV_ParticlePathBinary::GetInstance();
  writer->WriteFileHeader();
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

    std::multimap < long, PPlib::ParticleData *> Particles2;

    for(std::list < PPlib::ParticleData * >::iterator it =Particles.begin(); it!= Particles.end();++it)
    {
      Particles2.insert(std::make_pair((*it)->BlockID, (*it)));
    }

    //fvp形式で出力
    writer->SetParticles(&Particles2);
    writer->WriteRecordHeader();
    writer->WriteRecord();
    writer->WriteRecordFooter();
  }  
  writer->WriteFileFooter();
}

void print_usage_and_abort(char *cmd)
{
  std::cerr << "usage: " << cmd << " [-i input_file_directory] [-o FV13text | FV13 | FV14 | vtk | vtk_ascii]" << std::endl;
  MPI_Abort(MPI_COMM_WORLD, -1);
}

void my_getopt(const int &argc, char **argv, std::string &dir_name, std::string& writer_name)
{
  dir_name="./";
  writer_name="FV13";

  int results;
  while((results=getopt(argc,argv,"i:o:")) != -1)
  {
    switch(results)
    {
      case 'i':
        dir_name=optarg;
        break;
      case 'o':
        writer_name=optarg;
        break;
      case '?':
        print_usage_and_abort(argv[0]);
        break;
    }
  }
}

int main(int argc, char *argv[])
{
  using namespace PPlib;

  MPI_Init(&argc, &argv);

  std::string dir_name;
  std::string writer_name;
  my_getopt(argc, argv, dir_name, writer_name);

  std::list<std::string> * input_files = make_input_file_list(dir_name);

  //入力ファイルを開く
  std::list < std::ifstream * >InputFileStream;
  for(std::list < std::string >::iterator it = input_files->begin(); it != input_files->end(); ++it)
  {
    std::cerr << "Reading: " << (*it) << std::endl;
    std::ifstream * tmp = new std::ifstream;
    tmp->open((*it).c_str(), std::ios::binary);
    InputFileStream.push_back(tmp);
  }

  //IDとTimeStepのsetを作る
  std::set < LPT::ID > id_tree;
  std::set<unsigned int> time_step_tree;
  for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
  {
    LPT::LPT_ParticleInput Input(*it);
    Input.ReadFileHeader();
    Input.ReadTimeSteps(&time_step_tree);
    Input.ReadIDs(&id_tree);
  }

  //粒子データを出力
  LPT::FileManager::GetInstance()->SetBaseFileName("ParticleData");
  if(writer_name == "vtk_ascii")
  {
    write_vtk_file(time_step_tree, InputFileStream);
  }else if(writer_name == "FV13"){
    write_fv13(time_step_tree, InputFileStream);
  }

  //入力ファイルを閉じる
  for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
  {
    (*it)->close();
  }

  MPI_Finalize();
  return 0;
}
