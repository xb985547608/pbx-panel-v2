#ifndef OPSETTINGSZONE_H
#define OPSETTINGSZONE_H

#include <QtGui>
#include "SettingsWidget.h"

class OpSettingsZone : public QWidget
{
    Q_OBJECT
public:
    explicit OpSettingsZone(SettingsWidget* sw, QWidget* parent = 0);
protected:
    void timerEvent(QTimerEvent *);
private slots:
    void btnSubmitSlot();
private :
    SettingsWidget* mSettingsWidget;
    void loadOpNumber();
    void saveOpNumber();
    QLineEdit* mOp1Widget;
    QLineEdit* mOp2Widget;
    QPushButton* mBtnSubmit;
};

#endif // OPSETTINGSZONE_H
