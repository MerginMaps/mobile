/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINSUBSCRIPTIONSTATUS_H
#define MERGINSUBSCRIPTIONSTATUS_H

#include <QObject>

class MerginSubscriptionStatus
{
    Q_GADGET
  public:
    explicit MerginSubscriptionStatus();

    enum SubscriptionStatus
    {
      FreeSubscription,
      ValidSubscription,
      SubscriptionInGracePeriod,
      SubscriptionUnsubscribed,
      CanceledSubscription
    };
    Q_ENUMS( SubscriptionStatus )

    static QString toString( const SubscriptionStatus &type );
};

#endif // MERGINSUBSCRIPTIONSTATUS_H
