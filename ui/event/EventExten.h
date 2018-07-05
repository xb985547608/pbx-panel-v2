#ifndef EVENTEXTEN_H
#define EVENTEXTEN_H

#include "ui/base/uiextensionbase.h"
#include <QtGui>

class EventExten : public UiExtensionBase
{
    Q_OBJECT
public:
    explicit EventExten(PBX::Extension e, QWidget* parent = 0);
    void onStateChanged(int state);

signals:
    void onStateChangedSignal(int state);

public slots:
    void onStateChangedSlot(int state);
private slots:
    void clickedSlot();
};

#endif // EVENTEXTEN_H
