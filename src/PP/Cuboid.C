#include "Cuboid.h"
#include "LPT_LogOutput.h"
#include "Utility.h"
#include "SimpleStartPointFactory.h"

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

  void Cuboid::getAllVertexCoord(DSlib::DV3 *DV3Coord)
  {
    // 立方体の全頂点の座標を引数で渡されたDV3オブジェクトの配列へ格納する
    // 各頂点と格納先のindexの関係は以下のとおり
    //
    //    z      y
    //    |     /   
    //    | 7-----1
    //    |/| /  /|
    //    3-----6 |
    //    | 5---|-2
    //    |/    |/
    //    0-----4----> x
    //

    DV3Coord[0].x= Coord1[0];
    DV3Coord[0].y= Coord1[1];
    DV3Coord[0].z= Coord1[2];

    DV3Coord[1].x= Coord2[0];
    DV3Coord[1].y= Coord2[1];
    DV3Coord[1].z= Coord2[2];

    DV3Coord[2].x= Coord2[0];
    DV3Coord[2].y= Coord2[1];
    DV3Coord[2].z= Coord1[2];

    DV3Coord[3].x= Coord1[0];
    DV3Coord[3].y= Coord1[1];
    DV3Coord[3].z= Coord2[2];

    DV3Coord[4].x= Coord2[0];
    DV3Coord[4].y= Coord1[1];
    DV3Coord[4].z= Coord1[2];

    DV3Coord[5].x= Coord1[0];
    DV3Coord[5].y= Coord2[1];
    DV3Coord[5].z= Coord1[2];

    DV3Coord[6].x= Coord2[0];
    DV3Coord[6].y= Coord1[1];
    DV3Coord[6].z= Coord2[2];

    DV3Coord[7].x= Coord1[0];
    DV3Coord[7].y= Coord2[1];
    DV3Coord[7].z= Coord2[2];

    LPT::LPT_LOG::GetInstance()->LOG("Coord0 = ", DV3Coord[0]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord1 = ", DV3Coord[1]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord2 = ", DV3Coord[2]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord3 = ", DV3Coord[3]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord4 = ", DV3Coord[4]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord5 = ", DV3Coord[5]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord6 = ", DV3Coord[6]);
    LPT::LPT_LOG::GetInstance()->LOG("Coord7 = ", DV3Coord[7]);



  }

  void Cuboid::Divider(std::vector < StartPoint * >*StartPoints, const int &MaxNumStartPoints)
  {
    //オブジェクトの開始点数が引数で指定された数以下の場合は、自分自身のコピーを格納したvectorを返す
    if(MaxNumStartPoints >= GetSumStartPoints()) {
      Cuboid *NewCuboid = new Cuboid(*this);

      StartPoints->push_back(NewCuboid);
      return;
    } else if(MaxNumStartPoints <= 0){
      // MaxNumStartPointsが0以下の時はエラーメッセージを出力して終了
      LPT::LPT_LOG::GetInstance()->WARN("illegal MaxNumStartPoints. this StartPointSetting will be deleted ", this);
      return;
    }

    int N=NumStartPoints[0];
    int M=NumStartPoints[1];
    int K=NumStartPoints[2];

    //MaxNumStartPointsを (2^Pow[0] * 3^Pow[1] * 5^Pow[2] * Rem) の形に因数分解
    int Pow[4];
    Factorize235(MaxNumStartPoints, Pow);
    int &RemNumStartPoints=Pow[3];

    //分割後の小領域の初期値を決める
    int NB = pow(2, (Pow[0]/3+Pow[0]%3)) * pow(3, (Pow[1]/3)         ) * pow(5, (Pow[2]/3)) * RemNumStartPoints;
    int MB = pow(2, (Pow[0]/3)         ) * pow(3, (Pow[1]/3+Pow[1]%3)) * pow(5, (Pow[2]/3));
    int KB = pow(2, (Pow[0]/3)         ) * pow(3, (Pow[1]/3)         ) * pow(5, (Pow[2]/3+Pow[2]%3));

    LPT::LPT_LOG::GetInstance()->LOG("initial NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("initial MB = ", MB);
    LPT::LPT_LOG::GetInstance()->LOG("initial KB = ", KB);

    // NB>N, MB>M, KB>KとなっていたらNB,MB,KBを減らして調整
    while ( NB>N || MB>M || KB>K ) {
      if (NB>N) {
        if (NB%2 == 0) {
          NB /= 2;
          MB *= 2;
        }else if (NB %3 ==0) {
          NB /= 3;
          MB *= 3;
        }else if (NB %5 == 0) {
          NB /= 5;
          MB *= 5;
        }else if (NB % RemNumStartPoints) {
          NB /= RemNumStartPoints;
          MB *= RemNumStartPoints;
        }else {
          LPT::LPT_LOG::GetInstance()->ERROR("Illegal NB: ", NB);
        }
      }

      if (MB>M) {
        if (MB%2 == 0) {
          MB /= 2;
          KB *= 2;
        }else if (MB %3 ==0) {
          MB /= 3;
          KB *= 3;
        }else if (MB %5 == 0) {
          MB /= 5;
          KB *= 5;
        }else if (MB % RemNumStartPoints) {
          MB /= RemNumStartPoints;
          KB *= RemNumStartPoints;
        }else {
          LPT::LPT_LOG::GetInstance()->ERROR("Illegal MB: ", MB);
        }
      }

      if (KB>K) {
        if (KB%2 == 0) {
          KB /= 2;
          NB *= 2;
        }else if (KB %3 ==0) {
          KB /= 3;
          NB *= 3;
        }else if (KB %5 == 0) {
          KB /= 5;
          NB *= 5;
        }else if (KB % RemNumStartPoints) {
          KB /= RemNumStartPoints;
          NB *= RemNumStartPoints;
        }else {
          LPT::LPT_LOG::GetInstance()->ERROR("Illegal KB: ", KB);
        }
      }
    }

    LPT::LPT_LOG::GetInstance()->LOG("NB = ", NB);
    LPT::LPT_LOG::GetInstance()->LOG("MB = ", MB);
    LPT::LPT_LOG::GetInstance()->LOG("KB = ", KB);
    
    DSlib::DV3 DV3Coord[8];
    getAllVertexCoord(DV3Coord);


    //x方向の余りオブジェクトを分割
    if(N%NB != 0){
      std::vector < DSlib::DV3 > tmpCoords;
      DividePoints(&tmpCoords, N, DV3Coord[0], DV3Coord[4]);
      REAL_TYPE NReminderCoord1[3]={this->Coord1[0],this->Coord1[1],this->Coord1[2]};
      N -= N%NB;
      this->NumStartPoints[0]=N;
      this-> Coord2[0]   = tmpCoords[N].x;
      NReminderCoord1[0] = tmpCoords[N+1].x;
      int NReminderNumStartPoints[3]={N%NB, NumStartPoints[1], NumStartPoints[2]};
      StartPoints->push_back(CuboidFactory::create(NReminderCoord1, this->Coord2, NReminderNumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }

    //y方向の余りオブジェクトを分割
    if(M%MB != 0){
      std::vector < DSlib::DV3 > tmpCoords;
      DividePoints(&tmpCoords, M, DV3Coord[0], DV3Coord[5]);
      REAL_TYPE MReminderCoord1[3]={this->Coord1[0],this->Coord1[1],this->Coord1[2]};
      M -= M%MB;
      this->NumStartPoints[1]=M;
      this-> Coord2[1]   = tmpCoords[M].y;
      MReminderCoord1[1] = tmpCoords[M+1].y;
      int MReminderNumStartPoints[3]={NumStartPoints[0], M%MB, NumStartPoints[2]};
      StartPoints->push_back(CuboidFactory::create(MReminderCoord1, this->Coord2, MReminderNumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }

    //z方向の余りオブジェクトを分割
    if(K%KB != 0){
      std::vector < DSlib::DV3 > tmpCoords;
      DividePoints(&tmpCoords, K, DV3Coord[0], DV3Coord[3]);
      REAL_TYPE KReminderCoord1[3]={this->Coord1[0],this->Coord1[1],this->Coord1[2]};
      K -= K%KB;
      this->NumStartPoints[2]=K;
      this-> Coord2[1]   = tmpCoords[K].z;
      KReminderCoord1[1] = tmpCoords[K+1].z;
      int KReminderNumStartPoints[3]={NumStartPoints[0], NumStartPoints[1], K%KB};
      StartPoints->push_back(CuboidFactory::create(KReminderCoord1, this->Coord2, KReminderNumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }

    //残った部分をNB*MB*KBづつの小領域に分割する
    //まずはN方向を分割
    std::vector < Cuboid * > tmpStartPoints1;
    std::vector < DSlib::DV3 > NtmpCoords;
    DividePoints(&NtmpCoords, N, DV3Coord[0], DV3Coord[4]);

    for(int i=0;i < N-1;i+=NB)
    {
      REAL_TYPE Coord1[3] = {NtmpCoords[i].x,     this->Coord1[1],this->Coord1[2]};
      REAL_TYPE Coord2[3] = {NtmpCoords[i+NB-1].x,this->Coord2[1],this->Coord1[2]};
      int NumStartPoints[3] = {NB,M,K};
      tmpStartPoints1.push_back(CuboidFactory::create(Coord1, Coord2, NumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
    }

    //前のループで作ったN方向に分割後のオブジェクトをM方向に分割する
    std::vector < Cuboid * > tmpStartPoints2;
    for(std::vector < Cuboid * >::iterator it = tmpStartPoints1.begin(); it != tmpStartPoints1.end();++it)
    {
      std::vector < DSlib::DV3 > MtmpCoords;
      DividePoints(&MtmpCoords, M, DV3Coord[0], DV3Coord[5]);
      for (int i=0;i<M-1;i+=MB)
      {
        REAL_TYPE Coord1[3] = {((*it)->Coord1)[0], MtmpCoords[i].y,      ((*it)->Coord1)[2]};
        REAL_TYPE Coord2[3] = {((*it)->Coord2)[0], MtmpCoords[i+MB-1].y, ((*it)->Coord2)[2]};
        int NumStartPoints[3] = {NB,MB,K};
        tmpStartPoints2.push_back(CuboidFactory::create(Coord1, Coord2, NumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
      }
      delete *it;
    }

    //前のループで作ったN,M方向に分割後のオブジェクトをK方向に分割しStartPointに格納する
    for(std::vector < Cuboid * >::iterator it = tmpStartPoints2.begin(); it != tmpStartPoints2.end();++it)
    {
      std::vector < DSlib::DV3 > KtmpCoords;
      DividePoints(&KtmpCoords, K, DV3Coord[0], DV3Coord[3] );
      for(int i=0;i<K-1;i+=KB)
      {
        REAL_TYPE Coord1[3] = {((*it)->Coord1)[0], ((*it)->Coord1)[1], KtmpCoords[i].z,    };
        REAL_TYPE Coord2[3] = {((*it)->Coord2)[0], ((*it)->Coord2)[1], KtmpCoords[i+KB-1].z,};
        int NumStartPoints[3] = {NB,MB,KB};
        StartPoints->push_back(CuboidFactory::create(Coord1, Coord2, NumStartPoints, StartTime, ReleaseTime, TimeSpan, ParticleLifeTime));
      }
      delete *it;
    }
  }

  void Cuboid::GetGridPointCoord(std::vector < DSlib::DV3 > &Coords)
  {
    DSlib::DV3 DV3Coord[8];
    getAllVertexCoord(DV3Coord);

    // Coord0, 4 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords1;
    DividePoints(&tmpCoords1, NumStartPoints[0], DV3Coord[0], DV3Coord[4]);

    // Coord5, 2 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords2;
    DividePoints(&tmpCoords2, NumStartPoints[0], DV3Coord[5], DV3Coord[2]);

    // Coord3, 6 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords3;
    DividePoints(&tmpCoords3, NumStartPoints[0], DV3Coord[3], DV3Coord[6]);

    // Coord7, 1 間の開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords4;
    DividePoints(&tmpCoords4, NumStartPoints[0], DV3Coord[7], DV3Coord[1]);

    //Cood0, 2 の平面内にある開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords5;
    for(int i = 0; i < NumStartPoints[0]; i++) {
      DividePoints(&tmpCoords5, NumStartPoints[1], tmpCoords1[i], tmpCoords2[i]);
    }

    //Cood3, 1 の平面内にある開始点座標を求める
    std::vector < DSlib::DV3 > tmpCoords6;
    for(int i = 0; i < NumStartPoints[0]; i++) {
      DividePoints(&tmpCoords6, NumStartPoints[1], tmpCoords3[i], tmpCoords4[i]);
    }

    //領域内にある全ての開始点座標を求める
    for(int i = 0; i < NumStartPoints[0] * NumStartPoints[1]; i++) {
      DividePoints(&Coords, NumStartPoints[2], tmpCoords5[i], tmpCoords6[i]);
    }

    LPT::LPT_LOG::GetInstance()->LOG("Number of grid points = ", Coords.size());
  }

} // namespace PPlib
