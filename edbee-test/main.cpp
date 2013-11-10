/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QApplication>
#include <QDebug>
#include <QTimer>

#include <QsLog.h>
#include <QsLogDest.h>

#include "util/test.h"

#include "edbee/edbee.h"

#include "debug.h"


//#define SINGLE_TEST_TO_RUN "rbit::texteditor::LineOffsetVectorTest"
//#define SINGLE_TEST_TO_RUN "rbit::texteditor::PlainTextDocumentTest"
//#define SINGLE_TEST_TO_RUN "rbit::texteditor::TextLineDataTest"

int main(int argc, char* argv[])
{
    QApplication app( argc, argv);

    // make sure we see the QsLogging items
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    static QsLogging::DestinationPtr debugDestination( QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    //delete (void*)1;    // crash :-)
    logger.addDestination(debugDestination.get());
    logger.setLoggingLevel(QsLogging::TraceLevel);



    // Load the grammars
    QString appDataPath;
    #ifdef Q_OS_MAC
        appDataPath = app.applicationDirPath() + "/../Resources/";
    #else
        appDataPath= app.applicationDirPath() + "/data/";
    #endif
qlog_info() << "LOAD DATA:" << appDataPath;

    // configure the edbee component to use the default paths
    edbee::Edbee* tm = edbee::Edbee::instance();
//    tm->setKeyMapPath( QString("%1%2").arg(appDataPath).arg("keymaps"));
    tm->setGrammarPath(  QString("%1%2").arg(appDataPath).arg("syntaxfiles") );
//    tm->setThemePath( QString("%1%2").arg(appDataPath).arg("themes") );
    tm->init();

    // next run all tests
#ifdef SINGLE_TEST_TO_RUN
    edbee::test::engine().run(SINGLE_TEST_TO_RUN);
#else
    edbee::test::engine().runAll();
#endif


    // dirty hack to send a shutdown-hook signal
//    QTimer timer;
//    timer.connect(&timer,SIGNAL(timeout()),&app, SLOT(quit()));
//    timer.start(100);
//    app.exec();

    tm->shutdown();

    return 0;
}

