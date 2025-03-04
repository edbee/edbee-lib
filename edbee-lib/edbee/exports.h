// edbee - Copyright (c) 2012-2025 by Rick Blommers and contirbutors
// SPDX-License-Identifier: MIT

#pragma once

#include <QtCore/qglobal.h>

#ifdef EDBEE_SHARED
    #ifdef EDBEE_EXPORT_SYMBOLS
        #  define EDBEE_EXPORT Q_DECL_EXPORT
    #else
        #  define EDBEE_EXPORT Q_DECL_IMPORT
    #endif
#else
    #  define EDBEE_EXPORT
#endif
