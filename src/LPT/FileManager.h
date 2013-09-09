#ifndef LPT_FILE_MANAGER_H
#define LPT_FILE_MANAGER_H

#include <iostream>
#include <string>
#include <map>
#include <mpi.h>

namespace LPT
{
  //! @brief LPTが入/出力するファイルのstreamを生成/保持するクラス
  //! InputまたはOutputのどちらかでしかファイルストリームを開けないようにしている
  class FileManager
  {
    //! @brief ファイル入出力メソッドで使われるファイル名のprefix
    //! この後にRank番号を付け加えたものが実際のファイル名となる
    static std::string BaseFileName;

  private:
    //Singletonパターンを適用
    FileManager()
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
      static bool initialized = false;
      if(!initialized)
      {
        initialized = true;
        BaseFileName = "ParticleData";
      }
      return &instance;
    }

    /// 引数で指定したラベルに対応したファイル名を返す
    std::string GetFileName(const std::string & suffix);

    ///Accessor
    std::string GetBaseFileName(void)
    {
      return this->BaseFileName;
    };
    void SetBaseFileName(std::string BaseFileName)
    {
      this->BaseFileName = BaseFileName;
    };
  };
} // namespace LPT
#endif
