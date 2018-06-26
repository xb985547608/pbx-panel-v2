#ifndef SECTIONZONE_H
#define SECTIONZONE_H

#ifndef V4
#include <QtWidgets>
#else
#include <QtGui>
#endif

/*
生成一个矩形的section

----------------
-------titile-------
+---------------+
+               +
+     content     +
+     holder      +
+---------------+
+-btnOk---btnCancel-+

*/
class SectionZone : public QWidget
{
    Q_OBJECT
public:
    explicit SectionZone(
            QString title,
            QWidget* content,
            QPushButton* btnOk,
            QPushButton* btnCancel = 0,
            QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // SECTIONZONE_H
