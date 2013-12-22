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
#include "FileWriter.h"

void print_usage_and_abort(char *cmd)
{
  std::cerr << "usage: " << cmd << " [-i input_file_directory] [-o | FV13 | FV14 | vtk | vtk_ascii]" << std::endl;
  MPI_Finalize();
  exit(1);
}

void ComandLineParser(const int &argc, char **argv, std::string &dir_name, BaseWriter **writer)
{
  dir_name="./";
  std::string writer_name="FV13";

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
  if(writer_name == "FV14")
  {
    BaseParticleFileWriter *file_writer = new FV14Writer;
    *writer = new TimeSlicedWriter(*file_writer);
  } else if(writer_name == "vtk"){
    std::cerr << "vtk binary writer is not implimented"<<std::endl;
  } else if(writer_name == "vtk_ascii"){
    BaseParticleFileWriter *file_writer = new VtpAsciiWriter;
    *writer = new TimeSlicedWriter(*file_writer);
  } else if(writer_name == "FV13") {
    BaseParticleFileWriter *file_writer = new FV13Writer;
    *writer = new BulkWriter(*file_writer);
  }else{
     print_usage_and_abort(argv[0]);
  }
}

//引数で指定されたディレクトリにある *.prt ファイルを読み込み
//自Rankが担当するファイル名をlistに入れて返す
std::list<std::string> * AssignMyInputFiles(std::string dirname)
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

int main(int argc, char *argv[])
{
  using namespace PPlib;

  MPI_Init(&argc, &argv);

  LPT::FileManager::GetInstance()->SetBaseFileName("ParticleData");
  std::string dir_name;
  BaseWriter *writer;
  ComandLineParser(argc, argv, dir_name, &writer);

  std::list<std::string> * input_files = AssignMyInputFiles(dir_name);

  //入力ファイルを開く
  std::list < std::ifstream * >InputFileStream;
  for(std::list < std::string >::iterator it = input_files->begin(); it != input_files->end(); ++it)
  {
    std::cerr << "reading: " << (*it) << std::endl;
    std::ifstream * tmp = new std::ifstream;
    tmp->open((*it).c_str(), std::ios::binary);
    InputFileStream.push_back(tmp);
  }

  if(InputFileStream.size() < 1)
  {
    print_usage_and_abort(argv[0]);
  }

  //TimeStepのsetを作る
  std::set<unsigned int> time_step_tree;
  for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
  {
    LPT::LPT_ParticleInput Input(*it);
    Input.ReadFileHeader();
    Input.ReadTimeSteps(&time_step_tree);
    (*it)->clear();
    (*it)->seekg(0);
  }

  //粒子データを出力
  (*writer)(time_step_tree, InputFileStream);

  //入力ファイルを閉じる
  for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
  {
    (*it)->close();
  }

  MPI_Finalize();
  return 0;
}
