/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */


#pragma once

#include <new>
#include <stdlib.h>
#include <stdio.h>
//#ifdef HAVE_MALLOC_H
//#include <malloc.h>
//#endif
//#include "memoryleak.h"

// #if defined(QT_DEBUG) && !defined(__MINGW32__) && defined(EDBEE_DEBUG)
#if defined(EDBEE_DEBUG)
    #define EDBEE_DEBUG_NEW_ACTIVE

    void* debug_malloc(std::size_t size, const char* file, const int line);
    void  debug_free(void* p, const char* file, const int line);

    void* operator new(std::size_t size, const char* file, const int line) noexcept(false);
    void* operator new(std::size_t count, std::align_val_t a, const char* file, const int line);

    void operator delete(void* p) noexcept;
    // void operator delete(void* p, std::align_val_t a) noexcept;
    void operator delete(void* ptr, std::size_t size) noexcept ;


    // void  operator delete   (void* p, size_t size) throw();

    void* operator new[]    (size_t size, const char* file, const int line) throw();
    void  operator delete[] (void* p, const char* file, const int line) noexcept;
    void  operator delete[] (void* p, size_t size, const char* file, const int line) noexcept;
    // void  operator delete[] (void* p) noexcept;
    // void  operator delete[] (void* p, size_t size) noexcept;


    #define debug_new new(__FILE__, __LINE__)
    #define new       new(__FILE__, __LINE__)
    // #define delete    delete
    #define malloc(A) debug_malloc((A), __FILE__, __LINE__)
    #define free(A)   debug_free((A), __FILE__, __LINE__)
#endif


namespace edbee {
    void pause_memleak_detection(bool value);
} // edbee

