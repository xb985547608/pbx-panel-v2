win32: LIBS += -L$$PWD/sdk/lib/ -llibvlccore -llibvlc
INCLUDEPATH += $$PWD/sdk/include
DEPENDPATH += $$PWD/sdk/include

HEADERS += \
    $$PWD/videolinkagewidget.h \
    $$PWD/videowidget.h \
    $$PWD/playcontrol.h \
    $$PWD/settingsdialog.h

SOURCES += \
    $$PWD/videolinkagewidget.cpp \
    $$PWD/videowidget.cpp \
    $$PWD/playcontrol.cpp \
    $$PWD/settingsdialog.cpp

FORMS += \
    $$PWD/extensioncfg.ui
