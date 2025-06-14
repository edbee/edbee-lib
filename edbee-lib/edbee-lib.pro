#-------------------------------------------------
#
# Project created by QtCreator 2013-01-03T08:11:25
#
#-------------------------------------------------

QT += core gui widgets

# for the time being:
greaterThan(QT_MAJOR_VERSION,5): QT += core5compat

QMAKE_CXXFLAGS += -O2 -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough -Werror=format-security \
  -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 \
  -fstack-protector-strong -fexceptions \
  -D_GLIBCXX_ASSERTIONS \
  -isystem $$[QT_INSTALL_HEADERS]

# Doesn't work on arm:
# -fcf-protection=full
# -fstack-clash-protection

QMAKE_CXXFLAGS += -isystem "$$[QT_INSTALL_LIBS]/QtWidgets.framework/Headers" \
                  -isystem "$$[QT_INSTALL_LIBS]/QtXml.framework/Headers" -isystem "$$[QT_INSTALL_LIBS]/QtGui.framework/Headers" \
                  -isystem "$$[QT_INSTALL_LIBS]/QtCore.framework/Headers"

QMAKE_LDFLAGS=-Wl,-z,nodlopen -Wl,-z,noexecstack \
  -Wl,-z,relro -Wl,-z,now \
  -Wl,--as-needed -Wl,--no-copy-dt-needed-entries

TARGET = edbee
TEMPLATE = lib
CONFIG += staticlib

# Define EDBEE_BEGUG to enable memory debugging
DEFINES += EDBEE_DEBUG

# DEFINE 'EDBEE_SANITIZE' to enable santitize bounds checks
EDBEE_SANITIZE = $$(EDBEE_SANITIZE)
!isEmpty( EDBEE_SANITIZE ) {
  warning('*** SANITIZE ENABLED! edbee-lib ***')
  QMAKE_CXXFLAGS+=-fsanitize=address -fsanitize=bounds -fsanitize-undefined-trap-on-error
  QMAKE_LFLAGS+=-fsanitize=address -fsanitize=bounds -fsanitize-undefined-trap-on-error
}

# This seems to be required for Windows
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
DEFINES += QT_NODLL

include(edbee-lib.pri)
