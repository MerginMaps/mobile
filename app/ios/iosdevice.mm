#import <UIKit/UIKit.h>
#include "iosdevice.h"

QVariantMap QIDevice::fetch() const {
    UIDevice* device = [UIDevice currentDevice];

    QVariantMap result;

    result["identifierForVendor"] = QString::fromNSString([[device identifierForVendor] UUIDString]);

    return result;
}


