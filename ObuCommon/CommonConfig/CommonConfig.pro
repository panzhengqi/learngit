#-------------------------------------------------
#
# Project created by QtCreator 2018-05-22T09:45:51
#
#-------------------------------------------------

QT       -= gui

TARGET = CommonConfig
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    config.cpp

HEADERS += \
    config.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
