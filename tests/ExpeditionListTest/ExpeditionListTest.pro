QT += testlib core
CONFIG += console testcase c++11
TEMPLATE = app
TARGET = tst_expeditionlist

INCLUDEPATH += $$PWD/../..

SOURCES += \
    tst_expeditionlist.cpp \
    ../../core/ExpeditionList.cpp

HEADERS += \
    ../../core/ExpeditionList.h
