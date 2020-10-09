QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opengl1
TEMPLATE = app
DESTDIR = ~/build_dir/opengl

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

SOURCES += \
        main.cpp \
        openglwidget.cpp \
        mainwidget.cpp \
        camera.cpp

HEADERS += \
        openglwidget.h \
        mainwidget.h \
        camera.h

FORMS += \
        openglwidget.ui \
        mainwidget.ui

contains( CONFIG, qt ) {
    MOC_DIR = ./.moc
    UI_DIR  = ./.ui
    RCC_DIR = ./.rcc
    INCLUDEPATH += ./.ui
}
OBJECTS_DIR = ./.o

RESOURCES += \
    sources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=


