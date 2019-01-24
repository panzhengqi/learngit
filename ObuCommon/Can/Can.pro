#-------------------------------------------------
#
# Project created by QtCreator 2018-07-03T15:26:56
#
#-------------------------------------------------

QT       -= gui


TARGET = Can
TEMPLATE = lib
CONFIG += staticlib

SOURCES +=  can_manager.cpp \
    can_recv_manager.cpp \
    can_send_manager.cpp \
    com_protocol.cpp \
    inner_recv_manager.cpp \
    can_recv.cpp

HEADERS += \
    can_manager.h \
    can_recv_manager.h \
     includes.h \
    can_send_manager.h \
    com_protocol.h \
    inner_protocol.h \
    inner_recv_manager.h \
    can_recv.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx: LIBS += -L$$OUT_PWD/../JasonControl/ -lJasonControl

INCLUDEPATH += $$PWD/../JasonControl
DEPENDPATH += $$PWD/../JasonControl

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../JasonControl/libJasonControl.a
