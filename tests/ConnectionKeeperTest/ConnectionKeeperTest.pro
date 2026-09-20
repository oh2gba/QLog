QT += testlib core
CONFIG += console testcase c++11
TEMPLATE = app
TARGET = tst_connectionkeeper

INCLUDEPATH += $$PWD/../..

SOURCES += \
    tst_connectionkeeper.cpp \
    ../../core/ConnectionKeeper.cpp

HEADERS += \
    ../../core/ConnectionKeeper.h
