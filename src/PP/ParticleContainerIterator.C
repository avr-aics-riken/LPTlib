/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#include "ParticleContainerIterator.h"
#include "ParticleContainer.h"
#include "ParticleData.h"

namespace PPlib
{
std::map<int, std::list<ParticleData*>*> ParticleContainerIterator::map_dummy;
std::list<ParticleData*> ParticleContainerIterator::list_dummy;

ParticleContainerIterator::ParticleContainerIterator(ParticleContainer* arg_container)
{
    container     = arg_container;
    map_iterator  = map_dummy.end();
    list_iterator = list_dummy.end();
}

ParticleContainerIterator::ParticleContainerIterator(ParticleContainer* arg_container, std::map<int, std::list<ParticleData*>*>::iterator arg_map_iterator, std::list<ParticleData*>::iterator arg_list_iterator)
{
    container     = arg_container;
    map_iterator  = arg_map_iterator;
    list_iterator = arg_list_iterator;
}

//memo コピーコンストラクタも一応作ったけど、浅いコピーで良いので自動生成されるもので良かった。
ParticleContainerIterator::ParticleContainerIterator(const ParticleContainerIterator& arg)
{
    container     = arg.container;
    map_iterator  = arg.map_iterator;
    list_iterator = arg.list_iterator;
}

ParticleContainerIterator& ParticleContainerIterator::operator=(const ParticleContainerIterator& arg)
{
    container     = arg.container;
    map_iterator  = arg.map_iterator;
    list_iterator = arg.list_iterator;
    return *this;
}

ParticleData* ParticleContainerIterator::operator*()
{
    return *list_iterator;
}

ParticleData** ParticleContainerIterator::operator->()
{
    return &(*list_iterator);
}

ParticleContainerIterator& ParticleContainerIterator::operator++()
{
    ++list_iterator;
    //list の終端に来たらmap_iteratorをインクリメント
    if(list_iterator == (*map_iterator).second->end())
    {
        ++map_iterator;
        if(map_iterator != container->id_table.end())
        {
            //mapの終端に来ていなければ、list_iteratorを新しいlistのbegin()に設定
            list_iterator = (*map_iterator).second->begin();
        }else{
            //mapの終端だった場合はlist, mapともにdummyのend()に設定
            map_iterator  = map_dummy.end();
            list_iterator = list_dummy.end();
        }
    }
    return *this;
}

ParticleContainerIterator ParticleContainerIterator::operator++(int)
{
    ParticleContainerIterator before = *this;
    ++(*this);
    return before;
}

bool ParticleContainerIterator::operator==(const ParticleContainerIterator& iterator)
{
    return !(*this != iterator);
}

bool ParticleContainerIterator::operator!=(const ParticleContainerIterator& iterator)
{
    return this->map_iterator != iterator.map_iterator || this->list_iterator != iterator.list_iterator;
}
}