#include "Cuboid.h"
#include "LPT_LogOutput.h"

namespace PPlib
{
  std::ostream & operator <<(std::ostream & stream, const Cuboid& obj)
  {
    stream << obj.TextPrint(stream);
    return stream;
  }

  std::istream & operator >>(std::istream & stream, Cuboid& obj)
  {
    stream >> obj.Coord1[0] >> obj.Coord1[1] >> obj.Coord1[2];
    stream >> obj.Coord2[0] >> obj.Coord2[1] >> obj.Coord2[2];
    stream >> obj.SumStartPoints;
    stream >> obj.NumStartPoints[0] >> obj.NumStartPoints[1] >> obj.NumStartPoints[2];
    stream >> obj.StartTime >> obj.ReleaseTime >> obj.TimeSpan >> obj.ParticleLifeTime;

    return stream;
  }

  std::vector < StartPoint * >*Cuboid::Divider(const int &NumParts)
  {
    return NULL;
  }

  void Cuboid::GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)
  {
    // 立方体の8頂点の座標を保持するDV3オブジェクトを生成
    // 各オブジェクトと頂点の位置関係は以下のとおり
    //
    //      8-----2
    //     /|    /|
    //    4-----7 |
    //    | 6---|-3
    //    |/    |/
    //    1-----5
    //

    DSlib::DV3 DV3Coord1(Coord1[0], Coord1[1], Coord1[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord1 = ", DV3Coord1);

    DSlib::DV3 DV3Coord2(Coord2[0], Coord2[1], Coord2[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord2 = ", DV3Coord2);

    DSlib::DV3 Coord3(Coord2[0], Coord2[1], Coord1[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord3 = ", Coord3);

    DSlib::DV3 Coord4(Coord1[0], Coord1[1], Coord2[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord4 = ", Coord4);

    DSlib::DV3 Coord5(Coord2[0], Coord1[1], Coord1[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord5 = ", Coord5);

    DSlib::DV3 Coord6(Coord1[0], Coord2[1], Coord1[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord6 = ", Coord6);

    DSlib::DV3 Coord7(Coord2[0], Coord1[1], Coord2[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord7 = ", Coord7);

    DSlib::DV3 Coord8(Coord1[0], Coord2[1], Coord2[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord8 = ", Coord8);

    // Coord1, 5 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords1;
    DividePoints(&tmpCoords1, NumStartPoints[0], DV3Coord1, Coord5);

    // Coord6, 3 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords2;
    DividePoints(&tmpCoords2, NumStartPoints[0], Coord6, Coord3);

    // Coord4, 7 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords3;
    DividePoints(&tmpCoords3, NumStartPoints[0], Coord4, Coord7);

    // Coord8, 2 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords4;
    DividePoints(&tmpCoords4, NumStartPoints[0], Coord8, DV3Coord2);

    //Cood1, 3 の平面内にある開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords5;
    for(int i = 0; i < NumStartPoints[0]; i++) {
      DividePoints(&tmpCoords5, NumStartPoints[1], tmpCoords1[i], tmpCoords2[i]);
    }

    //Cood4, 2 の平面内にある開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords6;
    for(int i = 0; i < NumStartPoints[0]; i++) {
      DividePoints(&tmpCoords6, NumStartPoints[1], tmpCoords3[i], tmpCoords4[i]);
    }

    //全領域内にある開始点座標を求める
    for(int i = 0; i < NumStartPoints[0] * NumStartPoints[1]; i++) {
      DividePoints(&Coords, NumStartPoints[2], tmpCoords5[i], tmpCoords6[i]);
    }

    LPT::LPT_LOG::GetInstance()->LOG("Number of grid points = ", Coords.size());
  }

} // namespace PPlib
