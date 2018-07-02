#include "settingsdialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "ui_extensioncfg.h"
#include <QScrollBar>
#include <QDebug>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtensionCfgWidget)
{
    mpBaseWidget = new QWidget(this);
    mpScrollArea = new QScrollArea(this);
    mpScrollArea->setWidget(mpBaseWidget);
    mpScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mpScrollArea->setFrameStyle(QFrame::NoFrame);

    QLabel *header = new QLabel("视频联动", this);
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("border: solid black; border-width:0 0 2 0");
    QFont font;
    font.setPixelSize(24);
    font.setBold(true);
    header->setFont(font);
    QPushButton *footer = new QPushButton("添加", this);
    footer->setFixedSize(80, 30);

    QVBoxLayout *col = new QVBoxLayout();
    setLayout(col);
    col->addWidget(header);
    col->addWidget(mpScrollArea);
    col->addWidget(footer, 0, Qt::AlignCenter);

    QVBoxLayout *listLayout = new QVBoxLayout();
    listLayout->setSpacing(4);
    listLayout->setContentsMargins(0, 0, 4, 0);
    mpBaseWidget->setLayout(listLayout);

    setMinimumWidth(450);
    resize(450, 500);
    mpBaseWidget->setMinimumWidth(409);

    mpSettings = new QSettings("settings.ini", QSettings::IniFormat, this);
    mpSettings->beginGroup(INIPREFIX);
    foreach (QString key, mpSettings->allKeys()) {
        QStringList list = mpSettings->value(key).toStringList();
        if (list.isEmpty())
            continue;

        append(key, list);
    }
    mpSettings->endGroup();

    connect(footer, &QPushButton::clicked, this, [this](){
        append();
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

void SettingsDialog::append(QString extenName, QStringList urls)
{
    QWidget *cfgWidget = makeExtensionConfigWidget(extenName, urls);
    mpBaseWidget->layout()->addWidget(cfgWidget);


    /* 新增项后调整长度 */
    QSize size(mpBaseWidget->width(),
               mpBaseWidget->children().count() * cfgWidget->height());
    mpBaseWidget->resize(size);
}

QWidget *SettingsDialog::makeExtensionConfigWidget(QString extenName, QStringList urls)
{
    QFrame *cfgWidget = new QFrame(this);
    ui->setupUi(cfgWidget);
    ui->extenNum->setText(extenName);
    ui->url1->setText(urls.count() > 0 ? urls.at(0) : "");
    ui->url2->setText(urls.count() > 1 ? urls.at(1) : "");
    ui->url1->setEnabled(!ui->extenNum->text().isEmpty());
    ui->url2->setEnabled(!ui->extenNum->text().isEmpty());
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
        addExtension(exten->text(), QStringList()
                     << url1->text()
                     << url2->text());
    });
    connect(ui->remove, &QPushButton::clicked, this, [this, exten, cfgWidget](){
        if (mpSettings->contains(INIPREFIX + exten->text()))
            if (QMessageBox::question(this, "询问", "是否删除该分机在配置文件里的信息") == QMessageBox::No)
                return;
        delExtension(exten->text());
        delete cfgWidget;
    });

    cfgWidget->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    return cfgWidget;
}

void SettingsDialog::delExtension(QString extenName)
{
    mpSettings->remove(INIPREFIX + extenName);
}

void SettingsDialog::addExtension(QString extenName, QStringList urls)
{
    mpSettings->setValue(INIPREFIX + extenName, urls);
}
