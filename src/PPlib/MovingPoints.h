#ifndef PPLIB_MOVING_POINTS_H
#define PPLIB_MOVING_POINTS_H

#include <iostream>
#include <vector>
#include "StartPoint.h"

namespace PPlib
{
  //!  @brief 移動する開始点設定を保持するクラス
  //! Time[i]の時点の開始点座標がCoords[i]に保持される
  class MovingPoints:public StartPoint
  {
  private:
    //! 時系列で指定した開始点座標
    std::vector < REAL_TYPE > Coords;

    //! Coordsで指定した時刻
    std::vector < double >Time;

  public:
    //! 指定された時刻の開始点座標を設定する
    //! @param Time [in] 時刻
    //! @param Coord [in] Timeの時点の開始点座標
    //! Coordの4番目以降の引数は無視される。
    void AddCoords(double Time, REAL_TYPE * Coord)
    {
      this->Time.push_back(Time);
      for(int i = 0; i < 3; i++)
      {
        this->Coords.push_back(Coord[i]);
    }};

    //! 現在のタイムステップにおける開始点座標をCoordsから読み出して、Coord1にコピーする
    void UpdateStartPoint(double CurrentTime);

    //! 挿入子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, MovingPoints & obj);

  };

} // namespace PPlib
#endif
