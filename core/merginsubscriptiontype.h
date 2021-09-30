/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINSUBSCRIPTIONTYPE_H
#define MERGINSUBSCRIPTIONTYPE_H

#include <QObject>
#include <QString>

class MerginSubscriptionType
{
    Q_GADGET
  public:
    explicit MerginSubscriptionType();

    enum SubscriptionType
    {
      NoneSubscriptionType,
      AppleSubscriptionType, // in-app purchases (apple provider)
      StripeSubscriptionType, // stripe provider
      TestSubscriptionType, // testing (mock) provider
    };
    Q_ENUMS( SubscriptionType )

    static SubscriptionType fromString( const QString &name );
    static QString toString( const SubscriptionType &type );
};

#endif // MERGINSUBSCRIPTIONTYPE_H
