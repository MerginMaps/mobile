#ifndef IOSVIEWDELEGATE_H
#define IOSVIEWDELEGATE_H

#include <UIKit/UIKit.h>

@interface IOSViewDelegate : NSObject<UIAlertViewDelegate,
                                     UIActionSheetDelegate,
                                     UIImagePickerControllerDelegate,
                                     UINavigationControllerDelegate> {

    @public

    void ( ^ alertViewClickedButtonAtIndex )( int );
    void ( ^ alertViewDismissWithButtonIndex )( int );

    void ( ^ actionSheetClickedButtonAtIndex) (int);
    void ( ^ actionSheetDidDismissWithButtonIndex) (int);

    void (^ imagePickerControllerDidFinishPickingMediaWithInfo)(UIImagePickerController* picker,NSDictionary* info);
    void (^ imagePickerControllerDidCancel)(UIImagePickerController* picker);
}
@end

#endif // IOSVIEWDELEGATE_H
