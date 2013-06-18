#include "Line.h"
#include "ParticleData.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, Line & obj)
  {
    stream << "Coord1          = " << obj.Coord1[0] << "," << obj.Coord1[1] << "," << obj.Coord1[2] << std::endl;
    stream << "Coord2          = " << obj.Coord2[0] << "," << obj.Coord2[1] << "," << obj.Coord2[2] << std::endl;
    stream << "SumStartPoints  = " << obj.GetSumStartPoints() << std::endl;
    stream << "StartTime       = " << obj.StartTime << std::endl;
    stream << "ReleaseTime     = " << obj.ReleaseTime << std::endl;
    stream << "TimeSpan        = " << obj.TimeSpan << std::endl;
    stream << "LatestEmitTime  = " << obj.LatestEmitTime << std::endl;
    stream << "ID              = " << obj.ID[0] << "," << obj.ID[1] << std::endl;
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

  std::vector < StartPoint * >*Line::Divider(const int &AveNumStartPoints)
  {
    std::vector < StartPoint * >*StartPoints = new std::vector < StartPoint * >;

    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピーを格納したvectorを返す
    if(AveNumStartPoints >= GetSumStartPoints()) {
      Line *NewLine = new Line(*this);

      StartPoints->push_back(NewLine);
      return StartPoints;
    }
    //余りオブジェクトが持つ格子点数
    int NumReminder = GetSumStartPoints() % AveNumStartPoints;

    //余りオブジェクトを除いた分割後のオブジェクトの数
    int NumParts = GetSumStartPoints() / AveNumStartPoints;

    //分割後のオブジェクトが持つ格子点数
    int NumGridPoints = NumParts > 1 ? (GetSumStartPoints() - NumReminder) / NumParts : -1;

    for(int i = 0; i < NumParts; i++) {
      Line *tmp = new Line;

      tmp->SetSumStartPoints(NumGridPoints);
      tmp->SetStartTime(GetStartTime());
      tmp->SetReleaseTime(GetReleaseTime());
      tmp->SetTimeSpan(GetTimeSpan());
      tmp->SetParticleLifeTime(GetParticleLifeTime());
      (*StartPoints).push_back(tmp);
    }

    std::vector < DSlib::DV3 > Coords;
    GetGridPointCoord(Coords);

    //NumGridPoints個づつCoord1から数えて、ptrLineのCoord1, Coord2に入れる
    int index = 0;

    std::vector < DSlib::DV3 >::iterator itCoords = Coords.begin();
    for(std::vector < StartPoint * >::iterator it = (*StartPoints).begin(); it != (*StartPoints).end(); it++) {
      REAL_TYPE Coord1[3] = { (*itCoords).x, (*itCoords).y, (*itCoords).z };
      (*it)->SetCoord1(Coord1);
      itCoords += NumGridPoints - 1;

      REAL_TYPE Coord2[3] = { (*itCoords).x, (*itCoords).y, (*itCoords).z };
      (*it)->SetCoord2(Coord2);
      ++itCoords;
    }
    if(NumReminder != 0) {
      Line *tmp = new Line;

      tmp->SetSumStartPoints(NumReminder);
      tmp->SetStartTime(GetStartTime());
      tmp->SetReleaseTime(GetReleaseTime());
      tmp->SetTimeSpan(GetTimeSpan());
      tmp->SetParticleLifeTime(GetParticleLifeTime());

      REAL_TYPE Coord1[3] = { (*itCoords).x, (*itCoords).y, (*itCoords).z };
      tmp->SetCoord1(Coord1);

      REAL_TYPE Coord2[3];

      GetCoord2(Coord2);
      tmp->SetCoord2(Coord2);
      (*StartPoints).push_back(tmp);
    }
    return StartPoints;
  }

} // namespace PPlib
