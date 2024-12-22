/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QFileInfo>
#include <QDebug>

namespace edbee {
    enum LogLevel {
        LogLevelFatal = 1,
        LogLevelCritical = 2,
        LogLevelError = 3,
        LogLevelWarning = 4,
        LogLevelInfo = 5,
        LogLevelDebug = 6,
        LogLevelTrace = 7
    };
}

#ifndef EDBEE_LOG_LEVEL
#define EDBEE_LOG_LEVEL edbee::LogLevel::LogLevelTrace
#endif

#define qlog_trace() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::LogLevelTrace ) {} \
    else qDebug() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_debug() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::LogLevelDebug ) {} \
    else qDebug() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_info()  \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::LogLevelInfo ) {} \
    else qInfo() << QFileInfo( __FILE__).fileName() << '@' << __LINE__
#define qlog_warn()  \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::LogLevelWarning ) {} \
    else qWarning() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_error() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::LogLevelError ) {} \
    else qError() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_critical() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::LogLevelCritical	 ) {} \
    else qCritical() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_fatal() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::LogLevelFatal ) {} \
    else qFatal() << QFileInfo(__FILE__).fileName() << '@' << __LINE__



#define qlog() \
    qInfo()


