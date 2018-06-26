#ifndef UIEXTENSION_H
#define UIEXTENSION_H

#include "dingobutton.h"
#include "pbx/pbx.h"
#include <QLabel>

class UiExtensionBase : public DingoButton
{
    Q_OBJECT
public:
    explicit UiExtensionBase(PBX::Extension e, QWidget* parent = 0);
    PBX::Extension getPbxExten() { return mPbxExten; }

public slots:

public:
    void setStateIconRes(PBX::PeerState state, QString iconRes);
protected:
    QMap<int, QString> mStateIconsRes;
    PBX::Extension mPbxExten;
    QLabel* mIcon; //主头像
    PBX::PeerInfo getPeerWithLongestDuration(); //从peers中找出处于通话状态最长时间的peer
    int getLongestDuration(); //从peers中找到通话时长最长的peer，并返回时长，如果没有，就返回CALL_NOT_CONNECTED
    static int CALL_NOT_CONNECTED;
};

#endif // UIEXTENSION_H
