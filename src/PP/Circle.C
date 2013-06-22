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
      if(0.5 < dr / dl && dr / dl < 1.5)
      {
        return true;
      } else{
        return false;
      }
  }

  bool Circle::Make_N_and_a(void)
  {
    for(int i = 1; i < SumStartPoints; i++) {
      N = i;
      a = SumStartPoints / (N * (N + 1) / 2);

      if(isReasonable_N_and_a())
        break;
    }
    LPT::LPT_LOG::GetInstance()->LOG("N = ", N);
    LPT::LPT_LOG::GetInstance()->LOG("a = ", a);
    SetSumStartPoints(a * N * (N + 1) / 2 );
    return isReasonable_N_and_a();
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
    LPT::LPT_LOG::GetInstance()->INFO("Start point density = ", GetSumStartPoints() / (M_PI * Radius * Radius));
    return true;
  }

  void Circle::GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)
  {
    for(int i = Istart; i <= Iend; i++) {
      REAL_TYPE r = GetRadius() / N * i;

      for(int j = 0; j < i * a; j++) {
        DSlib::DV3 * tmpCoord = new DSlib::DV3;
        DSlib::DV3 tmp;
        double theta = 2 * M_PI / (i * a) * j;

        if(theta_min <= theta && theta < theta_max) {
          tmp.x = r * std::cos(theta);
          tmp.y = r * std::sin(theta);
          tmp.z = 0;
          tmpCoord->x = R[0] * tmp.x + R[1] * tmp.y + Coord1[0];
          tmpCoord->y = R[3] * tmp.x + R[4] * tmp.y + Coord1[1];
          tmpCoord->z = R[6] * tmp.x + R[7] * tmp.y + Coord1[2];
          Coords.push_back(*tmpCoord);
        }
      }
    }
    //円の中心の座標はtheta_min=0の領域のみに含める
    if (theta_min == 0 ){
      DSlib::DV3 tmpCoord(Coord1[0], Coord1[1], Coord1[2]);
      Coords.push_back(tmpCoord);
    }

  }

  std::vector < StartPoint * >*Circle::Divider(const int &AveNumStartPoints)
  {
    std::vector < StartPoint * >*StartPoints = new std::vector < StartPoint * >;

    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピーを格納したvectorを返す
    if(AveNumStartPoints >= GetSumStartPoints()) {
      Circle *NewCircle = new Circle(*this);

      StartPoints->push_back(NewCircle);
      return StartPoints;
    }
  }

} // namespace PPlib
