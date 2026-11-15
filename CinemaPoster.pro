QT       += core gui network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    moviedatabase.cpp \
    user.cpp \
    logindialog.cpp \
    bookingdialog.cpp

HEADERS += \
    mainwindow.h \
    moviedatabase.h \
    user.h \
    logindialog.h \
    bookingdialog.h

# Иконка приложения
RC_ICONS = cinema_icon.ico

# Стили
DISTFILES += \
    style.qss

# Изображения фильмов (добавьте свои изображения)
DISTFILES += \
    images/interstellar.jpg \
    images/inception.jpg \
    images/matrix.jpg \
    images/joker.jpg \
    images/avatar.jpg

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
