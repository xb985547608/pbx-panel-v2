#ifndef EVENTCHOOSER_H
#define EVENTCHOOSER_H

#include <QDialog>

class EventChooser : public QDialog
{
    Q_OBJECT
public:
    explicit EventChooser(QWidget *parent = 0);
    QString whichSelected() { return mWhichSelected; }
    QString selectedId() { return mSelectedId; }

signals:    
public slots:
    void onEventPlanClicked();
    void onEventClicked();
private:
    QString mWhichSelected; //"plan" or "event"
    QString mSelectedId;  //选择的plan或event的id
};

#endif // EVENTCHOOSER_H
