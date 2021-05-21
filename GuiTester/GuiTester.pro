QT += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    evocomputethread.cpp \
    main.cpp \
    evomaindialog.cpp

HEADERS += \
    evocomputethread.h \
    evomaindialog.h

FORMS += \
    evomaindialog.ui

RESOURCES += \
  resource.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../EvoCalLib/release/ -lEvoCalLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../EvoCalLib/debug/ -lEvoCalLib

INCLUDEPATH += $$PWD/../EvoCalLib
DEPENDPATH += $$PWD/../EvoCalLib


RC_ICONS = resource/home.ico

DISTFILES += \
    resource/home.ico
