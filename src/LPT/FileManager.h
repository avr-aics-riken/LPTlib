#ifndef LPT_FILE_MANAGER_H
#define LPT_FILE_MANAGER_H

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <mpi.h>

namespace LPT
{
  //! @brief LPTが入/出力するファイルのstreamを生成/保持するクラス
  //! InputまたはOutputのどちらかでしかファイルストリームを開けないようにしている
  class FileManager
  {
  private:
    //Singletonパターンを適用
    FileManager() : BaseFileName("ParticleOutput"), MaxTimeStep(1000)
    {
    }
    FileManager(const FileManager & obj);
    FileManager & operator=(const FileManager & obj);
    ~FileManager()
    {
    }

  public:
    static FileManager *GetInstance()
    {
      static FileManager instance;
      return &instance;
    }

    /// 引数で指定した拡張子に対応したファイル名を返す
    std::string GetFileName(const std::string & suffix)
    {
      int MyRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
      std::ostringstream oss;
      oss << GetBaseFileName() << "_" << MyRank << "." << suffix;
      return oss.str();
    }

    /// 引数で指定した拡張子に対応したファイル名を返す(タイムステップ付き)
    std::string GetFileNameWithTimeStep(const std::string & suffix, const int & timestep)
    {
      int MyRank;
      int NumProc;
      MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
      MPI_Comm_size(MPI_COMM_WORLD, &NumProc);

      std::ostringstream oss;
      oss << GetBaseFileName() << "_";
      oss << std::setfill('0') <<std::setw(CountDigit(NumProc)) <<MyRank << "_";
      oss << std::setfill('0') <<std::setw(CountDigit(MaxTimeStep)) << timestep << "." << suffix;
      return oss.str();
    }

    ///Accessor
    std::string GetBaseFileName(void)
    {
      return this->BaseFileName;
    };
    void SetBaseFileName(std::string BaseFileName)
    {
      this->BaseFileName = BaseFileName;
    };
    void SetMaxTimeStep(long MaxTimeStep)
    {
      this->MaxTimeStep=MaxTimeStep;
    }

  private:
    //! @brief ファイル入出力メソッドで使われるファイル名のprefix
    //! この後にRank番号を付け加えたものが実際のファイル名となる
    std::string BaseFileName;

    //! @brief GetFileNameWithTimeStep()で使うタイムステップの最大値
    long MaxTimeStep;

    //! @brief 10進数の整数の桁数を返す
    template <typename T>
    int CountDigit(T number)
    {
        int count=0;
        while(number>0)
        {
          number/=10;
          count++;
        }
        return count;
    }

  };
} // namespace LPT
#endif
