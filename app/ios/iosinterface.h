#ifndef IOSINTERFACE_H
#define IOSINTERFACE_H

#include <UIKit/UIKit.h>

#include <QVariantMap>

@interface IOSInterface :NSObject
-(QVariantMap)showImagePicker:(int)num1;
@end

#endif // IOSINTERFACE_H
