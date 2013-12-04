#ifndef LPT_FILE_OUTPUT_H
#define LPT_FILE_OUTPUT_H
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <mpi.h>
#include "FileManager.h"
#include "ParticleData.h"

namespace LPT
{
/**
 * @brief ファイル出力を行なう抽象クラス
 *
 * 以下のようなファイル構成を想定している
 *  FH(FileHeader)   ファイル全体の先頭部分に1回だけ出力される情報
 *  RH(RecordHeader) 1回の書き込み毎に先頭部分に1回出力される情報
 *  R(Record)        list又はvectorに格納された出力対象のデータ
 *  RF(RecordHeader) 1回の書き込み毎に末尾部分に1回出力される情報
 *  FF(FileHeader)   ファイル全体の末尾部分に1回だけ出力される情報
 *     
 *  バイナリファイルの出力イメージ
 *     +----+----+-------+----+----+-------+----+------+----+----+
 *     | FH | RH |   R   | RF | RH |   R   | RF |......| RF | FF |
 *     +----+----+-------+----+----+-------+----+------+----+----+
 *     ↑ファイルの先頭位置
 *
 *  テキストファイルの出力イメージ
 *     +---------------------------------------------+
 *     |  +---------------------------------------+  |
 *     |  | FH                                    |  |
 *     |  +---------------------------------------+  |
 *     |                                             |
 *     |  +---------------------------------------+  |
 *     |  | RH                                    |  |
 *     |  +---------------------------------------+  |
 *     |  +---------------------------------------+  |
 *     |  |                                       |  |
 *     |  | R                                     |  |
 *     |  |                                       |  |
 *     |  +---------------------------------------+  |
 *     |  +---------------------------------------+  |
 *     |  | RF                                    |  |
 *     |  +---------------------------------------+  |
 *     |                                             |
 *     |  +---------------------------------------+  |
 *     |  | RH                                    |  |
 *     |  +---------------------------------------+  |
 *     |  +---------------------------------------+  |
 *     |  |                                       |  |
 *     |  | R                                     |  |
 *     |  |                                       |  |
 *     |  +---------------------------------------+  |
 *     |  +---------------------------------------+  |
 *     |  | RF                                    |  |
 *     |  +---------------------------------------+  |
 *     |     ・                                      |
 *     |     ・                                      |
 *     |     ・                                      |
 *     |  +---------------------------------------+  |
 *     |  | RH                                    |  |
 *     |  +---------------------------------------+  |
 *     |  +---------------------------------------+  |
 *     |  |                                       |  |
 *     |  | R                                     |  |
 *     |  |                                       |  |
 *     |  +---------------------------------------+  |
 *     |  +---------------------------------------+  |
 *     |  | RF                                    |  |
 *     |  +---------------------------------------+  |
 *     |                                             |
 *     |  +---------------------------------------+  |
 *     |  | FF                                    |  |
 *     |  +---------------------------------------+  |
 *     |                                             |
 *     +---------------------------------------------+
 */
  class FileOutput
  {
  protected:
    /// 出力先のファイルストリームへのポインタ
    std::ofstream Out2;

    /// 出力する粒子データのリストへのポインタ
    std::multimap < long, PPlib::ParticleData *> *Particles;

  public:
    /// 出力する粒子データを受け取る
    virtual void SetParticles(std::multimap < long, PPlib::ParticleData *> * Particles) 
    {
      this->Particles = Particles;
    };

    /// ファイル全体のヘッダを出力
    virtual void WriteFileHeader()
    {
    };

    /// ファイル全体のフッターを出力
    virtual void WriteFileFooter()
    {
    };

    /// レコードのヘッダを出力
    virtual void WriteRecordHeader()
    {
    };

    /// レコードのフッターを出力
    virtual void WriteRecordFooter()
    {
    };

    /// レコードを出力
    virtual void WriteRecord()
    {
    };

    /// デストラクタ
    virtual ~ FileOutput() {
    };
  };

} //namespace LPT
#endif
