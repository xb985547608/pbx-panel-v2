#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "dlgemergencycall.h"
#include <QMessageBox>



DlgEmergencyCall::DlgEmergencyCall(QWidget *parent) :
    QDialog(parent)
{
    //v_spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    setMinimumSize(400, 500);
    m_layout = new QVBoxLayout;
    m_btnLayout = new QVBoxLayout;
    m_layout->addLayout(m_btnLayout);
    m_layout->addStretch();
    this->setLayout(m_layout);
}

void DlgEmergencyCall::AddEmergencyCallInfo(EmergencyCallInfo info)
{
    m_verInfo.push_back(info);
    PushButton();
}


void DlgEmergencyCall::PushButton()
{
    if(m_verInfo.size() <= 0){
        this->hide();
        return ;
    }
    QObjectList objList =  children();

    for(int i = 0; i < m_verInfo.size(); i++){
        bool isAdd = true;

        for(int child = 0; child < objList.length(); child++){
            QObject* o = objList.at(child);
            if (o->inherits("QPushButton")) {
                QPushButton* b = qobject_cast<QPushButton*>(o);
                if(b->objectName() == m_verInfo[i].m_strUuid){
                    //TODO:tuichubencixunhuan
                    isAdd = false;
                    break;
                }
            }
        }
        if(isAdd){
            QPushButton *btn = new QPushButton();
            btn->setFont(QFont("幼圆",12));
            btn->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.392, y1:0.665, x2:1, y2:0, stop:0 rgba(220, 127, 127, 255), stop:1 rgba(255, 255, 255, 255));");
            QString strChannel = m_verInfo[i].m_StrChannel;
            QString strName;
            if (strChannel.startsWith("SIP/")){
                strName = strChannel.mid(strChannel.indexOf("SIP/"),strChannel.indexOf("-") - strChannel.indexOf("SIP/"));
            }else if(strChannel.startsWith("Local/")){
                strName = strChannel.mid(strChannel.indexOf("Local/"),strChannel.indexOf("@") - strChannel.indexOf("Local/"));
            }
            QString btnText = strName + "  Wait  " + m_verInfo[i].m_strTime;
            btn->setText(btnText);

            btn->setObjectName(m_verInfo[i].m_strUuid);
            m_btnLayout->addWidget(btn);
            connect(btn,SIGNAL(clicked()),this,SLOT(OnClick()));
        }

    }

    this->show();


}

void DlgEmergencyCall::OnClick()
{
    QPushButton* btn= qobject_cast<QPushButton*>(sender());
    DelButton(btn->objectName());
    delete btn;

}

void DlgEmergencyCall::DelButton(QString strUuid)
{
    for(int i = 0; i < m_verInfo.size(); i++){
        if(m_verInfo[i].m_strUuid == strUuid){
            m_verInfo.remove(i);
            break;
        }
    }
    if(m_verInfo.size() <= 0){
        this->hide();
        return ;
    }
    //PushButton();
}
void DlgEmergencyCall::closeEvent(QCloseEvent *e)
{
    if(m_verInfo.size()>0){
        QMessageBox::about(this, tr("提示"), tr("没有处理紧急呼叫，马上关闭！"));
    }else{
        this->hide();
    }
    return;
}
