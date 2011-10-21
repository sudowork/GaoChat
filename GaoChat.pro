#-------------------------------------------------
#
# Project created by QtCreator 2011-10-18T03:41:54
#
#-------------------------------------------------

QT       += core gui

TARGET = GaoChat
TEMPLATE = app

MAKEFILE = Makefile.gui

SOURCES += main.cpp\
        clientgui.cpp\
        client.cpp\
        gaosocket.cpp\
        common.cpp

HEADERS  += clientgui.h\
        client.h\
        gaosocket.h\
        common.h

DESTDIR = bin
OBJECTS_DIR = bin/obj
MOC_DIR = bin/moc
RCC_DIR = bin/rcc
UI_DIR = bin/ui