#ifndef LPT_PARTICLE_OUTPUT_H
#define LPT_PARTICLE_OUTPUT_H
#include <list>
#include <string>
#include "FileManager.h"
#include "FileOutput.h"

//ParticleData.hは粒子データフォーマットのバージョン番号
//(マクロ変数 PARTICLE_DATA_FORMAT_VERSION)を取得するためにincludeしている
#include "ParticleData.h"

namespace LPT
{

  //! @brief LPT native binary形式で粒子データを出力する
  class LPT_ParticleOutput:public FileOutput
  {
    /// Byte Order Mark (big/little Endian判別用のデータ) を格納する配列
    int BOM[4];

    ///  出力するParticleDataのバージョンを格納する変数
    double ParticleDataFormatVersion;

    /// REAL_TYPEのサイズを格納する変数
    size_t SizeOfRealType;

  private:
    //Singletonパターンを適用
      LPT_ParticleOutput()
    {
    };
      LPT_ParticleOutput(const FileOutput & obj);
      LPT_ParticleOutput & operator=(const FileOutput & obj);
  public:
    static LPT_ParticleOutput *GetInstance()
    {
      static LPT_ParticleOutput instance;
      static bool initialized = false;
      if(!initialized)
      {
        initialized = true;
        instance.Out2.open((FileManager::GetInstance()->GetFileName("prt")).c_str(), std::ios::binary);
        instance.Particles = NULL;
        instance.BOM[0] = 0xff;
        instance.BOM[1] = 0xef;
        instance.BOM[2] = 0xff;
        instance.BOM[3] = 0xef;
        instance.ParticleDataFormatVersion = PARTICLE_DATA_FORMAT_VERSION;
        instance.SizeOfRealType = sizeof(REAL_TYPE);
      }
      return &instance;
    }

    void WriteFileHeader();
    void WriteRecordHeader();
    void WriteRecord();
    void SetParticles(std::list < PPlib::ParticleData * >*Particles)
    {
      this->Particles = Particles;
    };
  };

} //namespace LPT
#endif
