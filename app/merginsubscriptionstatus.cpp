/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginsubscriptionstatus.h"

MerginSubscriptionStatus::MerginSubscriptionStatus()
{
}

QString MerginSubscriptionStatus::toString( const MerginSubscriptionStatus::SubscriptionStatus &type )
{
  if ( type == MerginSubscriptionStatus::CanceledSubscription )
    return "canceled";

  if ( type == MerginSubscriptionStatus::FreeSubscription )
    return "free";

  if ( type == MerginSubscriptionStatus::SubscriptionInGracePeriod )
    return "grace";

  if ( type == MerginSubscriptionStatus::SubscriptionUnsubscribed )
    return "unsubscribed";

  if ( type == MerginSubscriptionStatus::ValidSubscription )
    return "valid";

  return "unknown";
}
