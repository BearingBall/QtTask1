include(openglwindow.pri)

CONFIG += c++11
LIBS   += -lopengl32
LIBS   += -lglu32

SOURCES += \
    main.cpp


QT += widgets


target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

HEADERS += \
    cube.h \
    customColorDialog.h \
    icosphere.h \
    objectAdapter.h \
    shaders.h
