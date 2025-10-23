/***************************************************************************
  iosviewdelegate.mm
  --------------------------------------
  Date                 : Nov 2019
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

//#include <QtCore>
#import "iosviewdelegate.h"

//@interface IOSViewDelegate()
//
//@end

@implementation IOSViewDelegate

-( id ) initWithHandler:( IOSImagePicker * )handler
{
  self = [super init];
  if ( self )
  {
    self->processingPicture = NO;
    self->handler = handler;
  }
  return self;
}

- ( void )imagePickerController:( UIImagePickerController * )picker didFinishPickingMediaWithInfo:( NSDictionary * )info
{
  if ( imagePickerControllerDidFinishPickingMediaWithInfo )
  {
    imagePickerControllerDidFinishPickingMediaWithInfo( picker, info );
  }
}

- ( void )imagePickerControllerDidCancel:( UIImagePickerController * )picker
{
  if ( imagePickerControllerDidCancel )
  {
    imagePickerControllerDidCancel( picker );
  }
}

@end

@implementation IOSGalleryViewController

- (void)openPHPicker {
    PHPickerConfiguration *config = [[PHPickerConfiguration alloc] init];
    config.filter = [PHPickerFilter imagesFilter];
    config.selectionLimit = 1;

    PHPickerViewController *picker = [[PHPickerViewController alloc] initWithConfiguration:config];
    picker.delegate = self;
    [self presentViewController:picker animated:YES completion:nil];
}

- (void)picker:(PHPickerViewController *)picker didFinishPicking:(NSArray<PHPickerResult *> *)results {
    [picker dismissViewControllerAnimated:YES completion:nil];

    if (results.count == 0) return;

    PHPickerResult *result = results.firstObject;
    NSString *assetId = result.assetIdentifier;

    if (assetId) {
        PHFetchResult<PHAsset *> *fetchResult =
            [PHAsset fetchAssetsWithLocalIdentifiers:@[assetId] options:nil];

        PHAsset *asset = fetchResult.firstObject;
        if (asset) {
//            [self copyAssetToProject:asset];
            NSLog(@"Should process Asset");
        }
    } else {
        NSLog(@"No asset identifier available");
    }
}

- (void)copyAssetToProject:(PHAsset *)asset {
    PHImageRequestOptions *options = [[PHImageRequestOptions alloc] init];
    options.networkAccessAllowed = YES;
    options.deliveryMode = PHImageRequestOptionsDeliveryModeHighQualityFormat;
    options.version = PHImageRequestOptionsVersionOriginal;

    [[PHImageManager defaultManager]
     requestImageDataAndOrientationForAsset:asset
     options:options
     resultHandler:^(NSData * _Nullable imageData,
                     NSString * _Nullable dataUTI,
                     CGImagePropertyOrientation orientation,
                     NSDictionary * _Nullable info) {
        if (imageData) {
                    NSString *imagePathFixed = [NSString stringWithFormat:@"%@/%@", @"file://", imagePath];
                    NSURL *outputURL = [NSURL URLWithString:[imagePathFixed stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]]];

                    NSError *error = nil;
                    BOOL success = [imageData writeToUrl:outputURL options:NSDataWritingWithoutOverwriting error:&error];
                    if (success) {
                        NSLog(@"Image saved successfully at path:\n%@", outputURL);
                    } else {
                        NSLog(@"Failed to save image: %@", error);
                    }
                } else {
                    NSLog(@"No image data found, info: %@", info);
                }
    }];
}

@end
