#-------------------------------------------------
#
# Project created by QtCreator 2011-11-21T17:24:40
#
#-------------------------------------------------

QT       += core gui

TARGET = QArpWatch
TEMPLATE = app

# Windows ico
RC_FILE = ressources.rc

# Mac ico use  Icon Composer to build icns file
# ICON = qarpwatch.icns

SOURCES += main.cpp\
        mainwindow.cpp \
    networkutils.cpp \
    arpwatcher.cpp \
    dialogcustomroute.cpp \
    dialogabout.cpp

HEADERS  += mainwindow.h \
    networkutils.h \
    arpwatcher.h \
    dialogcustomroute.h \
    dialogabout.h

FORMS    += mainwindow.ui \
    dialogcustomroute.ui \
    dialogabout.ui

RESOURCES += \
    ressources.qrc

OTHER_FILES += \
    README \
    ressources.rc

















