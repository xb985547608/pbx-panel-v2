#include "DialPad.h"
#include "ui/base/misccomponents.h"
#include "misc/Config.h"
#include "misc/logger.h"
#include <QMessageBox>

DialPad::DialPad(bool showPhonebook, QWidget *parent) :
    QDialog(parent)
{
    mPhonebookSelected = false;
    QVBoxLayout* rootbox = new QVBoxLayout;
    rootbox->setMargin(0);
    rootbox->setSpacing(0);
    QGroupBox* rootWidget = MiscComponents::getGroupBox(rootbox, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(rootWidget);
    setLayout(vbox);
    setMinimumSize(200, 400);

    QToolButton* phonebookBtn = new QToolButton(this);
    MiscComponents::setToolButton(phonebookBtn, Config::Instance()->Res_icon_phone_book_small, tr("通讯录"), QSize(16, 16), Qt::ToolButtonTextBesideIcon);
    phonebookBtn->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(phonebookBtn, SIGNAL(clicked()), this, SLOT(onBtnPhonebookClicked()));
    if (showPhonebook)
        rootbox->addWidget(phonebookBtn, 1);
    else
        phonebookBtn->setVisible(false);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    mNumberInput = new QLineEdit(this);
    mNumberInput->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QPushButton* btnErase = new QPushButton(tr("Backspace"), this);
    btnErase->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    connect(btnErase, SIGNAL(clicked()), this, SLOT(onBtnBackspaceClicked()));
    hbox->addWidget(mNumberInput);
    hbox->addWidget(btnErase);
    rootbox->addLayout(hbox, 1);

    QGridLayout* padLayout = new QGridLayout;
    padLayout->addWidget(getDigitBtn("1"), 0, 0, 1, 1);
    padLayout->addWidget(getDigitBtn("2"), 0, 1, 1, 1);
    padLayout->addWidget(getDigitBtn("3"), 0, 2, 1, 1);
    padLayout->addWidget(getDigitBtn("4"), 1, 0, 1, 1);
    padLayout->addWidget(getDigitBtn("5"), 1, 1, 1, 1);
    padLayout->addWidget(getDigitBtn("6"), 1, 2, 1, 1);
    padLayout->addWidget(getDigitBtn("7"), 2, 0, 1, 1);
    padLayout->addWidget(getDigitBtn("8"), 2, 1, 1, 1);
    padLayout->addWidget(getDigitBtn("9"), 2, 2, 1, 1);
    padLayout->addWidget(getDigitBtn("*"), 3, 0, 1, 1);
    padLayout->addWidget(getDigitBtn("0"), 3, 1, 1, 1);
    padLayout->addWidget(getDigitBtn("#"), 3, 2, 1, 1);

    rootbox->addLayout(padLayout, 4);

    QPushButton* btnDial = new QPushButton(tr("确定"));
    btnDial->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(btnDial, SIGNAL(clicked()), this, SLOT(onBtnCallClicked()));
    rootbox->addWidget(btnDial, 1);
}

QPushButton* DialPad::getDigitBtn(QString value)
{
    QPushButton* btnDigit = new QPushButton(value, this);
    btnDigit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    DialBtnData* data = new DialBtnData(value);
    btnDigit->setUserData(0, data);
    connect(btnDigit, SIGNAL(clicked()), this, SLOT(onBtnDigitClicked()));
    return btnDigit;
}

void DialPad::onBtnPhonebookClicked()
{
    mPhonebookSelected = true;
    accept();
}

void DialPad::onBtnBackspaceClicked()
{
    QString number = mNumberInput->text();
    if (number.length() > 0) {
        number = number.left(number.length() - 1);
    }
    mNumberInput->setText(number);
}

void DialPad::onBtnDigitClicked()
{
    QObject* object = QObject::sender();
    QPushButton* btnDigit = (QPushButton*)qobject_cast<QPushButton*>(object);
    QString digit = ((DialBtnData*)btnDigit->userData(0))->getData();
    QString number = mNumberInput->text() + digit;
    mNumberInput->setText(number);
}

void DialPad::onBtnCallClicked()
{
    if (mNumberInput->text() == "") {
        QMessageBox::about(this, tr("错误"), tr("请输入号码"));
        return;
    }
    accept();
}

DialBtnData::~DialBtnData()
{
    LOG(Logger::Debug, "DialBtnData deleting\n");
}
