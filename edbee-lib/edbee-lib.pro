#-------------------------------------------------
#
# Project created by QtCreator 2013-01-03T08:11:25
#
#-------------------------------------------------

QT += core gui widgets

TARGET = edbee
TEMPLATE = lib
CONFIG += staticlib

#QMAKE_CXXFLAGS+=-fsanitize=address -fsanitize=bounds
#QMAKE_LFLAGS+=-fsanitize=address -fsanitize=bounds

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../vendor/onig/release/ -lonig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../vendor/onig/debug/ -lonig
else:unix: LIBS += -L$$OUT_PWD/../vendor/onig/ -lonig

# This seems to be required for Windows
INCLUDEPATH += $$PWD $$PWD/../vendor/onig
DEPENDPATH += $$PWD $$PWD/../vendor/onig
DEFINES += QT_NODLL

include(edbee-lib.pri)
