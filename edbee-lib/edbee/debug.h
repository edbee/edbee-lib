// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/logging.h"

#include "edbee/util/mem/debug_new.h"

#if defined(QT_DEBUG)
    /// This assert requires the inclusion of QApplication an QThread
    #define Q_ASSERT_GUI_THREAD Q_ASSERT(  qApp->thread() == QThread::currentThread( ) )
    #define Q_ASSERT_NOT_GUI_THREAD Q_ASSERT(  qApp->thread() != QThread::currentThread( ) )
#else
    #define Q_ASSERT_GUI_THREAD
    #define Q_ASSERT_NOT_GUI_THREAD
#endif



