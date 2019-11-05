//
// QIViewDelegate - A universal delegate class for listening event
//

#include <UIKit/UIKit.h>

@interface QIViewDelegate : NSObject<UIAlertViewDelegate,
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
