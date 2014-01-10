#ifndef PARTICLE_CONTAINER_H
#define PARTICLE_CONTAINER_H
#include <map>
#include <list>
#include "ParticleContainerIterator.h"
#include "ParticleData.h"

//forward declartion
namespace PPlib
{

class ParticleContainer
{
  //non copyable
  ParticleContainer(const ParticleContainer &obj);
  ParticleContainer &operator=(const ParticleContainer&obj);
  public:
  ParticleContainer(){};
  ~ParticleContainer(){}

  //! 現在保持している全てのParticleDataオブジェクトを削除する
  //TODO 削除予定
  void delete_all(void)
  {
    for (std::map<int, std::list<ParticleData *> *>::iterator it_map=id_table.begin();it_map!=id_table.end();++it_map)
    {
      for(std::list<ParticleData *>::iterator it_list=(*it_map).second->begin(); it_list!=(*it_map).second->end();++it_list)
      {
        delete *it_list;
      }
    }
  }
  typedef ParticleContainerIterator iterator;

  //! コンテナに引数で渡された粒子データのエントリを追加する
  void insert(ParticleData *particle)
  {
    std::map<int, std::list<ParticleData *> *>::iterator it=id_table.find(particle->BlockID);
    if( it != id_table.end())
    {
      it->second->push_back(particle);
    }else{
      std::list<ParticleData *> *tmp = new std::list<ParticleData *>;
      tmp->push_back(particle);
      id_table.insert(std::make_pair(particle->BlockID, tmp));
    }
  }

  //! コンテナに登録されている粒子データの数を返す
  int size()
  {
    int sum_size=0;
    for (std::map<int, std::list<ParticleData *> *>::iterator it=id_table.begin();it!=id_table.end();++it)
    {
      sum_size+=(*it).second->size();
    }
    return sum_size;
  }

  iterator begin()
  {
    return id_table.empty() ? ParticleContainerIterator(this) : ParticleContainerIterator(this, id_table.begin(), (*id_table.begin()).second->begin());
  }

  iterator end()
  {
    return ParticleContainerIterator(this);
  }

  //! 指定されたBlockIDの粒子データオブジェクトが登録されている範囲を返す
  std::pair<iterator, iterator> equal_range(const int& BlockID)
  {
    std::map<int, std::list<ParticleData*>*>::iterator it=id_table.find(BlockID);
    if( it != id_table.end())
    {
      std::map<int, std::list<ParticleData*>*>::iterator next=it;
      ++next;
      if(next != id_table.end())
      {
        return make_pair(ParticleContainerIterator(this, it, (*it).second->begin()),ParticleContainerIterator(this, next, (*next).second->begin()));
      }else{
        return make_pair(ParticleContainerIterator(this, it, (*it).second->begin()),ParticleContainerIterator(this));
      }
    }else{
      return make_pair(ParticleContainerIterator(this), ParticleContainerIterator(this));
    }
  }

  //! コンテナから引数で渡された粒子データのエントリを削除する
  //
  //  粒子オブジェクトは削除されないので、自分でdeleteすること
  ParticleContainer::iterator erase(ParticleContainer::iterator it_particle)
  {
    ParticleContainer::iterator after=it_particle;
    ++after;
    (*it_particle.map_iterator).second->erase(it_particle.list_iterator);
    if((*it_particle.map_iterator).second->empty())
    {
      id_table.erase(it_particle.map_iterator);
    }
    return after;
  }


  private:
  //ブロックIDとそのIDに含まれる粒子データを格納するlistへのポインタを保持するテーブル
  std::map <int, std::list<ParticleData *> * > id_table;

  //GetRange()で該当するブロックIDが未登録だった時に返す値に使うダミーのlist
  std::list<ParticleData*> dummy_list;

  friend ParticleContainerIterator;
};


}//end of namespace
#endif
