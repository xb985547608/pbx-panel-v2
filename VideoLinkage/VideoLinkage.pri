win32: LIBS += -L$$PWD/sdk/lib/ -llibvlccore -llibvlc
INCLUDEPATH += $$PWD/sdk/include
INCLUDEPATH += $$PWD/sdk/include/vlc/plugins
DEPENDPATH += $$PWD/sdk/include

HEADERS += \
    $$PWD/videolinkagewidget.h \
    $$PWD/videowidget.h \
    $$PWD/playcontrol.h \
    $$PWD/settingsdialog.h \
    $$PWD/devicelist.h

SOURCES += \
    $$PWD/videolinkagewidget.cpp \
    $$PWD/videowidget.cpp \
    $$PWD/playcontrol.cpp \
    $$PWD/settingsdialog.cpp \
    $$PWD/devicelist.cpp

FORMS += \
    $$PWD/extensioncfg.ui

RESOURCES += \
    $$PWD/videolinkage.qrc
