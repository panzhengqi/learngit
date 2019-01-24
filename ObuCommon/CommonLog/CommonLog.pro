#-------------------------------------------------
#
# Project created by QtCreator 2018-06-27T14:44:38
#
#-------------------------------------------------

QT       -= gui

TARGET = CommonLog
TEMPLATE = lib
CONFIG += staticlib

SOURCES += log.cpp \
    log_wgy.cpp

HEADERS += log.h \
    block_queue.h \
    log_wgy.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
