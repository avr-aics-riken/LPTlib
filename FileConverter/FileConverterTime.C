#include "FileConverter.h"

void FileConverterTime::WriteBody(std::list < std::ifstream * >&InputFileStream, LPT::FileOutput & FileOutput)
{
  //TimeStepリストの作成
  std::set < unsigned int >Key;

  for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
  {
    LPT::LPT_ParticleInput Input(*it);
    Input.ReadFileHeader();
    Input.ReadTimeSteps(&Key);
  }

  //Time Step毎にファイルから読み込んで、出力
  for(std::set < unsigned int >::iterator it_key = Key.begin(); it_key != Key.end(); it_key++)
  {
    //バイナリデータの読み込み
    std::list < PPlib::ParticleData * >Particles;
    int iFile = 0;

    for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
    {
      iFile++;
      (*it)->clear();
      (*it)->seekg(0);
      LPT::LPT_ParticleInput Input(*it);
      Input.SetParticles(&Particles);
      Input.ReadFileHeader();
      Input.ReadRecord(*it_key);
    }
    //ID順にソート
    Particles.sort(PPlib::CompareID());

    //fvp形式で出力
    FileOutput.SetParticles(&Particles);
    FileOutput.WriteRecordHeader();
    FileOutput.WriteRecord();
    FileOutput.WriteRecordFooter();
  }
}
