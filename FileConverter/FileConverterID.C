#include "FileConverter.h"

void FileConverterID::WriteBody(std::list < std::ifstream * >&InputFileStream, LPT::FileOutput & FileOutput)
{
  //IDリストの作成
  std::set < LPT::ID > IDs;
  for(std::list < std::ifstream * >::iterator it = InputFileStream.begin(); it != InputFileStream.end(); it++)
  {
    LPT::LPT_ParticleInput Input(*it);
    Input.ReadFileHeader();
    Input.ReadIDs(&IDs);
  }

  //ID毎にファイルから読み込んで、出力
  for(std::set < LPT::ID >::iterator it_key = IDs.begin(); it_key != IDs.end(); it_key++)
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
    Particles.sort(PPlib::CompareTimeStep());

    //fvp形式で出力
    FileOutput.SetParticles(&Particles);
    FileOutput.WriteRecordHeader();
    FileOutput.WriteRecord();
    FileOutput.WriteRecordFooter();
  }
}
