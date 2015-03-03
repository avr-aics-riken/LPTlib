/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef PARTICLE_CONTAINER_ITERATOR_H
#define PARTICLE_CONTAINER_ITERATOR_H
#include <iterator>
#include <map>
#include <list>

namespace PPlib
{
//forward declaration
class ParticleContainer;
class ParticleData;
//! ParticleContanier classのiterator
//
//ParticleContainer::id_tableが持っているlistのイテレータを操作して返すだけ
//本当はbidirectionalで作ることができるが、++で走査する以外の使い方をしないので
//手抜きのためにforward_iteratorとして実装している
class ParticleContainerIterator: public std::iterator<std::forward_iterator_tag, ParticleData*>
{
    // map_iteratorとlist_iteratorをそれぞれend()で初期化するコンストラクタ
    ParticleContainerIterator(ParticleContainer* arg);

    // 指定された値でiteratorも初期化するコンストラクタ
    ParticleContainerIterator(ParticleContainer* arg, std::map<int, std::list<ParticleData*>*>::iterator arg_map_iterator, std::list<ParticleData*>::iterator arg_list_iterator);

public:
    ParticleContainerIterator(const ParticleContainerIterator& arg);
    ParticleContainerIterator& operator=(const ParticleContainerIterator& arg);
    ParticleData*  operator*();
    ParticleData** operator->();
    ParticleContainerIterator& operator++();
    ParticleContainerIterator operator++(int);
    bool operator==(const ParticleContainerIterator& iterator);
    bool operator!=(const ParticleContainerIterator& iterator);

private:
    std::map<int, std::list<ParticleData*>*>::iterator map_iterator;
    std::list<ParticleData*>::iterator                 list_iterator;
    ParticleContainer*                                 container;
    static std::map<int, std::list<ParticleData*>*>    map_dummy;
    static std::list<ParticleData*>                    list_dummy;

    friend ParticleContainer;
};
}
#endif
