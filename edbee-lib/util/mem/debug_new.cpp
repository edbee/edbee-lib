/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QMutexLocker>
#include <QMutex>

#include "debug_allocs.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif


#define debug_new_log // printf


void* debug_malloc(size_t size, const char* file, const int line)
{   
    DebugAllocationList* allocList = DebugAllocationList::instance();
    QMutexLocker lock(allocList->mutex());

    void* p = ::malloc(size);
    if( allocList->isRunning() ) {
        DebugAllocation* info = allocList->find(p);
        if (info != 0) {
            printf("already exist %p %s %d\n", p, file, line);
            printf("existing info : %p(%u) %s:%d\n", info->pointer, (unsigned int)info->size, info->file, info->line);
            ::free(p);
            return 0;
        }
        info =  allocList->add(p, size, (char*)file, (int)line);
        if (info == 0) {
            printf("can not add %p(%u) %s:%d\n", p,(unsigned int)size, file, line);
            ::free(p);
            return 0;
        }
    }
    return p;
}

void debug_free(void* p, const char* file, const int line)
{
    DebugAllocationList* allocList = DebugAllocationList::instance();
    QMutexLocker lock(allocList->mutex());

    if( allocList->isRunning() ) {
        if( !allocList->del(p) ) {
            // TODO: This doesn't work very good. It sees to many free that are NOT allocated by us
            if ( allocList->checkDelete_ ) {
                printf("can not del %p %s:%d\n", p, file, line );
            }
        }
    }
    ::free(p);
}

void* operator new (size_t size, const char* file, const int line)
{
    void* p = debug_malloc(size, file, line);
    return p;
}

void operator delete (void* p, const char* file, const int line)
{
    operator delete(p);
    debug_free(p, file, line);
}

void operator delete (void* p) throw()
{
    return debug_free(p, "unknown", 0);
}

void* operator new[] (size_t size, const char* file, const int line)
{
    void* p = debug_malloc(size, file, line);
    return p;
}

void operator delete[] (void* p, const char* file, const int line)
{
    delete[] p;
    return debug_free(p, file, line);
}

void  operator delete[] (void* p) throw()
{
    return debug_free(p, "unknown", 0);
}



void pause_memleak_detection(bool value)
{
    DebugAllocationList::instance()->pause(value);
}
