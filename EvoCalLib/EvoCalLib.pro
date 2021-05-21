CONFIG -= qt

TEMPLATE = lib
DEFINES += EVOCALLIB_LIBRARY

CONFIG += c++17 dll

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
		EvoObjFunction.h \
		EvoOperator.h \
		evocallib.h \
		evocore.h

SOURCES += \
    evocallib.cpp \
    evocore.cpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
