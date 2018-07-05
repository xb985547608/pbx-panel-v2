#ifndef SMSWIDGET_H
#define SMSWIDGET_H

#include <QDialog>
#include <QBoxLayout>
#include <QPlainTextEdit>
#include <QStandardItemModel>

class SMSWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SMSWidget(QWidget *parent = 0, bool showTemplate = true);
    QString getSms() { return mSmsContent->toPlainText(); }
    void setSms(QString text) { mSmsContent->setPlainText(text); }
    
signals:
    
public slots:
private slots:
    void onBtnSaveAsTemplateClicked();
    void onBtnSendClicked();
    void onBtnClearClicked();
    void onTemplateListClicked(const QModelIndex&);

private:
    QBoxLayout* getLeftLayout();
    QBoxLayout *getRightLayout();
    QPlainTextEdit* mSmsContent;
    QStandardItemModel* mTemplateListModel;
    void loadSmsTemplates();
    int mSelectedTemplateId; //选中的template id号
    bool mShowTemplate; //是否显示模板
};

#endif // SMSWIDGET_H
