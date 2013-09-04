/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QString>
#include <QMutex>

#include <stdlib.h>
#include <stdio.h>

#include "debug_allocs.h"


/// The global memory leak object
static DebugAllocationList allocationList;


DebugAllocationList* DebugAllocationList::instance()
{
    return &allocationList;
}



DebugAllocationList::DebugAllocationList()
    : allocationList_()
    , mutex_(0)
    , running_(false)
    , started_(false)
{
    mutex_ = new QMutex( QMutex::Recursive );
    start(false);
    checkDelete_ = false;
}

DebugAllocationList::~DebugAllocationList()
{
    stop();          // check last memory leak
    //delete mutex_;   // < do not delete keep memory leak :S. When deleting this mutex app shutdown crashes
    mutex_ = 0;
    clear();
}

/// clears the debugging allocation list
void DebugAllocationList::clear()
{
    // iterate over the map's (key,val) pairs as usual
    for(std::map<void*,DebugAllocation*>::iterator iter = allocationList_.begin(); iter != allocationList_.end(); ++iter) {
        std::free( iter->second );
    }

    allocationList_.clear();
}

QMutex *DebugAllocationList::mutex()
{
    return mutex_;
}


void DebugAllocationList::start( bool checkDelete )
{
    clear();
    checkDelete_ = checkDelete;
    running_ = true;
    started_ = true;
}

int DebugAllocationList::stop()
{
    started_ = false;
    running_ = false;
    checkDelete_ = false;
    int res = 0;

    for(std::map<void*,DebugAllocation*>::iterator iter = allocationList_.begin(); iter != allocationList_.end(); ++iter) {
        DebugAllocation *info = iter->second;
        if( info && info->pointer ) {
            printf( "Memory leak %p(%u) %s:%d\n", info->pointer, (unsigned int)info->size, info->file, info->line );
            res++;
        }
    }

    if( !res ) {
        printf("memory ok :-D\n");
    }

    return res;
}

DebugAllocation* DebugAllocationList::find(void* p)
{
    return allocationList_.find(p)->second;
}

DebugAllocation* DebugAllocationList::add(void* p, size_t size, char* file, int line)
{
    DebugAllocation* info = (DebugAllocation*)std::malloc( sizeof(DebugAllocation) );
    info->clear();
    info->pointer = p;
    info->size = size;
    info->file = file;
    info->line = line;
    allocationList_[p]=info;
    return info;
}

bool DebugAllocationList::del(void* p)
{
    std::map<void*,DebugAllocation*>::iterator iter = allocationList_.find(p);

    if( iter != allocationList_.end() ) {
        DebugAllocation *mi = iter->second; //take(p);
        allocationList_.erase(iter);
        if( !mi ) return false;
        std::free( mi );
    }
    return true;

}



//void DebugAllocationList::start(bool del_check)
//{
//   DebugAllocationList::instance()->_start(del_check);
//}

//int DebugAllocationList::stop()
//{
//  return  DebugAllocationList::instance()->_stop();
//}
