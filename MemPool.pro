TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    gtest/gtest-all.cc \
    Tests.cpp

HEADERS += \
    IndexableMemoryPool.h \
    MemoryReusingVector.h \
    SmallVector.h \
    gtest/gtest.h

