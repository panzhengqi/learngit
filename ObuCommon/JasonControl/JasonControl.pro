#-------------------------------------------------
#
# Project created by QtCreator 2018-06-25T16:05:42
#
#-------------------------------------------------

QT       -= core gui

include(json/json.pri)

TARGET = JasonControl
TEMPLATE = lib
CONFIG += staticlib

SOURCES +=

HEADERS +=
unix {
    target.path = /usr/lib
    INSTALLS += target
}
