#include <QtCore>
#import "iosviewdelegate.h"

@interface IOSViewDelegate ()

@end

@implementation IOSViewDelegate


- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
    if (imagePickerControllerDidFinishPickingMediaWithInfo) {
        imagePickerControllerDidFinishPickingMediaWithInfo(picker,info);
    }
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {
    if (imagePickerControllerDidCancel) {
        imagePickerControllerDidCancel(picker);
    }
}

@end
