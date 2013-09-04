
QT  += core gui
QT  -= sql
QT  += widgets

TARGET = edbee-test
TEMPLATE = app


INCLUDEPATH += $$PWD/../config

# The test sources
SOURCES += \
	edbee/commands/replaceselectioncommandtest.cpp \
	edbee/models/textrangetest.cpp \
    edbee/models/textdocumenttest.cpp \
    edbee/models/textbuffertest.cpp \
	edbee/models/textlinedatatest.cpp \
	edbee/util/gapvectortest.cpp \
	edbee/util/lineoffsetvectortest.cpp \
	main.cpp \
    edbee/util/lineendingtest.cpp \
    edbee/textdocumentserializertest.cpp \
    edbee/io/tmlanguageparsertest.cpp \
    edbee/util/regexptest.cpp \
    edbee/models/textdocumentscopestest.cpp \
    edbee/models/textundostacktest.cpp

HEADERS += \
	edbee/commands/replaceselectioncommandtest.h \
	edbee/models/textrangetest.h \
    edbee/models/textdocumenttest.h \
    edbee/models/textbuffertest.h \
	edbee/models/textlinedatatest.h \
	edbee/util/gapvectortest.h \
	edbee/util/lineoffsetvectortest.h \
    edbee/util/lineendingtest.h \
    edbee/textdocumentserializertest.h \
    edbee/io/tmlanguageparsertest.h \
    edbee/util/regexptest.h \
    edbee/models/textdocumentscopestest.h \
    edbee/models/textundostacktest.h


INCLUDEPATH += $$PWD/../edbee-lib

## Extra dependencies
##====================
include(../vendor/qslog/QsLog.pri)


## edbee-lib dependency
##=======================

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../edbee-lib/release/ -ledbee-lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../edbee-lib/debug/ -ledbee-lib
else:unix:!symbian: LIBS += -L$$OUT_PWD/../edbee-lib/ -ledbee-lib

INCLUDEPATH += $$PWD/../edbee-lib
DEPENDPATH += $$PWD/../edbee-lib

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../edbee-lib/release/edbee-lib.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../edbee-lib/debug/edbee-lib.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../edbee-lib/libedbee-lib.a
