/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginsubscriptiontype.h"

MerginSubscriptionType::MerginSubscriptionType()
{
}

MerginSubscriptionType::SubscriptionType MerginSubscriptionType::fromString( const QString &name )
{
  if ( name == "apple" )
    return MerginSubscriptionType::AppleSubscriptionType;

  if ( name == "stripe" )
    return MerginSubscriptionType::StripeSubscriptionType;

  if ( name == "test" )
    return MerginSubscriptionType::TestSubscriptionType;

  return MerginSubscriptionType::NoneSubscriptionType;
}

QString MerginSubscriptionType::toString( const MerginSubscriptionType::SubscriptionType &type )
{
  if ( type == MerginSubscriptionType::AppleSubscriptionType )
    return "apple";

  if ( type == MerginSubscriptionType::StripeSubscriptionType )
    return "stripe";

  if ( type == MerginSubscriptionType::TestSubscriptionType )
    return "test";

  return "";
}
