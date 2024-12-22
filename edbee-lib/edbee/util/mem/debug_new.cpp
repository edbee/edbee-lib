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

#if defined(EDBEE_DEBUG)
#define debug_new_log // printf

// https://en.cppreference.com/w/cpp/memory/new/operator_delete

/// Logs a malloc operation
/// @param size the size of reseved bytes
/// @param file the file of the malloc
/// @param line the line number of the allocation
void* debug_malloc(size_t size, const char* file, const int line)
{
    edbee::DebugAllocationList* allocList = edbee::DebugAllocationList::instance();
    QMutexLocker lock(allocList->mutex());

    void* p = ::malloc(size);

    if( allocList->isRunning() ) {
        edbee::DebugAllocation* info = allocList->find(p);
        if (info != nullptr) {
            printf("already exist %p %s %d\n", p, file, line);
            printf("existing info : %p(%u) %s:%d\n", info->pointer, static_cast<unsigned int>(info->size), info->file, info->line);
            std::free(p);
            return nullptr;
        }
        info =  allocList->add(p, size, const_cast<char*>(file), static_cast<int>(line));
        if (info == nullptr) {
            printf("can not add %p(%u) %s:%d\n", p, static_cast<unsigned int>(size), file, line);
            std::free(p);
            return nullptr;
        }
    }

    printf("- malloc =>  %p(%u) %s:%d\n", p, static_cast<unsigned int>(size), file, line);
    return p;
}


/// frees the given pointer
/// @param p the pointer to free
/// @param file the file which call originated from
/// @param line the linenumber of the delete
void debug_free(void* p, const char* file, const int line)
{
    printf("- free =>  %p(?) %s:%d\n", p, file, line);

    edbee::DebugAllocationList* allocList = edbee::DebugAllocationList::instance();
    QMutexLocker lock(allocList->mutex());

    if( allocList->isRunning() ) {
        if( !allocList->del(p) ) {
            // TODO: This doesn't work very good. It sees to many free that are NOT allocated by us
            if ( allocList->checkDelete() ) {
                printf("can not del %p %s:%d\n", p, file, line );
            }
        }
    }
    std::free(p);
}

/* ORG

/// the new operator
void* operator new (size_t size, const char* file, const int line)
{
    void* p = debug_malloc(size, file, line);
    return p;
}


/// th delete operator
void operator delete (void* p, const char* file, const int line)
{
    operator delete(p);
    debug_free(p, file, line);
}


/// the delete operator
void operator delete (void* p) throw()
{
    return debug_free(p, "unknown", 0);
}


/// the new array operator
void* operator new[] (size_t size, const char* file, const int line)
{
    void* p = debug_malloc(size, file, line);
    return p;
}


/// the delete array operator
void operator delete[] (void* p, const char* file, const int line)
{
    delete[] p; // << This is strange!!
    debug_free(p, file, line);
}


/// the delete array operator
void  operator delete[] (void* p) throw()
{
    debug_free(p, "unknown", 0);
}

*/

// void* operator new (size_t size, const char* file, const int line)

void * operator new(std::size_t size, const char* file, const int line) noexcept(false)
{
    // we are required to return non-null
    void* p = debug_malloc(size == 0 ? 1 : size, file, line);
    if (p == 0) {
        throw std::bad_alloc();
    }
    return p;
}

/// the new array operator
void* operator new[] (size_t size, const char* file, const int line)
{
    void* p = debug_malloc(size == 0 ? 1 : size, file, line);
    if (p == 0) {
        throw std::bad_alloc();
    }
    return p;
}

void operator delete(void *p) noexcept
{
    // operator delete(p);
    debug_free(p, "Unkown", 0);
}

void operator delete(void* p, std::size_t size) noexcept {
    // Your custom sized delete implementation
    debug_free(p, "Unkown sized", static_cast<int>(size));
}

void operator delete(void *p, const char* file, const int line) throw()
{
    // operator delete(p);
    debug_free(p, file, line);
}

void operator delete(void *p, size_t size, const char* file, const int line) throw()
{
    // operator delete(p);
    debug_free(p, file, line);
}

/// the delete array operator
void operator delete[] (void* p, const char* file, const int line) throw()
{
    // delete[] p; // << This is strange!!
    debug_free(p, file, line);
}

void operator delete[] (void* p, size_t size, const char* file, const int line) throw()
{
    // delete[] p; // << This is strange!!
    debug_free(p, file, line);
}

/// the delete array operator
void  operator delete[] (void* p) throw()
{
    debug_free(p, "unknown", 0);
}

void  operator delete[] (void* p, size_t size) throw()
{
    debug_free(p, "unknown", size);
}

#endif


namespace edbee {

/// pauses the memoryleak detection
void pause_memleak_detection(bool value)
{
#ifdef EDBEE_DEBUG_NEW_ACTIVE
    DebugAllocationList::instance()->pause(value);
#else
    Q_UNUSED(value)
#endif
}


} // edbee
