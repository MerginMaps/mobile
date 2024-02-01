#ifndef POSITIONPROVIDER_H
#define POSITIONPROVIDER_H

#include <QObject>
#include <QString>

class  PositionProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString name READ name CONSTANT )
    Q_PROPERTY( QString type READ type CONSTANT )
    Q_PROPERTY( QString message READ message CONSTANT )

public:
    PositionProvider() : pName("Gps Source is ok!"),  pType("external"), pStateMessage("Message is alright!") {}

    QString name() const {return pName;}
    QString type() const {return pType;}
    QString message() const {return pStateMessage;}

private:
    QString pName;
    QString pType;
    QString pStateMessage;

};

#endif // POSITIONPROVIDER_H
