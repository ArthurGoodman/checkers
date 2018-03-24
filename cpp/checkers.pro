QT += core websockets
QT -= gui

TARGET = checkers
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/lib/swi-prolog/include/
LIBS += -lswipl

HEADERS += \
    server.h \
    prologinterface.h \
    board.h \
    engine.h

SOURCES += main.cpp \
    server.cpp \
    prologinterface.cpp \
    board.cpp \
    engine.cpp
