#include "LoginForm.h"
#include "misc/Config.h"
#include "base/misccomponents.h"
#include "misc/dbwrapper.h"
#include "message_queue/RPCCommand.h"
#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent) :
    QDialog(parent)
{
    Config* cfg = Config::Instance();
    this->setWindowTitle(Config::Instance()->Window_Title);
    setFixedSize(800, 400);
    mLoginIsOk = false;
    QVBoxLayout* rootBox = new QVBoxLayout(this);

    QWidget* header = MiscComponents::getHeader(this);
    rootBox->addWidget(header);
    rootBox->addStretch();

    QLabel* label = new QLabel(this);
    label->setPixmap(QPixmap(cfg->Res_image_splash));
    label->setAlignment(Qt::AlignCenter);
    rootBox->addWidget(label);
    rootBox->addStretch();


    QLabel* labelPbxIp = new QLabel(tr("用户名"), this);
    mLineEditUsername = new QLineEdit(this);
    //mLineEditPbxIp->setText(DbWrapper::getSystemValue("pbx_ip"));
    QLabel* labelPassword = new QLabel(tr("请输入密码"), this);
    mLineEditPassword = new QLineEdit(this);
    mLineEditPassword->setEchoMode(QLineEdit::Password);
    QPushButton* btnOk = new QPushButton(tr("确定"), this);
    QPushButton* btnChangePassword = new QPushButton(tr("修改密码"), this);

    QGridLayout* gridLayout = new QGridLayout;
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setSpacing(20);
    hbox->addStretch();
    hbox->addLayout(gridLayout);
    hbox->addStretch();
    rootBox->addLayout(hbox);
    gridLayout->addWidget(labelPbxIp, 0, 0, 1, 1);
    gridLayout->addWidget(mLineEditUsername, 0, 1, 1, 2);
    gridLayout->addWidget(labelPassword, 1, 0, 1, 1);
    gridLayout->addWidget(mLineEditPassword, 1, 1, 1, 2);
    gridLayout->addWidget(btnOk, 2, 0, 1, 1);
    gridLayout->addWidget(btnChangePassword, 2, 2, 1, 1);
    btnChangePassword->setVisible(false);


    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);
    setLayout(rootBox);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onBtnLoginClicked()));
    connect(btnChangePassword, SIGNAL(clicked()), this, SLOT(onBtnChangePasswordClicked()));
}

void LoginForm::onBtnLoginClicked()
{
    if (RPCCommand::operatorLogin(mLineEditUsername->text(), mLineEditPassword->text())) {
        mLoginIsOk = true;
        //DbWrapper::setSystemValue("pbx_ip", mLineEditPbxIp->text());
        accept();
    } else {
        QMessageBox::about(this, tr("出错啦"), "密码不正确");
        return;
    }
}

void LoginForm::closeEvent(QCloseEvent *)
{
    return;
}

void LoginForm::onBtnChangePasswordClicked()
{
    ChangePassWordDlg* dialog = new ChangePassWordDlg(this);
    dialog->show();
}



ChangePassWordDlg::ChangePassWordDlg(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout* rootBox = new QVBoxLayout(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);
    setLayout(rootBox);

    QHBoxLayout* hbox = new QHBoxLayout;
    QLabel* label = new QLabel(tr("请输入当前密码："), this);
    mLineEditOldPassword = new QLineEdit(this);
    mLineEditOldPassword->setEchoMode(QLineEdit::Password);
    hbox->addWidget(label);
    hbox->addWidget(mLineEditOldPassword);
    rootBox->addLayout(hbox);

    hbox = new QHBoxLayout;
    label = new QLabel(tr("请输入新密码："), this);
    mLineEditPassword = new QLineEdit(this);
    mLineEditPassword->setEchoMode(QLineEdit::Password);
    hbox->addWidget(label);
    hbox->addWidget(mLineEditPassword);
    rootBox->addLayout(hbox);

    hbox = new QHBoxLayout;
    label = new QLabel(tr("请重复输入新密码："), this);
    mLineEditRePassword = new QLineEdit(this);
    mLineEditRePassword->setEchoMode(QLineEdit::Password);
    hbox->addWidget(label);
    hbox->addWidget(mLineEditRePassword);
    rootBox->addLayout(hbox);

    hbox = new QHBoxLayout;
    QPushButton* btnOk = new QPushButton(tr("确定"), this);
    QPushButton* btnCancel = new QPushButton(tr("取消"), this);
    hbox->addWidget(btnOk);
    hbox->addWidget(btnCancel);
    rootBox->addLayout(hbox);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onBtnOkClicked()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(onBtnCancelClicked()));
}

void ChangePassWordDlg::onBtnOkClicked()
{
}

void ChangePassWordDlg::onBtnCancelClicked()
{
    close();
}
