#include "Line.h"
#include "ParticleData.h"
#include "SimpleStartPointFactory.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, const Line& obj)
  {
    stream << obj.TextPrint(stream);
    return stream;
  }

  std::istream & operator >>(std::istream & stream, Line & obj)
  {
    stream >> obj.Coord1[0] >> obj.Coord1[1] >> obj.Coord1[2];
    stream >> obj.Coord2[0] >> obj.Coord2[1] >> obj.Coord2[2];
    stream >> obj.SumStartPoints;
    stream >> obj.StartTime >> obj.ReleaseTime >> obj.TimeSpan >> obj.ParticleLifeTime;

    return stream;
  }

  void Line::GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)
  {
    DSlib::DV3 DV3Coord1(Coord1[0], Coord1[1], Coord1[2]);
    DSlib::DV3 DV3Coord2(Coord2[0], Coord2[1], Coord2[2]);
    DividePoints(&Coords, GetSumStartPoints(), DV3Coord1, DV3Coord2);
  }

  void Line::Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints)
  {
    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピーを格納したvectorを返す
    if(MaxNumStartPoints >= GetSumStartPoints()) {
      Line *NewLine = new Line(*this);

      StartPoints->push_back(NewLine);
      return;
    } else if(MaxNumStartPoints <= 0){
      // MaxNumStartPointsが0以下の時はエラーメッセージを出力して終了
      LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
      return;
    }

    //余りオブジェクトが持つ格子点数
    int NumReminder = GetSumStartPoints() % MaxNumStartPoints;

    //余りオブジェクトを除いた分割後のオブジェクトの数
    int NumParts = GetSumStartPoints() / MaxNumStartPoints;

    //分割後のオブジェクトが持つ格子点数
    int NumGridPoints = NumParts > 1 ? (GetSumStartPoints() - NumReminder) / NumParts : -1;

    //分割前のオブジェクトの開始点座標を取得し、先頭から順にNumGridPoints個毎に新しいオブジェクトを作って
    //StartPointsにpush_backする
    std::vector < DSlib::DV3 > Coords;
    this->GetGridPointCoord(Coords);
    std::vector < DSlib::DV3 >::iterator itCoords = Coords.begin();
    for(int i = 0; i < NumParts; i++)
    {
      REAL_TYPE Coord1[3] = { (*itCoords).x, (*itCoords).y, (*itCoords).z };
      itCoords += NumGridPoints - 1;
      REAL_TYPE Coord2[3] = { (*itCoords).x, (*itCoords).y, (*itCoords).z };
      ++itCoords;

      StartPoints->push_back(LineFactory::create(Coord1, Coord2, NumGridPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }

    if(NumReminder != 0)
    {
      REAL_TYPE Coord1[3] = { (*itCoords).x, (*itCoords).y, (*itCoords).z };
      StartPoints->push_back(LineFactory::create(Coord1, this->Coord2, NumReminder, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }
    return;
  }

} // namespace PPlib
