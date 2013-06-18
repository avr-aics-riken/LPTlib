#include <iostream>

#include "Rectangle.h"
#include "Utility.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, Rectangle & obj)
  {
    stream << "Coord1          = " << obj.Coord1[0] << "," << obj.Coord1[1] << "," << obj.Coord1[2] << std::endl;
    stream << "Coord2          = " << obj.Coord2[0] << "," << obj.Coord2[1] << "," << obj.Coord2[2] << std::endl;
    stream << "NumStartPoints  = " << obj.NumStartPoints[0] << "," << obj.NumStartPoints[1] << "," << obj.NumStartPoints[2] << std::endl;
    stream << "StartTime       = " << obj.StartTime << std::endl;
    stream << "ReleaseTime     = " << obj.ReleaseTime << std::endl;
    stream << "TimeSpan        = " << obj.TimeSpan << std::endl;
    stream << "LatestEmitTime  = " << obj.LatestEmitTime << std::endl;
    stream << "ID              = " << obj.ID[0] << "," << obj.ID[1] << std::endl;
    return stream;
  }

  std::istream & operator >>(std::istream & stream, Rectangle & obj)
  {
    stream >> obj.Coord1[0] >> obj.Coord1[1] >> obj.Coord1[2];
    stream >> obj.Coord2[0] >> obj.Coord2[1] >> obj.Coord2[2];
    stream >> obj.NumStartPoints[0] >> obj.NumStartPoints[1] >> obj.NumStartPoints[2];
    stream >> obj.StartTime >> obj.ReleaseTime >> obj.TimeSpan >> obj.ParticleLifeTime;

    return stream;
  }

  void Rectangle::MakeCoord3_4(DSlib::DV3 * Coord3, int *NumPoints1, DSlib::DV3 * Coord4, int *NumPoints2)
  {
    if(Coord1[0] == Coord2[0]) {
      Coord3->x = Coord1[0];
      Coord3->y = Coord2[1];
      Coord3->z = Coord1[2];

      Coord4->x = Coord1[0];
      Coord4->y = Coord1[1];
      Coord4->z = Coord2[2];

      *NumPoints1 = NumStartPoints[1];
      *NumPoints2 = NumStartPoints[2];
    } else if(Coord1[1] == Coord2[1]) {
      Coord3->x = Coord1[0];
      Coord3->y = Coord1[1];
      Coord3->z = Coord2[2];

      Coord4->x = Coord2[0];
      Coord4->y = Coord1[1];
      Coord4->z = Coord1[2];

      *NumPoints1 = NumStartPoints[2];
      *NumPoints2 = NumStartPoints[0];
    } else if(Coord1[2] == Coord2[2]) {
      Coord3->x = Coord2[0];
      Coord3->y = Coord1[1];
      Coord3->z = Coord1[2];

      Coord4->x = Coord1[0];
      Coord4->y = Coord2[1];
      Coord4->z = Coord1[2];

      *NumPoints1 = NumStartPoints[0];
      *NumPoints2 = NumStartPoints[1];
    } else {
      LPT::LPT_LOG::GetInstance()->ERROR("illeagal Coords");
    }
    LPT::LPT_LOG::GetInstance()->LOG("Coord3 = ", Coord3);
    LPT::LPT_LOG::GetInstance()->LOG("Coord4 = ", Coord4);
    LPT::LPT_LOG::GetInstance()->LOG("NumPoints1 = ", *NumPoints1);
    LPT::LPT_LOG::GetInstance()->LOG("NumPoints2 = ", *NumPoints2);
  }

  void Rectangle::GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)
  {
    //残りの2頂点の座標と各辺の点数を決める
    int NumPoints1; //Coord1とCoord3の間の開始点数
    int NumPoints2; //Coord1とCoord4の間の開始点数

    DSlib::DV3 Coord3;
    DSlib::DV3 Coord4;
    MakeCoord3_4(&Coord3, &NumPoints1, &Coord4, &NumPoints2);

    //Coord1, 3間の頂点座標を求める
    std::vector < DSlib::DV3 > tmpCoords1;
    DSlib::DV3 DV3Coord1(Coord1[0], Coord1[1], Coord1[2]);
    DividePoints(&tmpCoords1, NumPoints1, DV3Coord1, Coord3);

    //Coord4, 2間の頂点座標を求める
    std::vector < DSlib::DV3 > tmpCoords2;
    DSlib::DV3 DV3Coord2(Coord2[0], Coord2[1], Coord2[2]);
    DividePoints(&tmpCoords2, NumPoints1, Coord4, DV3Coord2);

    for(int i = 0; i < NumPoints1; i++) {
      DividePoints(&Coords, NumPoints2, tmpCoords1[i], tmpCoords2[i]);
    }
    LPT::LPT_LOG::GetInstance()->LOG("Number of grid points = ", Coords.size());
  }

  std::vector < StartPoint * >*Rectangle::Divider(const int &AveNumStartPoints)
  {
    std::vector < StartPoint * >*StartPoints = new std::vector < StartPoint * >;

    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピーを格納したvectorを返す
    if(AveNumStartPoints >= GetSumStartPoints()) {
      Rectangle *NewRectangel = new Rectangle(*this);

      StartPoints->push_back(NewRectangel);
      return StartPoints;
    }

    int N;  //Coord1とCoord3の間の開始点数
    int M;  //Coord1とCoord4の間の開始点数

    DSlib::DV3 Coord3(Coord3[0], Coord3[1], Coord3[2]);
    DSlib::DV3 Coord4(Coord4[0], Coord4[1], Coord4[2]);
    MakeCoord3_4(&Coord3, &N, &Coord4, &M);

    //AveNumStartPointsを (2^Pow[0] * 3^Pow[1] * 5^Pow[2] * Rem) の形に因数分解
    int Pow[4];

    Factorize235(AveNumStartPoints, Pow);

    //分割後の小領域のサイズを決める
    int NB = pow(2, (Pow[0] / 2 + Pow[0] % 2)) * pow(3, (Pow[1] / 2 + Pow[1] % 2)) * pow(5, (Pow[2] / 2));
    int MB = pow(2, (Pow[0] / 2)) * pow(3, (Pow[1] / 2)) * pow(5, (Pow[2] / 2 + Pow[2] % 2));

    LPT::LPT_LOG::GetInstance()->LOG("initial NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("initial MB = ", MB);

    //NB>Nとなっていた場合に、NBを減らしてMBを増やす
    //NB < 2*N となるように調整
    while(NB / N >= 2) {
      if(NB / N >= 5 && Pow[2] > 0) {
        NB /= 5;
        MB *= 5;
      } else if(NB / N >= 3 && Pow[1] > 0) {
        NB /= 3;
        MB *= 3;
      } else if(NB / N >= 2 && Pow[0] > 0) {
        NB /= 2;
        MB *= 2;
      }
    }
    //最後に2か3か5かのどれかで1回割る
    if(NB > N && NB % 2 == 0) {
      NB /= 2;
      MB *= 2;
    } else if(NB > N && NB % 3 == 0) {
      NB /= 3;
      MB *= 3;
    } else if(NB > N && NB % 5 == 0) {
      NB /= 5;
      MB *= 5;
    }
    //MB>Mとなっていた場合に、MBを減らしてNBを増やす
    while(MB / M >= 2) {
      if(MB / M >= 5 && Pow[2] > 0) {
        MB /= 5;
        NB *= 5;
      } else if(MB / M >= 3 && Pow[1] > 0) {
        MB /= 3;
        NB *= 3;
      } else if(MB / M >= 2 && Pow[0] > 0) {
        MB /= 2;
        NB *= 2;
      }
    }
    //最後に2か3か5かのどれかで1回割る
    if(MB > M && MB % 2 == 0) {
      MB /= 2;
      NB *= 2;
    } else if(MB > M && MB % 3 == 0) {
      MB /= 3;
      NB *= 3;
    } else if(MB > M && MB % 5 == 0) {
      MB /= 5;
      NB *= 5;
    }

    LPT::LPT_LOG::GetInstance()->LOG("NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("MB = ", MB);

    //N方向の余り領域を分割してStartPointsに格納
    if(N % NB != 0) {
      Rectangle *ReminderN = new Rectangle(*this);

      std::vector < DSlib::DV3 > tmpCoords;
      DSlib::DV3 DV3Coord1(Coord1[0], Coord1[1], Coord1[2]);

      if(Coord1[0] == Coord2[0]) {
        DividePoints(&tmpCoords, N, DV3Coord1, Coord3);

        NumStartPoints[1] = (N / NB) * NB;
        Coord2[1] = tmpCoords[(N / NB) * NB - 1].y;

        (ReminderN->NumStartPoints)[1] = N % NB;
        (ReminderN->Coord1)[1] = tmpCoords[(N / NB) * NB].y;
      } else if(Coord1[1] == Coord2[1]) {
        DividePoints(&tmpCoords, N, DV3Coord1, Coord3);

        NumStartPoints[2] = (N / NB) * NB;
        Coord2[2] = tmpCoords[(N / NB) * NB - 1].z;

        (ReminderN->NumStartPoints)[2] = N % NB;
        (ReminderN->Coord1)[2] = tmpCoords[(N / NB) * NB].z;
      } else if(Coord1[2] == Coord2[2]) {
        DividePoints(&tmpCoords, N, DV3Coord1, Coord3);

        NumStartPoints[0] = (N / NB) * NB;
        Coord2[0] = tmpCoords[(N / NB) * NB - 1].x;

        (ReminderN->NumStartPoints)[0] = N % NB;
        (ReminderN->Coord1)[0] = tmpCoords[(N / NB) * NB].x;
      }
      ReminderN->SetSumStartPoints((ReminderN->NumStartPoints)[0] * (ReminderN->NumStartPoints)[1] * (ReminderN->NumStartPoints)[2]);
      StartPoints->push_back(ReminderN);
      LPT::LPT_LOG::GetInstance()->LOG("ReminderN = ", ReminderN);
    }
    //この時点で、thisオブジェクトのサイズ(開始点数)は (N/NB)*NB * M となっている

    //M方向の余り領域を分割してStartPointsに格納
    if(M % MB != 0) {
      Rectangle *ReminderM = new Rectangle(*this);

      std::vector < DSlib::DV3 > tmpCoords;
      DSlib::DV3 DV3Coord1(Coord1[0], Coord1[1], Coord1[2]);
      DividePoints(&tmpCoords, M, DV3Coord1, Coord4);

      if(Coord1[0] == Coord2[0]) {
        NumStartPoints[2] = (M / MB) * MB;
        Coord2[2] = tmpCoords[(M / MB) * MB - 1].z;
        (ReminderM->NumStartPoints)[2] = M % MB;
        (ReminderM->Coord1)[2] = tmpCoords[(M / MB) * MB].z;
      } else if(Coord1[1] == Coord2[1]) {
        NumStartPoints[0] = (M / MB) * MB;
        Coord2[0] = tmpCoords[(M / MB) * MB - 1].x;
        (ReminderM->NumStartPoints)[0] = M % MB;
        (ReminderM->Coord1)[0] = tmpCoords[(M / MB) * MB].x;
      } else if(Coord1[2] == Coord2[2]) {
        NumStartPoints[1] = (M / MB) * MB;
        Coord2[1] = tmpCoords[(M / MB) * MB - 1].y;
        (ReminderM->NumStartPoints)[1] = M % MB;
        (ReminderM->Coord1)[1] = tmpCoords[(M / MB) * MB].y;
      }

      ReminderM->SetSumStartPoints((ReminderM->NumStartPoints)[0] * (ReminderM->NumStartPoints)[1] * (ReminderM->NumStartPoints)[2]);
      StartPoints->push_back(ReminderM);
      LPT::LPT_LOG::GetInstance()->LOG("ReminderM = ", ReminderM);
    }
    //N方向をNpointsづつのRectangleに分割
    std::vector < Rectangle * >tmpStartPoint;
    int NumDivide = N / NB;

    for(int i = 0; i < NumDivide; i++) {
      Rectangle *tmpRectangle = new Rectangle(*this);

      std::vector < DSlib::DV3 > tmpCoords;
      DSlib::DV3 DV3Coord1(Coord1[0], Coord1[1], Coord1[2]);

      MakeCoord3_4(&Coord3, &N, &Coord4, &M);
      DividePoints(&tmpCoords, N, DV3Coord1, Coord3);
      N = N - NB;
      if(Coord1[0] == Coord2[0]) {
        if(N > 0) {
          NumStartPoints[1] = N;
          Coord2[1] = tmpCoords[N - 1].y;
        }
        (tmpRectangle->NumStartPoints)[1] = NB;
        (tmpRectangle->Coord1)[1] = tmpCoords[N].y;
      } else if(Coord1[1] == Coord2[1]) {
        if(N > 0) {
          NumStartPoints[2] = N;
          Coord2[2] = tmpCoords[N - 1].z;
        }
        (tmpRectangle->NumStartPoints)[2] = NB;
        (tmpRectangle->Coord1)[2] = tmpCoords[N].z;
      } else if(Coord1[2] == Coord2[2]) {
        if(N > 0) {
          NumStartPoints[0] = N;
          Coord2[0] = tmpCoords[N - 1].x;
        }
        (tmpRectangle->NumStartPoints)[0] = NB;
        (tmpRectangle->Coord1)[0] = tmpCoords[N].x;
      }
      tmpStartPoint.push_back(tmpRectangle);
    }

    //分割した個々のオブジェクトをさらにM方向にMpointsづつに分割
    int orgM = M;

    for(std::vector < Rectangle * >::iterator it = tmpStartPoint.begin(); it != tmpStartPoint.end(); it++) {
      M = orgM;
      for(int i = 0; i < orgM / MB; i++) {
        Rectangle *tmpRectangle = new Rectangle(**it);

        std::vector < DSlib::DV3 > tmpCoords;
        DSlib::DV3 DV3Coord1(((*it)->Coord1)[0], ((*it)->Coord1)[1], ((*it)->Coord1)[2]);

        (*it)->MakeCoord3_4(&Coord3, &N, &Coord4, &M);
        DividePoints(&tmpCoords, M, DV3Coord1, Coord4);
        M = M - MB;

        if(Coord1[0] == Coord2[0]) {
          if(M > 0) {
            ((*it)->NumStartPoints)[2] = M;
            ((*it)->Coord2)[2] = tmpCoords[M - 1].z;
          }
          (tmpRectangle->NumStartPoints)[2] = MB;
          (tmpRectangle->Coord1)[2] = tmpCoords[M].z;
        } else if(Coord1[1] == Coord2[1]) {
          if(M > 0) {
            ((*it)->NumStartPoints)[0] = M;
            ((*it)->Coord2)[0] = tmpCoords[M - 1].x;
          }
          (tmpRectangle->NumStartPoints)[0] = MB;
          (tmpRectangle->Coord1)[0] = tmpCoords[M].x;
        } else if(Coord1[2] == Coord2[2]) {
          if(M > 0) {
            ((*it)->NumStartPoints)[1] = M;
            ((*it)->Coord2)[1] = tmpCoords[M - 1].y;
          }
          (tmpRectangle->NumStartPoints)[1] = MB;
          (tmpRectangle->Coord1)[1] = tmpCoords[M].y;
        }
        tmpRectangle->SetSumStartPoints((tmpRectangle->NumStartPoints)[0] * (tmpRectangle->NumStartPoints)[1] * (tmpRectangle->NumStartPoints)[2]);
        StartPoints->push_back(tmpRectangle);
      }
    }

    return StartPoints;
  }

} // namespace PPlib
