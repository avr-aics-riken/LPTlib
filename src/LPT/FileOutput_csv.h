#ifndef CSV_OUTPUT_H
#define CSV_OUTPUT_H

#include <list>
#include "FileManager.h"
#include "FileOutput.h"

namespace LPT
{
  //!  @brief CSV形式(カンマ区切り)で粒子データを出力するクラス
  class CSV_Output:public FileOutput
  {
  public:
    CSV_Output()
    {
      Out2.open((FileManager::GetInstance()->GetFileName("csv")).c_str());
    }

    void WriteFileHeader();
    void WriteRecord();
  };

} //namespace LPT
#endif
