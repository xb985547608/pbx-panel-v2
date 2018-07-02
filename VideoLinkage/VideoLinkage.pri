LIBS       += -Lh:/Libs/vlc-qt/lib -lVLCQtCore -lVLCQtWidgets
INCLUDEPATH += h:/Libs/vlc-qt/include

HEADERS += \
    $$PWD/videolinkagewidget.h \
    $$PWD/videowidget.h \
#    $$PWD/playcontrol.h \
    $$PWD/settingsdialog.h

SOURCES += \
    $$PWD/videolinkagewidget.cpp \
    $$PWD/videowidget.cpp \
#    $$PWD/playcontrol.cpp \
    $$PWD/settingsdialog.cpp

FORMS += \
    $$PWD/extensioncfg.ui
