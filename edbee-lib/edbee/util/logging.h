/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QFileInfo>
#include <QDebug>

namespace edbee {
    enum LogLevel {
        FATAL = 1,
        CRITICAL = 2,
        ERROR = 3,
        WARNING = 4,
        INFO = 5,
        DEBUG = 6,
        TRACE = 7   // No difference between trace and debug
    };
}

#ifndef EDBEE_LOG_LEVEL
#define EDBEE_LOG_LEVEL edbee::LogLevel::TRACE
#endif

#define qlog_trace() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::TRACE ) {} \
    else qDebug() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_debug() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::DEBUG ) {} \
    else qDebug() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_info()  \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::INFO ) {} \
    else qInfo() << QFileInfo( __FILE__).fileName() << '@' << __LINE__
#define qlog_warn()  \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::WARNING ) {} \
    else qWarning() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_error() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::ERROR ) {} \
    else qError() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_critical() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::CRITICAL ) {} \
    else qCritical() << QFileInfo(__FILE__).fileName() << '@' << __LINE__
#define qlog_fatal() \
    if( EDBEE_LOG_LEVEL < edbee::LogLevel::FATAL ) {} \
    else qFatal() << QFileInfo(__FILE__).fileName() << '@' << __LINE__



#define qlog() \
    qInfo()


