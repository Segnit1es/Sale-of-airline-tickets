QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    database.cpp \
    errorhandler.cpp \
    flight.cpp \
    flightavltree.cpp \
    flightdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    passenger.cpp \
    passengerdialog.cpp \
    passengerhashtable.cpp \
    textsearch.cpp \
    ticket.cpp \
    ticketdialog.cpp \
    ticketingsystem.cpp \
    ticketlist.cpp \
    ticketrecord.cpp \
    validator.cpp

HEADERS += \
    TicketNode.h \
    database.h \
    errorhandler.h \
    flight.h \
    flightavltree.h \
    flightdialog.h \
    flightnode.h \
    mainwindow.h \
    passenger.h \
    passengerdialog.h \
    passengerhashtable.h \
    textsearch.h \
    ticket.h \
    ticketdialog.h \
    ticketingsystem.h \
    ticketlist.h \
    ticketrecord.h \
    validator.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    styles/darkstyle.qss
