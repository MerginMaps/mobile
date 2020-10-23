/***************************************************************************
  iosutils.mm
  --------------------------------------
  Date                 : Oct 2020
  Copyright            : (C) 2019 by Viktor Sklencar
  Email                : viktor.sklencar@lutraconsulting.co.uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <UIKit/UIKit.h>
#include "iosutils.h"

void IosUtils::setIdleTimerDisabled()
{
  [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
}
