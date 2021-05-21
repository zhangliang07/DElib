CONFIG -= qt

TEMPLATE = lib
DEFINES += EVOOPERATOR_LIBRARY

CONFIG += c++17 dll

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
		../EvoCalLib/EvoOperator.h \
		evooperatorde.h

SOURCES += \
    EvoOperator.cpp \
    evooperatorde.cpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
