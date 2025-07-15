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
#include <sys/utsname.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <QString>
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

QString IosUtils::getManufacturerImpl()
{
  return "APPLE INC.";
}

QString IosUtils::getDeviceModelImpl()
{
  struct utsname systemInfo;
  uname( &systemInfo );
  QString deviceModel = QString::fromUtf8( systemInfo.machine );
  return deviceModel.toUpper();
}

@interface FileOpener : UIViewController <UIDocumentInteractionControllerDelegate>
@end

@implementation FileOpener

- ( UIViewController * )documentInteractionControllerViewControllerForPreview:( UIDocumentInteractionController * )ctrl
{
  return self;
}

@end

bool IosUtils::openFileImpl( const QString &filePath )
{
  static FileOpener *viewer = nil;
  NSURL *resourceURL = [NSURL fileURLWithPath:filePath.toNSString()];

  UIDocumentInteractionController *interactionCtrl = [UIDocumentInteractionController interactionControllerWithURL:resourceURL];
  UIViewController *rootViewController = [[[[UIApplication sharedApplication] windows] firstObject] rootViewController];

  viewer = [[FileOpener alloc] init];
  [rootViewController addChildViewController: viewer];
  interactionCtrl.delegate = ( id<UIDocumentInteractionControllerDelegate> )viewer;

  if ( ![interactionCtrl presentPreviewAnimated:NO] )
  {
    if ( ![interactionCtrl presentOptionsMenuFromRect:CGRectZero inView:viewer.view animated:NO] )
    {
      return false;
    }
  }

  return true;
}

void IosUtils::vibrate()
{
  // initialize engine
  @property( nonatomic, strong ) CHHapticEngine *engine;
  NSError *errorEngine;
  _engine = [[CHHapticEngine alloc] initAndReturnError:&errorEngine];

  // restart engine handler
  __weak ViewController *weakViewController = self;
  [_engine setResetHandler:^
  {
    // Try restarting the engine again.
    NSError * startupError;
    [weakViewController.engine startAndReturnError:&startupError];

    if ( startupError )
    {
      NSLog( @"ERROR: Engine couldn't restart!" );
    }

    // Recreate haptic pattern player
    weakViewController.player = [weakViewController.engine createPlayerWithPattern:pattern error:&errorPlayer];
  }];


  // vibration pattern (single tap)
  NSDictionary *hapticDict = @
  {
  CHHapticPatternKeyPattern: @[
    @{
    CHHapticPatternKeyEvent: @{
      CHHapticPatternKeyEventType: CHHapticEventTypeHapticTransient,
      CHHapticPatternKeyTime: @( CHHapticTimeImmediate ),
      CHHapticPatternKeyEventDuration: @1.0
      },
    },
    ],
  };

  // initialize patter from dictionary
  NSError *errorPattern;
  CHHapticPattern *pattern = [[CHHapticPattern alloc] initWithDictionary:hapticDict error:&errorPattern];

  // create player from engine
  NSError *errorPlayer = nil;
  id<CHHapticPatternPlayer> player = [_engine createPlayerWithPattern:pattern error:&errorPlayer];

  // Stop the engine after it completes the playback.
  [_engine notifyWhenPlayersFinished:^CHHapticEngineFinishedAction( NSError * _Nullable error )
  {
    return CHHapticEngineFinishedActionStopEngine;
  }];

  [_engine startWithCompletionHandler: ^ ( NSError * returnedError )
  {
    NSError *error;
    [self.player startAtTime:0 error:&error];
  }];

}