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

QVector<int> IosUtils::getSafeAreaImpl()
{
  QVector<int> ret;

  if ( @available( iOS 11.0, * ) )
  {
    UIWindow *window = UIApplication.sharedApplication.windows.firstObject;

    int top = window.safeAreaInsets.top;
    int right = window.safeAreaInsets.right;
    int bottom = window.safeAreaInsets.bottom;
    int left = window.safeAreaInsets.left;

    ret << top << right << bottom << left;
    return ret;
  }

  return ret;
}
