#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <QtGui>

class PhoneBook
{
public:
    PhoneBook() {}
    QString toXmlString() const;
    QString id;
    QString company;
    QString department;
    QString position; //职位
    QString name;
    QString extenNumber;
    QString homeNumber;
    QString mobile;
    QString directdid;
};

#endif // PHONEBOOK_H
