#-------------------------------------------------
#
# Project created by QtCreator 2018-05-22T09:43:20
#
#-------------------------------------------------

QT       += core

QT       -= gui
QT       += network

TARGET = ObuControl
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    UdpCommon.cpp \
    commonltev2x.cpp \
    rtkpacketcontrol.cpp \
    gpsprotocolparser.cpp \
    TcpServer.cpp \
    tcpclientcommon.cpp \
    GpsAnalyse.cpp \
    recvtongjiudp.cpp \
    recvcanmsgprocess.cpp \
    CommonSerialPort.cpp \
    tongjixieyiprcase.cpp

HEADERS += \
    UdpCommon.h \
    commonltev2x.h \
    TcpServer.h \
    tcpclientcommon.h \
    rtkpacketcontrol.h \
    tongjixieyiprcase.h \
    recvtongjiudp.h \
    GpsAnalyse.h \
    recvcanmsgprocess.h\
    ltev2x.h \
    CommonSerialPort.h \
    gpsprotocolparser.h

LIBS += -lm



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../CommonConfig/release/ -lCommonConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../CommonConfig/debug/ -lCommonConfig
else:unix:!macx: LIBS += -L$$OUT_PWD/../CommonConfig/ -lCommonConfig

INCLUDEPATH += $$PWD/../CommonConfig
DEPENDPATH += $$PWD/../CommonConfig

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonConfig/release/libCommonConfig.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonConfig/debug/libCommonConfig.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonConfig/release/CommonConfig.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonConfig/debug/CommonConfig.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../CommonConfig/libCommonConfig.a

unix:!macx: LIBS += -L$$OUT_PWD/../Can/ -lCan

INCLUDEPATH += $$PWD/../Can
DEPENDPATH += $$PWD/../Can

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../Can/libCan.a

unix:!macx: LIBS += -L$$OUT_PWD/../JasonControl/ -lJasonControl

INCLUDEPATH += $$PWD/../JasonControl
DEPENDPATH += $$PWD/../JasonControl

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../JasonControl/libJasonControl.a

unix:!macx: LIBS += -L$$OUT_PWD/../CommonLog/ -lCommonLog

INCLUDEPATH += $$PWD/../CommonLog
DEPENDPATH += $$PWD/../CommonLog

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../CommonLog/libCommonLog.a

unix:!macx: LIBS += -L$$PWD/ -lltev2x

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
