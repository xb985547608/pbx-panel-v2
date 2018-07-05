#ifndef DINGOBUTTON_H
#define DINGOBUTTON_H

#include <QGroupBox>
#include <QVBoxLayout>

class DingoButton : public QGroupBox
{
    Q_OBJECT
public:
    typedef enum _ToggleState { Default, Toggled } ToggleState;
    explicit DingoButton(QWidget *parent = 0);
    bool isToggled() { return mToggled; }
    void setToggleable(bool toggleable) { mToggleable = toggleable; }
    void toggle(bool toggle);
    void setLayout(QLayout *);
    void setBackgroundStyleSheet(ToggleState, const QString &styleSheet);
    
signals:
    void widgetClickedSignal();
    
private slots:

protected:
    virtual void mousePressEvent(QMouseEvent *);

    void refreshBackground();
    bool mToggleable;
    bool mToggled;
    QString mStyleSheetToggled;
    QString mStyleSheetNormal;
    QWidget* mContainer;
};

#endif // DINGOBUTTON_H
