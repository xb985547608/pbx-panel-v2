#-------------------------------------------------
#
# Project created by QtCreator 2013-03-17T23:06:38
#
#-------------------------------------------------

QT       += core gui network sql widgets multimedia

TRANSLATIONS+=en.ts

TARGET = PbxPanel
TEMPLATE = app
INCLUDEPATH += $$PWD
#DEFINES += EEPBX
#DEFINES += OEM
DEFINES += WHITE
#DEFINES += HBNYJT
#DEFINES += JXLC
#DEFINES += LOCKTEL
DEFINES += V5

DESTDIR = $$PWD/bin

#win32: LIBS += -lole32 -lws2_32 -L$$PWD/lib -lcwebpage -lrabbitmq "D:\Qt\Qt5.1.1\5.1.1\mingw48_32\bin\Qt5Ftp.dll"
win32: LIBS += -lole32 -lws2_32 -L$$PWD/lib -lrabbitmq#-lcwebpage 暂时不用这个库

SOURCES += main.cpp \
    misc/tools.cpp \
    misc/singleton.cpp \
    misc/logger.cpp \
    message_queue/MessageClient.cpp \
    misc/Config.cpp \
#    ui/base/uiwebbrowser.cpp \
    ui/base/tablewidget.cpp \
    ui/base/multitablewidgets.cpp \
    ui/base/misccomponents.cpp \
    ui/base/dingobutton.cpp \
    message_queue/RPCCommand.cpp \
    pbx/pbx.cpp \
    pbx/PhoneBook.cpp \
    ui/LoginForm.cpp \
    misc/dbwrapper.cpp \
    ui/base/uiextensionbase.cpp \
    ui/mainwidget/SelectedExtensZone.cpp \
    ui/mainwidget/RinggroupsZone.cpp \
    ui/mainwidget/QueueZone.cpp \
    ui/mainwidget/OperatorZone.cpp \
    ui/mainwidget/MainWidget.cpp \
    ui/mainwidget/FastFuncZone.cpp \
    ui/mainwidget/ExtensionsZone.cpp \
    ui/MainWindow.cpp \
    ui/settings/SettingsWidget.cpp \
    ui/settings/OpSettingsZone.cpp \
    pbx/EventModel.cpp \
    ui/settings/RecordingFileZone.cpp \
    ui/base/BaseWidget.cpp \
    ui/phonebook/UiPhoneBook.cpp \
    ui/eventplan/EventPlanEditor.cpp \
    ui/base/SectionZone.cpp \
    ui/phonebook/UiPhoneBookList.cpp \
    ui/settings/PlanListZone.cpp \
    ui/event/PlanZone.cpp \
    ui/event/EventWidget.cpp \
    ui/event/EventExtensionsZone.cpp \
    ui/event/FunctionZone.cpp \
    ui/event/DescriptionZone.cpp \
    ui/event/EventExten.cpp \
    ui/event/EventChooser.cpp \
    ui/calllog/CallLogWidget.cpp \
    ui/eventlog/EventLog.cpp \
    misc/ftpclient.cpp \
    ui/dialogs/DialPad.cpp \
    ui/dialogs/SMSWidget.cpp \
    ui/dialogs/PlaySoundWidget.cpp \
    ui/meetme/MeetMeWidget.cpp \
    ui/settings/TimingBroadcastZone.cpp \
    ui/timedbroadcast/TimedBroadcastEditor.cpp \
    ui/dialogs/dlgemergencycall.cpp \
    misc/qftp.cpp \
    misc/qurlinfo.cpp \
    misc/UDPNotifier.cpp \
    misc/TCPNotifier.cpp

HEADERS += \
    message_queue/amqp_framing.h \
    message_queue/amqp.h \
    misc/tools.h \
    misc/singleton.h \
    misc/logger.h \
#    misc/cwebpage.h \
    message_queue/MessageClient.h \
    misc/Config.h \
#    ui/base/uiwebbrowser.h \
    ui/base/tablewidget.h \
    ui/base/multitablewidgets.h \
    ui/base/misccomponents.h \
    ui/base/dingobutton.h \
    message_queue/RPCCommand.h \
    pbx/pbx.h \
    pbx/PhoneBook.h \
    ui/LoginForm.h \
    misc/dbwrapper.h \
    ui/base/uiextensionbase.h \
    ui/mainwidget/SelectedExtensZone.h \
    ui/mainwidget/RinggroupsZone.h \
    ui/mainwidget/QueueZone.h \
    ui/mainwidget/OperatorZone.h \
    ui/mainwidget/MainWidget.h \
    ui/mainwidget/FastFuncZone.h \
    ui/mainwidget/ExtensionsZone.h \
    ui/MainWindow.h \
    ui/settings/SettingsWidget.h \
    ui/settings/OpSettingsZone.h \
    pbx/EventModel.h \
    ui/settings/RecordingFileZone.h \
    ui/base/BaseWidget.h \
    ui/phonebook/UiPhoneBook.h \
    ui/eventplan/EventPlanEditor.h \
    ui/base/SectionZone.h \
    ui/phonebook/UiPhoneBookList.h \
    ui/settings/PlanListZone.h \
    ui/event/EventWidget.h \
    ui/event/PlanZone.h \
    ui/event/EventExtensionsZone.h \
    ui/event/FunctionZone.h \
    ui/event/DescriptionZone.h \
    ui/event/EventExten.h \
    ui/event/EventChooser.h \
    ui/calllog/CallLogWidget.h \
    ui/eventlog/EventLog.h \
    misc/ftpclient.h \
    ui/dialogs/DialPad.h \
    ui/dialogs/SMSWidget.h \
    ui/dialogs/PlaySoundWidget.h \
    ui/meetme/MeetMeWidget.h \
    ui/settings/TimingBroadcastZone.h \
    ui/timedbroadcast/TimedBroadcastEditor.h \
    ui/dialogs/dlgemergencycall.h \
    misc/qftp.h \
    misc/qurlinfo.h \
    misc/UDPNotifier.h \
    misc/TCPNotifier.hpp

RESOURCES += \
    pbx_panel.qrc

OTHER_FILES += \
    stylesheet.qss \
    en.qm
RC_FILE = panel.rc

include ( $$PWD/VideoLinkage/VideoLinkage.pri )
