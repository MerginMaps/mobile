#ifndef IOSINTERFACE_H
#define IOSINTERFACE_H

#include <UIKit/UIKit.h>

#include <QVariantMap>


/**
 * The interface of objective-c methods.
*/
@interface IOSInterface : NSObject
-( void )showImagePicker;
@end

#endif // IOSINTERFACE_H
