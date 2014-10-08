/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include <iostream>

#include "Rectangle.h"
#include "Utility.h"
#include "LPT_LogOutput.h"
#include "SimpleStartPointFactory.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, const Rectangle& obj)
  {
    stream << obj.TextPrint(stream);
    return stream;
  }

  std::istream & operator >>(std::istream & stream, Rectangle & obj)
  {
    stream >> obj.Coord1[0] >> obj.Coord1[1] >> obj.Coord1[2];
    stream >> obj.Coord2[0] >> obj.Coord2[1] >> obj.Coord2[2];
    stream >> obj.SumStartPoints;
    stream >> obj.NumStartPoints[0] >> obj.NumStartPoints[1] >> obj.NumStartPoints[2];
    stream >> obj.StartTime >> obj.ReleaseTime >> obj.TimeSpan >> obj.ParticleLifeTime;

    return stream;
  }

  void Rectangle::MakeCoord3_4(REAL_TYPE Coord3[3], int *NumPoints1, REAL_TYPE Coord4[3], int *NumPoints2)
  {
    if(Coord1[0] == Coord2[0]) {
      Coord3[0] = Coord1[0];
      Coord3[1] = Coord2[1];
      Coord3[2] = Coord1[2];

      Coord4[0] = Coord1[0];
      Coord4[1] = Coord1[1];
      Coord4[2] = Coord2[2];

      *NumPoints1 = NumStartPoints[1];
      *NumPoints2 = NumStartPoints[2];
    } else if(Coord1[1] == Coord2[1]) {
      Coord3[0] = Coord1[0];
      Coord3[1] = Coord1[1];
      Coord3[2] = Coord2[2];

      Coord4[0] = Coord2[0];
      Coord4[1] = Coord1[1];
      Coord4[2] = Coord1[2];

      *NumPoints1 = NumStartPoints[2];
      *NumPoints2 = NumStartPoints[0];
    } else if(Coord1[2] == Coord2[2]) {
      Coord3[0] = Coord2[0];
      Coord3[1] = Coord1[1];
      Coord3[2] = Coord1[2];

      Coord4[0] = Coord1[0];
      Coord4[1] = Coord2[1];
      Coord4[2] = Coord1[2];

      *NumPoints1 = NumStartPoints[0];
      *NumPoints2 = NumStartPoints[1];
    } else {
      LPT::LPT_LOG::GetInstance()->ERROR("illeagal Coords");
    }
    LPT::LPT_LOG::GetInstance()->LOG("Coord3 = ", Coord3, 3);
    LPT::LPT_LOG::GetInstance()->LOG("Coord4 = ", Coord4, 3);
    LPT::LPT_LOG::GetInstance()->LOG("NumPoints1 = ", *NumPoints1);
    LPT::LPT_LOG::GetInstance()->LOG("NumPoints2 = ", *NumPoints2);
  }

  void Rectangle::GetGridPointCoord(std::vector < REAL_TYPE > &Coords)
  {
    std::vector<REAL_TYPE> coord_x;
    utility::DivideLine1D(&coord_x, NumStartPoints[0], Coord1[0], Coord2[0]);
    std::vector<REAL_TYPE> coord_y;
    utility::DivideLine1D(&coord_y, NumStartPoints[1], Coord1[1], Coord2[1]);
    std::vector<REAL_TYPE> coord_z;
    utility::DivideLine1D(&coord_z, NumStartPoints[2], Coord1[2], Coord2[2]);

    for(std::vector<REAL_TYPE>::iterator it_x = coord_x.begin(); it_x != coord_x.end(); ++it_x)
    {
      for(std::vector<REAL_TYPE>::iterator it_y = coord_y.begin(); it_y != coord_y.end(); ++it_y)
      {
        for(std::vector<REAL_TYPE>::iterator it_z = coord_z.begin(); it_z != coord_z.end(); ++it_z)
        {
          Coords.push_back(*it_x);
          Coords.push_back(*it_y);
          Coords.push_back(*it_z);
        }
      }
    }
    LPT::LPT_LOG::GetInstance()->LOG("Number of grid points = ", Coords.size()/3);
  }

  void Rectangle::Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints)
  {
    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピーを格納したvectorを返す
    if(MaxNumStartPoints >= GetSumStartPoints()) {
      Rectangle *NewRectangel = new Rectangle(*this);

      StartPoints->push_back(NewRectangel);
      return;
    } else if(MaxNumStartPoints <= 0){
      // MaxNumStartPointsが0以下の時はエラーメッセージを出力して終了
      LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
      return;
    }

    //残りの2頂点の座標と各辺上の点数を決める
    int N;  //Coord1とCoord3の間の開始点数
    int M;  //Coord1とCoord4の間の開始点数
    REAL_TYPE Coord3[3];
    REAL_TYPE Coord4[3];
    MakeCoord3_4(Coord3, &N, Coord4, &M);

    // N*Mの格子点をだいたい同じくらいの点数が含まれるようにブロック分割する

    //MaxNumStartPointsを (2^Pow[0] * 3^Pow[1] * 5^Pow[2] * Rem) の形に因数分解
    int Pow[4];
    utility::Factorize235(MaxNumStartPoints, Pow);

    //分割後の小領域のサイズを決める
    int NB = pow(2, (Pow[0] / 2 + Pow[0] % 2)) * pow(3, (Pow[1] / 2 + Pow[1] % 2)) * pow(5, (Pow[2] / 2));
    int MB = pow(2, (Pow[0] / 2))              * pow(3, (Pow[1] / 2))              * pow(5, (Pow[2] / 2 + Pow[2] % 2));

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

      std::vector < REAL_TYPE > tmpCoords;
      if(Coord1[0] == Coord2[0]) {
        DividePoints(&tmpCoords, N, Coord1, Coord3);

        NumStartPoints[1] = (N / NB) * NB;
        Coord2[1] = tmpCoords[3*((N / NB) * NB - 1)+1];

        (ReminderN->NumStartPoints)[1] = N % NB;
        (ReminderN->Coord1)[1] = tmpCoords[3*((N / NB) * NB)];
      } else if(Coord1[1] == Coord2[1]) {
        DividePoints(&tmpCoords, N, Coord1, Coord3);

        NumStartPoints[2] = (N / NB) * NB;
        Coord2[2] = tmpCoords[3*((N / NB) * NB - 1)+2];

        (ReminderN->NumStartPoints)[2] = N % NB;
        (ReminderN->Coord1)[2] = tmpCoords[3*((N / NB) * NB)];
      } else if(Coord1[2] == Coord2[2]) {
        DividePoints(&tmpCoords, N, Coord1, Coord3);

        NumStartPoints[0] = (N / NB) * NB;
        Coord2[0] = tmpCoords[3*((N / NB) * NB - 1)];

        (ReminderN->NumStartPoints)[0] = N % NB;
        (ReminderN->Coord1)[0] = tmpCoords[3*((N / NB) * NB)];
      }
      ReminderN->SetSumStartPoints((ReminderN->NumStartPoints)[0] * (ReminderN->NumStartPoints)[1] * (ReminderN->NumStartPoints)[2]);
      StartPoints->push_back(ReminderN);
      LPT::LPT_LOG::GetInstance()->LOG("ReminderN = ", ReminderN);
    }
    //この時点で、thisオブジェクトのサイズ(開始点数)は (N/NB)*NB * M となっている

    //M方向の余り領域を分割してStartPointsに格納
    if(M % MB != 0) {
      Rectangle *ReminderM = new Rectangle(*this);

      std::vector < REAL_TYPE > tmpCoords;
      DividePoints(&tmpCoords, M, Coord1, Coord4);

      if(Coord1[0] == Coord2[0]) {
        NumStartPoints[2] = (M / MB) * MB;
        Coord2[2] = tmpCoords[3*((M / MB) * MB - 1)+2];
        (ReminderM->NumStartPoints)[2] = M % MB;
        (ReminderM->Coord1)[2] = tmpCoords[3*((M / MB) * MB)];
      } else if(Coord1[1] == Coord2[1]) {
        NumStartPoints[0] = (M / MB) * MB;
        Coord2[0] = tmpCoords[3*((M / MB) * MB - 1)];
        (ReminderM->NumStartPoints)[0] = M % MB;
        (ReminderM->Coord1)[0] = tmpCoords[3*((M / MB) * MB)];
      } else if(Coord1[2] == Coord2[2]) {
        NumStartPoints[1] = (M / MB) * MB;
        Coord2[1] = tmpCoords[3*((M / MB) * MB - 1)+1];
        (ReminderM->NumStartPoints)[1] = M % MB;
        (ReminderM->Coord1)[1] = tmpCoords[3*((M / MB) * MB)+1];
      }

      ReminderM->SetSumStartPoints((ReminderM->NumStartPoints)[0] * (ReminderM->NumStartPoints)[1] * (ReminderM->NumStartPoints)[2]);
      StartPoints->push_back(ReminderM);
      LPT::LPT_LOG::GetInstance()->LOG("ReminderM = ", ReminderM);
    }


    //N方向をNBpointsづつのRectangleに分割
    std::vector < Rectangle * >tmpStartPoint;
    int NumDivide = N / NB;

    for(int i = 0; i < NumDivide; i++) {
      Rectangle *tmpRectangle = new Rectangle(*this);

      std::vector < REAL_TYPE > tmpCoords;
      MakeCoord3_4(Coord3, &N, Coord4, &M);
      DividePoints(&tmpCoords, N, Coord1, Coord3);
      N = N - NB;
      if(Coord1[0] == Coord2[0]) {
        if(N > 0) {
          NumStartPoints[1] = N;
          Coord2[1] = tmpCoords[3*(N - 1)+1];
        }
        (tmpRectangle->NumStartPoints)[1] = NB;
        (tmpRectangle->Coord1)[1] = tmpCoords[3*N+1];
      } else if(Coord1[1] == Coord2[1]) {
        if(N > 0) {
          NumStartPoints[2] = N;
          Coord2[2] = tmpCoords[3*(N - 1)+2];
        }
        (tmpRectangle->NumStartPoints)[2] = NB;
        (tmpRectangle->Coord1)[2] = tmpCoords[3*N+2];
      } else if(Coord1[2] == Coord2[2]) {
        if(N > 0) {
          NumStartPoints[0] = N;
          Coord2[0] = tmpCoords[3*(N - 1)];
        }
        (tmpRectangle->NumStartPoints)[0] = NB;
        (tmpRectangle->Coord1)[0] = tmpCoords[3*N];
      }
      tmpStartPoint.push_back(tmpRectangle);
    }

    //分割した個々のオブジェクトをさらにM方向にMpointsづつに分割
    int orgM = M;

    for(std::vector < Rectangle * >::iterator it = tmpStartPoint.begin(); it != tmpStartPoint.end(); ++it) {
      M = orgM;
      for(int i = 0; i < orgM / MB; i++) {
        Rectangle *tmpRectangle = new Rectangle(**it);

        std::vector < REAL_TYPE > tmpCoords;
        (*it)->MakeCoord3_4(Coord3, &N, Coord4, &M);
        DividePoints(&tmpCoords, M, (*it)->Coord1, Coord4);
        M = M - MB;

        if(Coord1[0] == Coord2[0]) {
          if(M > 0) {
            ((*it)->NumStartPoints)[2] = M;
            ((*it)->Coord2)[2] = tmpCoords[3*(M - 1)+2];
          }
          (tmpRectangle->NumStartPoints)[2] = MB;
          (tmpRectangle->Coord1)[2] = tmpCoords[3*M+2];
        } else if(Coord1[1] == Coord2[1]) {
          if(M > 0) {
            ((*it)->NumStartPoints)[0] = M;
            ((*it)->Coord2)[0] = tmpCoords[3*(M - 1)];
          }
          (tmpRectangle->NumStartPoints)[0] = MB;
          (tmpRectangle->Coord1)[0] = tmpCoords[3*M];
        } else if(Coord1[2] == Coord2[2]) {
          if(M > 0) {
            ((*it)->NumStartPoints)[1] = M;
            ((*it)->Coord2)[1] = tmpCoords[3*(M - 1)+1];
          }
          (tmpRectangle->NumStartPoints)[1] = MB;
          (tmpRectangle->Coord1)[1] = tmpCoords[3*M+1];
        }
        tmpRectangle->SetSumStartPoints((tmpRectangle->NumStartPoints)[0] * (tmpRectangle->NumStartPoints)[1] * (tmpRectangle->NumStartPoints)[2]);
        StartPoints->push_back(tmpRectangle);
      }
    }

    return;
  }

} // namespace PPlib
