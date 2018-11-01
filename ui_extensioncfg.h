/********************************************************************************
** Form generated from reading UI file 'extensioncfg.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTENSIONCFG_H
#define UI_EXTENSIONCFG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExtensionCfgWidget
{
public:
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QLineEdit *extenNum;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *url1;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QLineEdit *url2;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout;
    QPushButton *save;
    QPushButton *remove;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ExtensionCfgWidget)
    {
        if (ExtensionCfgWidget->objectName().isEmpty())
            ExtensionCfgWidget->setObjectName(QStringLiteral("ExtensionCfgWidget"));
        ExtensionCfgWidget->resize(383, 108);
        ExtensionCfgWidget->setMinimumSize(QSize(0, 108));
        ExtensionCfgWidget->setMaximumSize(QSize(16777215, 108));
        horizontalLayout_4 = new QHBoxLayout(ExtensionCfgWidget);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 2, -1, -1);
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label = new QLabel(ExtensionCfgWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(60, 25));
        label->setMaximumSize(QSize(60, 25));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label);

        extenNum = new QLineEdit(ExtensionCfgWidget);
        extenNum->setObjectName(QStringLiteral("extenNum"));
        extenNum->setMinimumSize(QSize(70, 25));
        extenNum->setMaximumSize(QSize(70, 25));

        horizontalLayout_3->addWidget(extenNum);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(ExtensionCfgWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(60, 25));
        label_2->setMaximumSize(QSize(60, 25));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(14);
        label_2->setFont(font1);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_2);

        url1 = new QLineEdit(ExtensionCfgWidget);
        url1->setObjectName(QStringLiteral("url1"));
        url1->setMinimumSize(QSize(0, 25));
        url1->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_2->addWidget(url1);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_3 = new QLabel(ExtensionCfgWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(60, 25));
        label_3->setMaximumSize(QSize(60, 25));
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_3);

        url2 = new QLineEdit(ExtensionCfgWidget);
        url2->setObjectName(QStringLiteral("url2"));
        url2->setMinimumSize(QSize(0, 25));
        url2->setMaximumSize(QSize(16777215, 25));

        horizontalLayout->addWidget(url2);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        horizontalLayout_4->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        save = new QPushButton(ExtensionCfgWidget);
        save->setObjectName(QStringLiteral("save"));
        save->setMinimumSize(QSize(60, 25));
        save->setMaximumSize(QSize(60, 25));

        verticalLayout->addWidget(save);

        remove = new QPushButton(ExtensionCfgWidget);
        remove->setObjectName(QStringLiteral("remove"));
        remove->setMinimumSize(QSize(60, 25));
        remove->setMaximumSize(QSize(60, 25));

        verticalLayout->addWidget(remove);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_4->addLayout(verticalLayout);


        retranslateUi(ExtensionCfgWidget);

        QMetaObject::connectSlotsByName(ExtensionCfgWidget);
    } // setupUi

    void retranslateUi(QWidget *ExtensionCfgWidget)
    {
        ExtensionCfgWidget->setWindowTitle(QApplication::translate("ExtensionCfgWidget", "Form", Q_NULLPTR));
        label->setText(QApplication::translate("ExtensionCfgWidget", "\345\210\206\346\234\272\345\217\267:", Q_NULLPTR));
        label_2->setText(QApplication::translate("ExtensionCfgWidget", "URL1:", Q_NULLPTR));
        label_3->setText(QApplication::translate("ExtensionCfgWidget", "URL2:", Q_NULLPTR));
        save->setText(QApplication::translate("ExtensionCfgWidget", "\344\277\235\345\255\230", Q_NULLPTR));
        remove->setText(QApplication::translate("ExtensionCfgWidget", "\345\210\240\351\231\244", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ExtensionCfgWidget: public Ui_ExtensionCfgWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTENSIONCFG_H
