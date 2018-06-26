#ifndef DIALPAD_H
#define DIALPAD_H

#include <QDialog>
#include <QLineEdit>

class DialPad : public QDialog
{
    Q_OBJECT
public:
    explicit DialPad(bool showPhonebook = true, QWidget *parent = 0);
    QString getNumber() { return mNumberInput->text(); }
    bool isPhonebookSelected() { return mPhonebookSelected; }
    
signals:
    
public slots:
    void onBtnBackspaceClicked();
    void onBtnDigitClicked();
    void onBtnCallClicked();
    void onBtnPhonebookClicked();
private :
    QPushButton* getDigitBtn(QString value);
    QLineEdit* mNumberInput;
    bool mPhonebookSelected;
};

class DialBtnData : public QObjectUserData
{
public:
    DialBtnData(QString digit) :
        mDigit(digit)
    {}
    ~DialBtnData();
    QString getData() { return mDigit; }
private :
    QString mDigit;
};

#endif // DIALPAD_H
