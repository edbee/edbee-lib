#-------------------------------------------------
#
# Project created by QtCreator 2013-01-03T08:11:25
#
#-------------------------------------------------

QT += core gui widgets

TARGET = edbee
TEMPLATE = lib
CONFIG += staticlib

EDBEE_SANITIZE = $$(EDBEE_SANITIZE)
!isEmpty( EDBEE_SANITIZE ) {
  QMAKE_CXXFLAGS+=-fsanitize=address -fsanitize=bounds -fsanitize-undefined-trap-on-error
  QMAKE_LFLAGS+=-fsanitize=address -fsanitize=bounds -fsanitize-undefined-trap-on-error
}

# This seems to be required for Windows
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
DEFINES += QT_NODLL

include(edbee-lib.pri)
