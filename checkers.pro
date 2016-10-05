QT += core websockets
QT -= gui

TARGET = checkers
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += "C:/Program Files (x86)/swipl/include"
LIBS += -L"C:/Program Files (x86)/swipl/bin" -lswipl

HEADERS += \
    server.h

SOURCES += main.cpp \
    server.cpp

OTHER_FILES += \
    checkers.html \
    checkers.js
