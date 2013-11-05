#include "FileConverter.h"

void print_usage_and_abort(char *cmd)
{
  std::cerr << "usage: " << cmd << " [-o csv | FVtext | FVbin] [Input file directory]" << std::endl;
  MPI_Abort(MPI_COMM_WORLD, -1);
}

int main(int argc, char *argv[])
{
  using namespace PPlib;

  int nproc, myrank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  //オプションから出力形式を読み取り、対応するFileOutputの派生クラスのインスタンスを生成する
  std::string format("FVbin");  //default output format
  int option;

  while((option = getopt(argc, argv, "o:")) != -1)
  {
    switch (option)
    {
      case 'o':
        format = optarg;
        break;
      case '?':                //unknown option
        print_usage_and_abort(argv[0]);
    }
  }

  LPT::FileOutput * FileOutput;
  if(format == "csv")
  {
    FileOutput = new LPT::CSV_Output;
  } else if(format == "FVtext")
  {
    FileOutput = new LPT::FV_ParticlePath;
  } else if(format == "FVbin")
  {
    FileOutput = LPT::FV_ParticlePathBinary::GetInstance();
  } else
  {
    print_usage_and_abort(argv[0]);
  }

  ////引数で指定されたディレクトリにある *.prt ファイルを読み込む
  //オプション以外の引数が無い場合は、カレントディレクトリ内が対象
  DIR *dp;
  struct dirent *entry;

  std::string dirname;
  std::list < std::string > InputFiles;

  if(optind < argc)
  {
    dirname = argv[optind];
  } else
  {
    dirname = "./";
  }
  if((dp = opendir(dirname.c_str())) == NULL)
  {
    std::cerr << "Couldn't open " << dirname << std::endl;
    std::abort();
  }

  entry = readdir(dp);
  while(entry != NULL)
  {
    std::string filename(entry->d_name);
    if(filename.find(".prt") != std::string::npos)
    {
      InputFiles.push_back(dirname + filename);
    }
    entry = readdir(dp);
  }
  closedir(dp);
  InputFiles.sort();

  //自Rankが処理を担当するファイルを決める
  std::list < std::string > MyInputFiles;
  int counter = 0;

  for(std::list < std::string >::iterator it = InputFiles.begin(); it != InputFiles.end(); ++it)
  {
    if(counter % nproc == myrank)
    {
      MyInputFiles.push_back(*it);
    }
    ++counter;
  }

  //タイムステップベースまたはIDベースのファイル出力オブジェクトを生成
  FileConverter *FC;

  if(format == "FVbin")
  {
    FC = new FileConverterTime;
  } else
  {
    FC = new FileConverterID;
  }

  //入力ファイルを開く
  std::list < std::ifstream * >InputFileStream;
  for(std::list < std::string >::iterator it = MyInputFiles.begin(); it != MyInputFiles.end(); it++)
  {
    std::cerr << "Reading: " << (*it) << std::endl;
    std::ifstream * tmp = new std::ifstream;
    tmp->open((*it).c_str(), std::ios::binary);
    InputFileStream.push_back(tmp);
  }

  //出力ファイルを開く
  LPT::FileManager::GetInstance()->SetBaseFileName("ParticleData");
  std::cerr << "WriteFileHeader" << std::endl;
  FileOutput->WriteFileHeader();

  std::cerr << "WriteBody" << std::endl;
  FC->WriteBody(InputFileStream, *FileOutput);

  std::cerr << "WriteFileFooter" << std::endl;
  FileOutput->WriteFileFooter();

  for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
  {
    (*it)->close();
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
