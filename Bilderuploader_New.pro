#-------------------------------------------------
#
# Project created by QtCreator 2017-02-10T01:09:32
#
#-------------------------------------------------

QT      += core gui multimedia
QT      += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bilderuploader
TEMPLATE = app


SOURCES += main.cpp\
        bilderuploader.cpp \
    bildbetrachter.cpp \
    bilderapp.cpp \
    bilder.cpp \
    bilderftpdlg.cpp \
    qftp.cpp \
    qurlinfo.cpp \
    qdevicewatcher.cpp \
    qdevicewatcher_win32.cpp \
    bliderbutton.cpp \
    bilderfile.cpp

HEADERS  += bilderuploader.h \
    bildbetrachter.h \
    resource.h \
    bilderapp.h \
    bilder.h \
    bilderftpdlg.h \
    qftp.h \
    qurlinfo.h \
    qdevicewatcher.h \
    qdevicewatcher_p.h \
    bliderbutton.h \
    bilderfile.h

RESOURCES += Resource.qrc

RC_ICONS+= res/icon2.ico
