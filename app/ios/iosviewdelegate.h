#ifndef IOSVIEWDELEGATE_H
#define IOSVIEWDELEGATE_H

#include <UIKit/UIKit.h>

@interface IOSViewDelegate : NSObject<UIImagePickerControllerDelegate,
                                     UINavigationControllerDelegate> {

    @public

    void (^ imagePickerControllerDidFinishPickingMediaWithInfo)(UIImagePickerController* picker,NSDictionary* info);
    void (^ imagePickerControllerDidCancel)(UIImagePickerController* picker);
}
@end

#endif // IOSVIEWDELEGATE_H
