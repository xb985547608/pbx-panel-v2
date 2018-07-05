#include "PhoneBook.h"

QString PhoneBook::toXmlString()
{
    QString xmlString = QString(
                "<PhoneBook>"
                    "<company>%1</company>"
                    "<id>%2</id>"
                    "<mobile>%3</mobile>"
                    "<officephone>%4</officephone>"
                    "<position>%5</position>"
                "</PhoneBook>")
            .arg(company)
            .arg(id)
            .arg(mobile)
            .arg(homeNumber)
            .arg(position);
    return xmlString;
}
