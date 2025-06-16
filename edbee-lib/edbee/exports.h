// edbee - Copyright (c) 2012-2025 by Rick Blommers and contirbutors
// SPDX-License-Identifier: MIT

#pragma once

// code for stricter compilation and testing of validation
// A bit of a hack to place this in export.h.
// This file is included by every header file in edbee.
// Here it is possible to configure the correct validation to perform fixes that are required by every file

#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-conversion"
    #pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#elif defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
#elif defined(_MSC_VER)
#endif

// include headers to prevent cast-errors in Qt Header
#include <QDebug>
#include <QtCore/qglobal.h>
#include <QHash>
#include <QList>
#include <QString>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <qtmochelpers.h>
#endif

#if defined(__clang__)
    #pragma GCC diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#endif


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
