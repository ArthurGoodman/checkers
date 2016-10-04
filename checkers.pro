QT += core websockets
QT -= gui

TARGET = checkers
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += "C:/Program Files (x86)/swipl/include"
LIBS += -L"C:/Program Files (x86)/swipl/bin" -lswipl

HEADERS +=

SOURCES += main.cpp

OTHER_FILES += \
    client.html
