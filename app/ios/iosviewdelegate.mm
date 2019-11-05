#include <QtCore>
#import "iosviewdelegate.h"

@interface QIViewDelegate ()

@end

@implementation QIViewDelegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if (alertViewClickedButtonAtIndex)
        alertViewClickedButtonAtIndex(buttonIndex);
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex{
    if (alertViewDismissWithButtonIndex)
        alertViewDismissWithButtonIndex(buttonIndex);
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {

    if (actionSheetClickedButtonAtIndex) {
        actionSheetClickedButtonAtIndex(buttonIndex);
    }
}

- (void)actionSheet:(UIActionSheet *)actionSheet didDismissWithButtonIndex:(NSInteger)buttonIndex {
    if (actionSheetDidDismissWithButtonIndex) {
        actionSheetDidDismissWithButtonIndex(buttonIndex);
    };
}

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
