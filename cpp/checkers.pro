QT += core websockets
QT -= gui

TARGET = checkers
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += "C:/Program Files (x86)/swipl/include"
LIBS += -L"C:/Program Files (x86)/swipl/bin" -lswipl

HEADERS += \
    server.h \
    prologinterface.h \
    board.h

SOURCES += main.cpp \
    server.cpp \
    prologinterface.cpp \
    board.cpp
