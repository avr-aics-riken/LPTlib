#ifndef LPT_FILE_MANAGER_H
#define LPT_FILE_MANAGER_H

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <mpi.h>

namespace LPT
{
  //! @brief LPTが入/出力するファイルの名前を生成するクラス
  class FileManager
  {
  private:
    //Singletonパターンを適用
    FileManager() : BaseFileName("ParticleOutput"), MaxTimeStep(1000)
    {
    }
    FileManager(const FileManager & obj);
    FileManager & operator=(const FileManager & obj);
    ~FileManager() {}

  public:
    static FileManager *GetInstance()
    {
      static FileManager instance;
      return &instance;
    }

    /// 引数で指定した拡張子に対応したタイムステップ無しのファイル名を返す
    std::string GetFileName(const std::string & suffix)
    {
      std::ostringstream oss;
      oss << GetBaseFileName();
      int nproc;
      MPI_Comm_size(MPI_COMM_WORLD, &nproc);
      if(nproc > 1)
      {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        oss << std::setfill('0') <<std::setw(CountDigit(nproc)) <<my_rank << "_";
      }
      oss << "." << suffix;
      return oss.str();
    }

    /// 引数で指定した拡張子に対応したタイムステップ付きのファイル名を返す
    std::string GetFileNameWithTimeStep(const std::string & suffix, const int & timestep)
    {
      int nproc;
      MPI_Comm_size(MPI_COMM_WORLD, &nproc);

      std::ostringstream oss;
      oss << GetBaseFileName() << "_";
      if(nproc > 1)
      {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        oss << std::setfill('0') <<std::setw(CountDigit(nproc)) <<my_rank << "_";
      }
      oss << std::setfill('0') <<std::setw(CountDigit(MaxTimeStep)) << timestep << "." << suffix;
      return oss.str();
    }

    ///Accessor
    std::string GetBaseFileName(void) { return this->BaseFileName; };
    void SetBaseFileName(std::string BaseFileName) { this->BaseFileName = BaseFileName; };
    void SetMaxTimeStep(long MaxTimeStep) { this->MaxTimeStep=MaxTimeStep; }

  private:
    //! @brief ファイル入出力メソッドで使われるファイル名のprefix
    //!
    //! この後にRank番号を付け加えたものが実際のファイル名となる
    std::string BaseFileName;

    //! @brief GetFileNameWithTimeStep()で使うタイムステップの最大値
    long MaxTimeStep;

    //! @brief 10進数の整数の桁数を返す
    template <typename T>
    int CountDigit(T number){return (int)std::log10(number)+1;}

  };
} // namespace LPT
#endif
