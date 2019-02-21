QT += xml

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

HEADERS += \
    $$PWD/src/parser.h \
    $$PWD/src/dictionarymanager.h \
    $$PWD/src/astnode.h \
    $$PWD/src/parsermanager.h

SOURCES += \
    $$PWD/src/parser.cpp \
    $$PWD/src/dictionarymanager.cpp \
    $$PWD/src/astnode.cpp \
    $$PWD/src/parsermanager.cpp
