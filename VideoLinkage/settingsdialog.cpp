#include "settingsdialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "ui_extensioncfg.h"
#include <QScrollBar>
#include <QDebug>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    /************************ top ************************/
    QLabel *header = new QLabel("视频联动", this);
    header->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPixelSize(24);
    font.setBold(true);
    header->setFont(font);

    /************************ center ************************/
    QLabel *topLine = new QLabel(this);
    topLine->setFixedHeight(2);
    topLine->setStyleSheet("background: red");

    mpBaseWidget = new QWidget(this);
    mpScrollArea = new QScrollArea(this);
    mpScrollArea->setWidget(mpBaseWidget);
    mpScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mpScrollArea->setFrameStyle(QFrame::NoFrame);
    mpScrollArea->setAlignment(Qt::AlignTop);

    QLabel *bottomLine = new QLabel(this);
    bottomLine->setFixedHeight(2);
    bottomLine->setStyleSheet("background: red");

    /************************ center ************************/
    QPushButton *footer = new QPushButton("添加", this);
    footer->setFixedSize(80, 30);

    /************************ Layout ************************/
    QVBoxLayout *col = new QVBoxLayout(this);
    col->addWidget(header);
    col->addWidget(topLine);
    col->addWidget(mpScrollArea);
    col->addWidget(bottomLine);
    col->addWidget(footer, 0, Qt::AlignCenter);

    QVBoxLayout *listLayout = new QVBoxLayout(mpBaseWidget);
    listLayout->setSpacing(4);
    listLayout->setContentsMargins(0, 0, 0, 0);

    setFixedSize(450, 500);
    mpBaseWidget->setMinimumWidth(409);

    /* 初始化配置信息列表 */
    mpSettings = new QSettings("settings.ini", QSettings::IniFormat, this);
    mpSettings->beginGroup(INIPREFIX);
    foreach (QString key, mpSettings->allKeys()) {
        addConfigWidget(key, mpSettings->value(key).toStringList());
    }
    mpSettings->endGroup();

    connect(footer, &QPushButton::clicked, this, [this](){
        makeConfigWidget();
    });
}

bool SettingsDialog::contains(const QString &key) const
{
    return mpSettings->contains(INIPREFIX + key);
}

QStringList SettingsDialog::value(const QString &key) const
{
    QStringList urls;
    foreach (QString url, mpSettings->value(INIPREFIX + key).toStringList()) {
        if (!url.isEmpty())
            urls.append(url);
    }
    return urls;
}

void SettingsDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    mpBaseWidget->resize(mpScrollArea->viewport()->width(), mpBaseWidget->size().height());
}

void SettingsDialog::addConfigWidget(QString extenName, QStringList urls)
{
    qDebug() << urls;
    QFrame *cfgWidget = makeConfigWidget();
    if (!extenName.isEmpty()) {
        mWidgetOfUi[cfgWidget]->extenNum->setText(extenName);
        mWidgetOfUi[cfgWidget]->url1->setText(urls.count() > 0 ? urls.at(0) : "");
        mWidgetOfUi[cfgWidget]->url2->setText(urls.count() > 1 ? urls.at(1) : "");
    }
}

QFrame *SettingsDialog::makeConfigWidget()
{
    Ui::ExtensionCfgWidget *ui = new Ui::ExtensionCfgWidget;
    QFrame *cfgWidget = new QFrame(this);
    cfgWidget->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    mWidgetOfUi.insert(cfgWidget, ui);
    mpBaseWidget->layout()->addWidget(cfgWidget);

    ui->setupUi(cfgWidget);
    ui->extenNum->setEnabled(true);
    ui->url1->setEnabled(false);
    ui->url2->setEnabled(false);
    ui->save->setEnabled(false);

    QLineEdit *exten = ui->extenNum;
    QLineEdit *url1 = ui->url1;
    QLineEdit *url2 = ui->url2;
    QPushButton *save = ui->save;

    connect(ui->extenNum, &QLineEdit::textChanged, this, [this, exten, url1, url2, save](){
        bool result = !exten->text().isEmpty() && (!url1->text().isEmpty() || !url2->text().isEmpty());
        save->setEnabled(result);
        url1->setEnabled(!exten->text().isEmpty());
        url2->setEnabled(!exten->text().isEmpty());
    });
    connect(ui->url1, &QLineEdit::textChanged, this, [this, url1, url2, save](){
        bool result = !url1->text().isEmpty() || !url2->text().isEmpty();
        save->setEnabled(result);
    });
    connect(ui->url2, &QLineEdit::textChanged, this, [this, url1, url2, save](){
        bool result = !url1->text().isEmpty() || !url2->text().isEmpty();
        save->setEnabled(result);
    });
    connect(ui->save, &QPushButton::clicked, this, [this, exten, url1, url2, save](){
        save->setEnabled(false);
        append(exten->text(), QStringList()
                     << url1->text()
                     << url2->text());
    });
    connect(ui->remove, &QPushButton::clicked, this, [this, exten, cfgWidget](){
        if (mpSettings->contains(INIPREFIX + exten->text())) {
            if (QMessageBox::question(this, "询问", "是否删除该分机在配置文件里的信息") == QMessageBox::No)
                return;
        }
        remove(exten->text());
        mWidgetOfUi.remove(cfgWidget);
        delete cfgWidget;
        refresh();
    });

    refresh();
    return cfgWidget;
}

void SettingsDialog::append(QString number, QStringList urls)
{
    mpSettings->setValue(INIPREFIX + number, urls);
}

void SettingsDialog::remove(QString number)
{
    if (!mpSettings->contains(INIPREFIX + number))
        return;

    mpSettings->remove(INIPREFIX + number);
}

void SettingsDialog::refresh()
{
    if (mWidgetOfUi.count() == 0) {

        mpBaseWidget->setFixedSize(mpBaseWidget->width(), 4);
    } else {

        int h = mWidgetOfUi.count() * mWidgetOfUi.firstKey()->height();
        h += (mWidgetOfUi.count() - 1) * mpBaseWidget->layout()->spacing();
        mpBaseWidget->setFixedSize(mpBaseWidget->width(), h);
    }
}
