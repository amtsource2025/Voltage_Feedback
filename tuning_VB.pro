QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adc_tuning8 #idle->0 (1280x720)
TARGET = adc_tuning9 #idle->100 (1280x720)
TEMPLATE = app

CONFIG += c++11

# Qwt cross-compiled for Zynq
INCLUDEPATH += /home/aai-01/qwt-6.2.0-zynq/include
LIBS += -L/home/aai-01/qwt-6.2.0-zynq/lib -lqwt

# Runtime rpath on Zynq board (/usr/lib contains libqwt.so from qwt-deploy)
QMAKE_LFLAGS += -Wl,-rpath,/usr/lib

SOURCES += \
    hwhandler.cpp \
    keypaddialog.cpp \
    ltc2614.cpp \
    main.cpp \
    mainwindow.cpp \
    adcreader.cpp \
    vaccum.cpp

HEADERS += \
    hwhandler.h \
    keypaddialog.h \
    ltc2614.h \
    mainwindow.h \
    adcreader.h \
    vaccum.h

FORMS += \
    keypaddialog.ui \
    keypaddialog.ui \
    mainwindow.ui
