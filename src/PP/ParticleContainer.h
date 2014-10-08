/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PARTICLE_CONTAINER_H
#define PARTICLE_CONTAINER_H
#include <map>
#include <list>
#include <omp.h>
#include "LPT_LogOutput.h"
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
  ParticleContainer()
  {
    omp_init_lock(&ParticleContainerLock);
  };
  ~ParticleContainer(){}

  typedef ParticleContainerIterator iterator;

  //! コンテナに引数で渡された粒子データのエントリを追加する
  void insert(ParticleData *particle)
  {
    omp_set_lock(&ParticleContainerLock);
    std::map<int, std::list<ParticleData *> *>::iterator it=id_table.find(particle->BlockID);
    if( it != id_table.end())
    {
      it->second->push_back(particle);
    }else{
      std::list<ParticleData *> *tmp = new std::list<ParticleData *>;
      tmp->push_back(particle);
      id_table.insert(std::make_pair(particle->BlockID, tmp));
    }
    omp_unset_lock(&ParticleContainerLock);
  }

  //! コンテナに、引数で渡された粒子のリストを追加する
  //
  //! @attention 同一リスト内の粒子は全て同じBloackIDを持つものでなければならないが
  //!            パフォーマンス上の理由により、同一Blockに所属するかどうかのチェックはしていないので注意
  void insert(std::list<ParticleData*>* particles)
  {
    if(particles==NULL) return;
    if(particles->empty()) return;

    omp_set_lock(&ParticleContainerLock);
    LPT::LPT_LOG::GetInstance()->LOG("particles = ", particles);
    std::map<int, std::list<ParticleData *> *>::iterator it=id_table.find(particles->front()->BlockID);
    if( it != id_table.end())
    {
       LPT::LPT_LOG::GetInstance()->LOG("add to existing list");
      it->second->splice(it->second->end(), *particles);
    }else{
       LPT::LPT_LOG::GetInstance()->LOG("insert new list:");
      id_table.insert(std::make_pair(particles->front()->BlockID, particles));
    }
    omp_unset_lock(&ParticleContainerLock);
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
    std::pair<iterator, iterator> rt = make_pair(ParticleContainerIterator(this), ParticleContainerIterator(this));

    omp_set_lock(&ParticleContainerLock);
    std::map<int, std::list<ParticleData*>*>::iterator it=id_table.find(BlockID);
    if( it != id_table.end())
    {
      std::map<int, std::list<ParticleData*>*>::iterator next=it;
      ++next;
      if(next != id_table.end())
      {
        rt = make_pair(ParticleContainerIterator(this, it, (*it).second->begin()),ParticleContainerIterator(this, next, (*next).second->begin()));
      }else{
        rt = make_pair(ParticleContainerIterator(this, it, (*it).second->begin()),ParticleContainerIterator(this));
      }
    }
    omp_unset_lock(&ParticleContainerLock);
    return rt;
  }

  //! 指定されたBlockIDの粒子データオブジェクトが登録されているlistを返す
  //
  //ここで渡したlistはテーブルから削除される
  std::list<ParticleData*>* find(const int& BlockID)
  {
    std::list<ParticleData*>* rt = NULL;
    omp_set_lock(&ParticleContainerLock);
    std::map<int, std::list<ParticleData*>*>::iterator it=id_table.find(BlockID);
    if( it != id_table.end())
    {
      rt = (*it).second;
      id_table.erase(it);
    }
    omp_unset_lock(&ParticleContainerLock);
    return rt;
  }

  //! コンテナから引数で渡された粒子データのエントリを削除する
  //
  //  粒子オブジェクトは削除されないので、自分でdeleteすること
  ParticleContainer::iterator erase(ParticleContainer::iterator it_particle)
  {
    omp_set_lock(&ParticleContainerLock);
    ParticleContainer::iterator after=it_particle;
    ++after;
    (*it_particle.map_iterator).second->erase(it_particle.list_iterator);
    if((*it_particle.map_iterator).second->empty())
    {
      id_table.erase(it_particle.map_iterator);
    }
    omp_unset_lock(&ParticleContainerLock);
    return after;
  }


  private:
  //ブロックIDとそのIDに含まれる粒子データを格納するlistへのポインタを保持するテーブル
  std::map <int, std::list<ParticleData *> * > id_table;

  //GetRange()で該当するブロックIDが未登録だった時に返す値に使うダミーのlist
  std::list<ParticleData*> dummy_list;

  // ParticleContainerの操作に関わるロック変数
  omp_lock_t ParticleContainerLock;

  friend ParticleContainerIterator;
};


}//end of namespace
#endif
