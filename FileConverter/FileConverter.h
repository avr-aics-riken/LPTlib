#ifndef FILE_CONVERTER_H
#define FILE_CONVERTER_H
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
#include "FileOutput_FVbin.h"
#include "FileOutput_FVtext.h"
#include "FileOutput_csv.h"

class FileConverter
{
public:
  virtual void WriteBody(std::list < std::ifstream * >&InputFileStream, LPT::FileOutput & FileOutput) = 0;
};

class FileConverterID:public FileConverter
{
public:
  void WriteBody(std::list < std::ifstream * >&InputFileStream, LPT::FileOutput & FileOutput);
};
class FileConverterTime:public FileConverter
{
public:
  void WriteBody(std::list < std::ifstream * >&InputFileStream, LPT::FileOutput & FileOutput);
};

#endif
