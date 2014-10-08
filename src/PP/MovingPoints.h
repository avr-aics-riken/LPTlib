/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PPLIB_MOVING_POINTS_H
#define PPLIB_MOVING_POINTS_H

#include <iostream>
#include <vector>
#include "StartPoint.h"

namespace PPlib
{
  //!  @brief 移動する開始点設定を保持するクラス
  //!
  //! Time[i]の時点の開始点座標がCoords[i]に保持される
  class MovingPoints:public StartPoint
  {
  private:
    //!@brief 現在時刻で有効な開始点の座標
    //! インスタンス生成時に値が設定されていてもUpdateStartPoint()内で上書きされる
    REAL_TYPE Coord1[3];

    //! 時系列で指定した開始点座標
    std::vector < REAL_TYPE > Coords;

    //! Coordsで指定した時刻
    std::vector < double >Time;

  public:
    //! テキスト出力を行う
    std::ostream & TextPrint(std::ostream & stream) const
    {
      stream << "Coord  1        = " << this->Coords[0] << "," << this->Coords[1] << "," << this->Coords[2] << std::endl;
      stream << "Time  1         = " << this->Time[0] << std::endl;
      stream << "StartTime       = " << this->StartTime << std::endl;
      stream << "ReleaseTime     = " << this->ReleaseTime << std::endl;
      stream << "TimeSpan        = " << this->TimeSpan << std::endl;
      stream << "LatestEmitTime  = " << this->LatestEmitTime << std::endl;
      stream << "ID              = " << this->ID[0] << "," << this->ID[1] << std::endl;
      return stream;
    }

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

    //! 出力演算子オーバーロード
    friend std::ostream & operator <<(std::ostream & stream, const MovingPoints& obj);

    //! 入力演算子オーバーロード
    friend std::istream & operator >>(std::istream & stream, MovingPoints& obj);
  };

} // namespace PPlib
#endif
