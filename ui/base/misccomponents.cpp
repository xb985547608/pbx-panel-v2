#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "misccomponents.h"
#include "misc/Config.h"
#include "misc/logger.h"

MiscComponents::MiscComponents()
{
}

void MiscComponents::setPushButton(QPushButton *btn, QString imgRes, QString text, QString styleSheet)
{
    QPixmap pix(imgRes);
    QSize size = pix.size();
    MiscComponents::setPushButton(btn, imgRes, text, size, styleSheet);
}

void MiscComponents::setPushButton(QPushButton* btn, QString imgRes, QString text, QSize size, QString styleSheet)
{
    QIcon icon;
    icon.addPixmap(QPixmap(imgRes));
    btn->setIcon(icon);
    btn->setIconSize(size);
    if (text != "")
        btn->setText(text);
    if (styleSheet != "")
        btn->setStyleSheet("QPushButton " + styleSheet);
}

void MiscComponents::setToolButton(QToolButton *btn, QString imgRes, QString text, Qt::ToolButtonStyle toolButtonStyle, QString styleSheet)
{
    QPixmap pix(imgRes);
    btn->setText(text);
    btn->setIcon(pix);
    btn->setIconSize(pix.size());
    btn->setToolButtonStyle(toolButtonStyle);
    btn->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    if (styleSheet != "")
        btn->setStyleSheet("QToolButton " + styleSheet);
}

void MiscComponents::setToolButton(QToolButton* btn, QString imgRes, QString text, QSize size, Qt::ToolButtonStyle toolButtonStyle, QString styleSheet)
{
    QPixmap pix(imgRes);
    btn->setText(text);
    btn->setIcon(pix);
    btn->setIconSize(size);
    btn->setToolButtonStyle(toolButtonStyle);
    btn->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    if (styleSheet != "")
        btn->setStyleSheet("QToolButton " + styleSheet);
}

void MiscComponents::setTextToHeader(QLabel *header, QString text)
{
    Config* cfg = Config::Instance();
    QLabel* headWidget = header;
    if (text == "") {
        headWidget->setPixmap(QPixmap(cfg->Res_image_banner));
        headWidget->setScaledContents(true);
        return;
    }
    QPixmap pixmap(cfg->Res_image_banner);
    QFont font;
    QPainter painter;
    font.setPixelSize(28);
    painter.begin(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(font);
//    painter.drawText(800,15,1035,50, Qt::AlignVCenter, text);
    painter.drawText(950,10,1185,50, Qt::AlignBottom, text);
    painter.end();
    headWidget->setPixmap(pixmap);
    headWidget->setScaledContents(true);
}

QWidget* MiscComponents::getHeader(QWidget *parent)
{
    QLabel* headWidget = new QLabel(parent);
    MiscComponents::setTextToHeader(headWidget);
    return headWidget;
}

QWidget* MiscComponents::gettitle(QColor backgroundColor, QString imgRes, QString text, QWidget *parent)
{
    return gettitle(backgroundColor, imgRes, text, NULL, parent);
}

QWidget* MiscComponents::gettitle(QColor backgroundColor, QString imgRes, QString text, QWidget* backBtn, QWidget *parent)
{
    QWidget* headWidget = new QWidget(parent);
    QPalette palette;
    palette.setBrush(headWidget->backgroundRole(), QBrush(backgroundColor));
    headWidget->setAutoFillBackground(true);
    headWidget->setPalette(palette);
    QHBoxLayout* rootBox = new QHBoxLayout;
    rootBox->setMargin(0);
    rootBox->setSpacing(0);
    headWidget->setLayout(rootBox);

    if (backBtn != NULL)
        rootBox->addWidget(backBtn);

    rootBox->addStretch();

    QLabel* logo = new QLabel(headWidget);
    if (imgRes == "")
        logo->setPixmap(Config::Instance()->Res_icon_empty);
    else
        logo->setPixmap(QPixmap(imgRes));
    rootBox->addWidget(logo);
    QLabel* label = new QLabel();
    label->setText(text);
    label->setStyleSheet("QLabel {font: 75 12pt \"黑体\"; color : blue;}");
    rootBox->addWidget(label);
    rootBox->addStretch();
    return headWidget;
}

QWidget* MiscComponents::getSectionLabel(QString text, QWidget *parent)
{
    Config* cfg = Config::Instance();
    QWidget* widget = new QWidget(parent);
    widget->setStyleSheet("QWidget { background-color : rgb(103,103,103); }");
    QHBoxLayout* hbox = new QHBoxLayout;
    widget->setLayout(hbox);
    QLabel* label = new QLabel(widget);
    label->setPixmap(QPixmap(cfg->Res_icon_empty));
    hbox->addWidget(label, 1);
    label = new QLabel(text, widget);
    label->setObjectName("sectionLabel");
    label->setAlignment(Qt::AlignCenter);
    hbox->addWidget(label, 100);
    hbox->setContentsMargins(15, 5, 15, 5);
    return widget;
}

QWidget* MiscComponents::getSectionLabel(QString text, QString imgRes, QWidget *parent)
{
    QWidget* widget = new QWidget(parent);
    widget->setStyleSheet("QWidget { background-color : rgb(103,103,103); }");
    QHBoxLayout* hbox = new QHBoxLayout;
    widget->setLayout(hbox);
    QLabel* label = new QLabel(widget);
    label->setPixmap(QPixmap(imgRes));
    hbox->addWidget(label, 1);
    label = new QLabel(text, widget);
    label->setStyleSheet("QLabel { font: 75 12pt \"黑体\"; color : white; }");
    label->setAlignment(Qt::AlignCenter);
    hbox->addWidget(label, 100);
    hbox->setContentsMargins(15, 5, 15, 5);
    return widget;
}

QGroupBox* MiscComponents::getGroupBox(QBoxLayout *layout, QWidget *parent)
{
    layout->setMargin(2);
    QGroupBox* groupbox = new QGroupBox(parent);
    groupbox->setStyleSheet(Config::Instance()->groupboxStylesheet);
    groupbox->setLayout(layout);
    groupbox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    return groupbox;
}

UserData::~UserData()
{
    LOG(Logger::Debug, "UserData deleting\n");
}
