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
    void append(QString extenName = "", QStringList urls = QStringList());
    QWidget *makeExtensionConfigWidget(QString extenName, QStringList urls);
    void delExtension(QString extenName);
    void addExtension(QString extenName, QStringList urls);

public slots:
private:
    Ui::ExtensionCfgWidget *ui;
    QScrollArea *mpScrollArea;
    QWidget *mpBaseWidget;
    QSettings *mpSettings;
};

#endif // SETTINGSDIALOG_H
