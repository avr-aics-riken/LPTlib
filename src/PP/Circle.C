#include <iostream>
#include <cmath>

#include "Circle.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, const Circle& obj)
  {
    stream << obj.TextPrint(stream);
    return stream;
  }

  std::istream & operator >>(std::istream & stream, Circle & obj)
  {
    stream >> obj.Coord1[0] >> obj.Coord1[1] >> obj.Coord1[2];
    stream >> obj.NormalVector[0] >> obj.NormalVector[1] >> obj.NormalVector[2];
    stream >> obj.Radius;
    stream >> obj.SumStartPoints;
    stream >> obj.StartTime >> obj.ReleaseTime >> obj.TimeSpan >>obj.LatestEmitTime>>  obj.ParticleLifeTime;

    return stream;
  }

  void Circle::NormalizeVector(REAL_TYPE * v)
  {
    REAL_TYPE length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if(length != 0.0) {
      v[0] = v[0] / length;
      v[1] = v[1] / length;
      v[2] = v[2] / length;
    }
  }

  void Circle::ConvertNormalVector(void)
  {
    //zが負の時はNormalVectorを逆向きにする
    if(NormalVector[2] < 0) {
      NormalVector[0] = -NormalVector[0];
      NormalVector[1] = -NormalVector[1];
      NormalVector[2] = -NormalVector[2];
    }
    NormalizeVector(NormalVector);
  }
  void Circle::MakeOuterProducts(REAL_TYPE* OuterProducts)
  {
    //z軸方向の単位ベクトルを Vz={0,0,1}とすると
    //OuterProducts[0] = Vz[1]*NormalVector[2]-Vz[2]*NormalVector[1];
    //OuterProducts[1] = Vz[2]*NormalVector[0]-Vz[0]*NormalVector[2];
    //OuterProducts[2] = Vz[0]*NormalVector[1]-Vz[1]*NormalVector[0];
    //
    //0となる項を消すと次行のようになる。

    OuterProducts[0] = -NormalVector[1];
    OuterProducts[1] =  NormalVector[0];
    OuterProducts[2] =  0;

    NormalizeVector(OuterProducts);
  }

  void Circle::MakeRotationMatrix(REAL_TYPE * OuterProducts)
  {
    REAL_TYPE cos = (NormalVector[2]);
    REAL_TYPE sin = std::sqrt(1 - cos * cos);

    R[0] = OuterProducts[0] * OuterProducts[0] * (1 - cos) + cos;
    R[1] = OuterProducts[0] * OuterProducts[1] * (1 - cos) - OuterProducts[2] * sin;
    R[2] = OuterProducts[0] * OuterProducts[2] * (1 - cos) + OuterProducts[1] * sin;

    R[3] = OuterProducts[1] * OuterProducts[0] * (1 - cos) + OuterProducts[2] * sin;
    R[4] = OuterProducts[1] * OuterProducts[1] * (1 - cos) + cos;
    R[5] = OuterProducts[1] * OuterProducts[2] * (1 - cos) - OuterProducts[0] * sin;

    R[6] = OuterProducts[2] * OuterProducts[0] * (1 - cos) - OuterProducts[1] * sin;
    R[7] = OuterProducts[2] * OuterProducts[1] * (1 - cos) + OuterProducts[0] * sin;
    R[8] = OuterProducts[2] * OuterProducts[2] * (1 - cos) + cos;
  }

  bool Circle::isReasonable_N_and_a(void)
  {
      REAL_TYPE dr = GetRadius() / N;
      REAL_TYPE dl = 2 * GetRadius() * std::sin(2 * M_PI / (a * N) / 2);

      return (0.5 < dr / dl && dr / dl < 1.5);
  }

  bool Circle::Make_N_and_a(void)
  {
    int NumStartPointsWithoutCenter = SumStartPoints-1;
    //半径方向の分割数(N)を決める
    for(int i = 1; i < NumStartPointsWithoutCenter; i++) {
      N = i;
      a = NumStartPointsWithoutCenter / (N * (N + 1) / 2);

      if(isReasonable_N_and_a())
      {
        LPT::LPT_LOG::GetInstance()->LOG("N = ", N);
        LPT::LPT_LOG::GetInstance()->LOG("a = ", a);
        break;
      }
    }
    SetSumStartPoints(CalcSumStartPoints());
    return isReasonable_N_and_a();
  }

  int Circle::CalcSumStartPoints()
  {
    int tmpSumStartPoints=0;
    for(int i = Istart; i <= Iend; i++) {
      for(int j = 0; j < i * a; j++) {
        double theta = 2 * M_PI / (i * a) * j;
        if(theta_min <= theta && theta < theta_max) {
          ++tmpSumStartPoints;
        }
      }
    }
    //円の中心の座標はtheta_min=0の領域のみに含める
    if (theta_min == 0 )
    {
      ++tmpSumStartPoints;
    }
    return tmpSumStartPoints;
  }


  bool Circle::Initialize(void)
  {
    if(! Make_N_and_a())
    {
      LPT::LPT_LOG::GetInstance()->WARN("Couldn't find reasonable a and N");
      return false;
    }

    ConvertNormalVector();

    REAL_TYPE OuterProducts[3];
    MakeOuterProducts(OuterProducts);

    MakeRotationMatrix(OuterProducts);

    //開始点の密度 開始点数/円の面積(無次元) を計算してログに出力
    LPT::LPT_LOG::GetInstance()->INFO("Number of Start point = ", GetSumStartPoints());
    LPT::LPT_LOG::GetInstance()->INFO("Start point density   = ", GetSumStartPoints() / (M_PI * Radius * Radius));
    return true;
  }

  void Circle::GetGridPointCoord(std::vector < REAL_TYPE >& Coords)
  {
    for(int i = Istart; i <= Iend; i++) {
      REAL_TYPE r = GetRadius() / N * i;

      for(int j = 0; j < i * a; j++) {
        double theta = 2 * M_PI / (i * a) * j;

        if(theta_min <= theta && theta < theta_max) {
          REAL_TYPE x = r * std::cos(theta);
          REAL_TYPE y = r * std::sin(theta);
          Coords.push_back(R[0] * x + R[1] * y + Coord1[0]);
          Coords.push_back(R[3] * x + R[4] * y + Coord1[1]);
          Coords.push_back(R[6] * x + R[7] * y + Coord1[2]);
        }
      }
    }
    //円の中心の座標はtheta_min=0の領域のみに含める
    if (theta_min == 0 ){
      Coords.push_back(Coord1[0]);
      Coords.push_back(Coord1[1]);
      Coords.push_back(Coord1[2]);
    }

  }

  void Circle::Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints)
  {
    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピー(へのポインタ)を
    //vectorに格納して終了
    if(MaxNumStartPoints >= GetSumStartPoints()) {
      Circle *NewCircle = new Circle(*this);
      StartPoints->push_back(NewCircle);
      LPT::LPT_LOG::GetInstance()->LOG("GetSumStartPoints = ", GetSumStartPoints());
      LPT::LPT_LOG::GetInstance()->LOG("MaxNumStartPoints= ", MaxNumStartPoints);
      return;
    } else if(MaxNumStartPoints <= 0){
      // MaxNumStartPointsが0以下の時はエラーメッセージを出力して終了
      LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
      return;
    }

    //余りオブジェクトが持つ開始点数
    int NumReminder = GetSumStartPoints() % MaxNumStartPoints;

    //余りオブジェクトを除いた分割後のオブジェクトの数
    int NumParts = GetSumStartPoints() / MaxNumStartPoints;
    LPT::LPT_LOG::GetInstance()->LOG("NumParts = ",NumParts );
    LPT::LPT_LOG::GetInstance()->LOG("Reminder = ",NumReminder);

    //分割後のオブジェクトが持つ開始点数
    int NumGridPoints = (GetSumStartPoints() - NumReminder) / NumParts;

    //余りオブジェクトの中心角はオブジェクトに含まれる開始点数の比を元に
    //初期値を決めて、オブジェクトに含まれる開始点数がNumReminderよりも少ない場合は
    //すこしづつ中心角を広げる
    //逆に余りオブジェクトに余計に開始点が含まれている場合はそのままにする
    double ReminderCentralAngle=0.0;
    if (NumReminder != 0)
    {
      ReminderCentralAngle=((double)NumReminder/(double)SumStartPoints) * 2 * M_PI;
      Circle *NewCircle = new Circle(*this);
      NewCircle->theta_min = 2 * M_PI - ReminderCentralAngle;
      double tick = (2 * M_PI - ReminderCentralAngle)/NumParts/NumGridPoints;
 
      while(NumReminder > NewCircle->CalcSumStartPoints())
      {
        NewCircle->theta_min -= tick;
      }
      LPT::LPT_LOG::GetInstance()->LOG("theta_min = ",NewCircle->theta_min);
      LPT::LPT_LOG::GetInstance()->LOG("theta_max = ",NewCircle->theta_max);
      LPT::LPT_LOG::GetInstance()->LOG("ReminderCentralAngle = ",ReminderCentralAngle);
      NewCircle->SetSumStartPoints(NewCircle->CalcSumStartPoints());
      StartPoints->push_back(NewCircle);
    }

    const double CentralAngle=(2*M_PI-ReminderCentralAngle)/NumParts;
    LPT::LPT_LOG::GetInstance()->LOG("ReminderCentralAngle = ",ReminderCentralAngle);
    LPT::LPT_LOG::GetInstance()->LOG("CentralAngle = ",CentralAngle);
    for (int i=0;i<NumParts;++i)
    {
      Circle *NewCircle = new Circle(*this);

      NewCircle->theta_min=i*CentralAngle;
      NewCircle->theta_max=(i+1)*CentralAngle;
      LPT::LPT_LOG::GetInstance()->LOG("theta_min = ",NewCircle->theta_min);
      LPT::LPT_LOG::GetInstance()->LOG("theta_max = ",NewCircle->theta_max);
      NewCircle->SetSumStartPoints(NewCircle->CalcSumStartPoints());
      StartPoints->push_back(NewCircle);
    }
  }


} // namespace PPlib
