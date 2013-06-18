#ifndef LPT_LOG_OUTPUT_H
#define LPT_LOG_OUTPUT_H
#include <iostream>
#include <fstream>
#include <string>
#include "FileManager.h"

namespace LPT
{
  //! @brief メッセージ出力を行なうクラス
  class LPT_LOG
  {
    std::ofstream logfile;
    bool FileFlushFlag;
  private:
    //Singletonパターンを適用
      LPT_LOG():FileFlushFlag(false)
    {
      logfile.open((FileManager::GetInstance()->GetFileName("log")).c_str());
    }
    LPT_LOG(const FileManager & obj);
      LPT_LOG & operator=(const FileManager & obj);
  public:
    static LPT_LOG *GetInstance()
    {
      static LPT_LOG instance;
      static bool initialized = false;
      if(!initialized)
      {
        initialized = true;
      }
      return &instance;
    }
    void enableFileFlushFlag(void)
    {
      this->FileFlushFlag = true;
    }
    void disableFileFlushFlag(void)
    {
      this->FileFlushFlag = false;
    }
    bool isFileFlushFlag(void)
    {
      return this->FileFlushFlag;
    }

    void LOG(std::string message)
    {
#ifdef LPT_LOG_ENABLE
      logfile << "LPT LOG  : " << message << "\n";
      if(FileFlushFlag)
        logfile.flush();
#endif
    }
    template < class T > void LOG(std::string message, T value)
    {
#ifdef LPT_LOG_ENABLE
      logfile << "LPT LOG  : " << message << value << "\n";
      if(FileFlushFlag)
        logfile.flush();
#endif
    }
    template < class T > void LOG(std::string message, T * value, int max)
    {
#ifdef LPT_LOG_ENABLE
      logfile << "LPT LOG  : " << message;
      for(int i = 0; i < max - 1; i++) {
        logfile << value[i] << ",";
      }
      logfile << value[max - 1] << "\n";

      if(FileFlushFlag)
        logfile.flush();
#endif
    }

    void INFO(std::string message)
    {
      logfile << "LPT INFO : " << message << "\n";
      if(FileFlushFlag)
        logfile.flush();
    }

    template < class T > void INFO(std::string message, T value)
    {
      logfile << "LPT INFO : " << message << value << "\n";
      if(FileFlushFlag)
        logfile.flush();
    }
    template < class T > void INFO(std::string message, T * value, int max)
    {
      logfile << "LPT INFO : " << message;
      for(int i = 0; i < max - 1; i++) {
        logfile << value[i] << ",";
      }
      logfile << value[max - 1] << "\n";
      if(FileFlushFlag)
        logfile.flush();
    }

    void WARN(std::string message)
    {
      logfile << "LPT WARN : " << message << "\n";
      if(FileFlushFlag)
        logfile.flush();
    }
    template < class T > void WARN(std::string message, T value)
    {
      logfile << "LPT WARN : " << message << value << "\n";
      if(FileFlushFlag)
        logfile.flush();
    }

    void ERROR(std::string message)
    {
      logfile << "LPT ERROR: " << message << "\n";
      logfile.flush();
    }
    template < class T > void ERROR(std::string message, T value)
    {
      logfile << "LPT ERROR : " << message << value << "\n";
      logfile.flush();
    }
  };
} // namespace LPT
#endif
