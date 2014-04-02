#-------------------------------------------------
#
# Project created by QtCreator 2014-04-02T15:51:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    ../tl_backgroundsubtractors/onlinebackgroundsubtractor.cpp \
    ../tl_core/backgroundsubtractor.cpp \
    ../tl_core/detector.cpp \
    ../tl_core/filter.cpp \
    ../tl_core/tracker.cpp \
    ../tl_detectors/meanshiftdetector.cpp \
    ../tl_detectors/nodetector.cpp \
    ../tl_detectors/templatematchingdetector.cpp \
    ../tl_filters/kalmanfilter.cpp \
    ../tl_gpu/templatematchingdetectorgpu.cpp \
    ../tl_util/color.cpp \
    ../tl_util/conversions.cpp \
    ../tl_util/geometry.cpp

HEADERS  += mainwindow.h \
    ../common.h \
    ../tracklib.h \
    ../tl_backgroundsubtractors/onlinebackgroundsubtractor.h \
    ../tl_core/backgroundsubtractor.h \
    ../tl_core/detector.h \
    ../tl_core/filter.h \
    ../tl_core/tracker.h \
    ../tl_detectors/meanshiftdetector.h \
    ../tl_detectors/nodetector.h \
    ../tl_detectors/templatematchingdetector.h \
    ../tl_filters/kalmanfilter.h \
    ../tl_gpu/templatematchingdetectorgpu.h \
    ../tl_util/color.h \
    ../tl_util/conversions.h \
    ../tl_util/geometry.h

FORMS    += mainwindow.ui

INCLUDEPATH += .. \
            /opt/local/include

LIBS += -L/opt/local/lib/ \
    -lopencv_core \
    -lopencv_gpu \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_video

INCLUDEPATH += /opt/local/include
DEPENDPATH += /opt/local/include
