#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QScrollArea>
#include <QSettings>

namespace Ui {
class ExtensionCfgWidget;
}

#define INIPREFIX "ExtensionURL/"

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    bool contains(const QString &key) const;
    QStringList value(const QString &key) const;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void addConfigWidget(QString number, QStringList urls);
    QFrame *makeConfigWidget();

    void append(QString number, QStringList urls);
    void remove(QString number);
    void refresh();

public slots:

private:
    QMap<QFrame *, Ui::ExtensionCfgWidget *> mWidgetOfUi;
    QScrollArea *mpScrollArea;
    QWidget *mpBaseWidget;
    QSettings *mpSettings;
};

#endif // SETTINGSDIALOG_H
