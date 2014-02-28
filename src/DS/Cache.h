#ifndef DSLIB_CACHE_H
#define DSLIB_CACHE_H

namespace DSlib
{
//forward declaration
  struct DataBlock;

  //! キャッシュの1エントリ分のデータ(ブロックIDとデータブロック本体へのポインタ)を保持する構造体
  struct Cache
  {
    //! データブロックのID
    long BlockID;

    //!  DataBlockオブジェクトへのポインタ
    DataBlock *ptrData;

    //! Constructor
    Cache():BlockID(-1), ptrData(NULL)
    {
    }
    ~Cache()
    {
    }

  };
} // namespace DSlib
#endif
