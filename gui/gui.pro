TEMPLATE = app
TARGET = gui
QT += core \
    gui \
    xml \
    xmlpatterns \
    network \
    svg \
    opengl \
    webkit \
    script
HEADERS += qdockwidget2.h \
    gui.h
SOURCES += main.cpp \
    gui.cpp
FORMS += gui.ui
RESOURCES += 
INCLUDEPATH += ../spellchecklib/

# it's not selectively added. TODO
debug:LIBS += -L../spellchecklib/Debug/ \
    -lspellchecklib \
    -lfst
release:
