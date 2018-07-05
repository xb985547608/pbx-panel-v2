#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QLineEdit>

class LoginForm : public QDialog
{
    Q_OBJECT
public:
    explicit LoginForm(QWidget *parent = 0);
    
signals:
    
public slots:
    void onBtnLoginClicked();
    void onBtnChangePasswordClicked();
protected :
    void closeEvent(QCloseEvent *);
private :
    QLineEdit* mLineEditPassword;
    QLineEdit* mLineEditUsername;
    bool mLoginIsOk;
};

class ChangePassWordDlg : public QDialog
{
    Q_OBJECT
public :
    explicit ChangePassWordDlg(QWidget* parent = 0);
public slots:
    void onBtnOkClicked();
    void onBtnCancelClicked();
private :
    QLineEdit* mLineEditOldPassword;
    QLineEdit* mLineEditPassword;
    QLineEdit* mLineEditRePassword;
};

#endif // LOGINFORM_H
