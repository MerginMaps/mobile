# How-To

This documents is storage for various how-tos for development and distribution team

## Android assets

### application icon
 - all app icon variants are stored in app/android/res subfolders

 - use Android Studio to generate all app icon variants from svg (see https://developer.android.com/studio/write/create-app-icons#create-adaptive)
   - command `find . -name 'ic_appicon*' -exec sh -c 'cp {} "<path to app/android/res>/$(dirname {})/"' \;` might be handy - it copies all generated icons in subfolders to correct subfolders in the destination
   - note: you need to have some project opened in Android Studio, you can open the generated android project from QtCreator

 - last step is to change `android:icon=@mipmap/<generated_icon_name>` in AndroidManifest

### launch screen
 - we are using SplashScreen API (introduced in Android 12), see https://developer.android.com/develop/ui/views/launch/splash-screen 
 - open splasscreentheme.xml and simply edit parameters there to adjust the splashscreen

## iOS assets 

### application icon

NOTE: icon must be without transparency

 - replace icon in `ios/Images.xcassets/AppIcon.appiconset/appicon.png` with 1024x1024px variant
 - you can also do it via XCode - in file browser navigate to `Input -> Resources -> Images` and choose `AppIcon`. Make sure that it says `Single Size` for iOS in the right panel. This way you need only one 1024x1024 image for all icon variants.

### launch screen

 - defined in `app/ios/launchscreen/MMLaunchScreen.storyboard` - open in XCode and edit.
 - if you need to add a new one, open the configured project in XCode and add a new storyboard following: https://appbus.wordpress.com/2020/04/15/qt-ios-splash-storyboard/#:~:text=Create%20a%20new%20Storyboard%20Splash%20Screen

## iOS dist certificates

1. Create new iOS distribution certificate
- Keychain Access is either in /Application/Utilities or in folder "/System/Library/CoreServices/Applications/" (macos 15.3+)
- open Keychain Access -> Certificate Assistant -> Request certificate (see https://help.apple.com/developer-account/#/devbfa00fef7)
   - User email: peter.petrik@lutraconsulting.co.uk
   - Common name: LUTRA CONSULTING LIMITED
   - CA email: None
      - Request is: Saved to disk
  -> creates `CertificateSigningRequest.certSigningRequest` file
- click + on https://developer.apple.com/account/resources/certificates/list and create new iOS Distribution Certificate
- Download it (`ios_distribution.cer`) and double click to open in Keychain Access
- In Keychain Access, right click and export p12 file (`iPhone Distribution: LUTRA CONSULTING LIMITED (xxxxxxxxx)`), with passport (IOS_CERT_KEY). You need to have it imported to "login" or personal space to be able to export p12 file.

2. Create/Update provisioning profile
- Go to https://developer.apple.com/account/resources/profiles
- Edit LutraConsultingLtd.Input.AppStore and assign the certificate generated in 1.
- Download the `LutraConsultingLtdInputAppStore.mobileprovision`

3. Encrypt the files for GitHub
- Create IOS_GPG_KEY
- install gpg `brew install gnupg`
- Encrypt p12 file with command (use space in front of command to not end up in history!) ` gpg --symmetric --batch --passphrase="<IOS_GPG_KEY>" --output ./Certificates_ios_dist.p12.gpg ./Certificates.p12`
- Encrypt mobileprovision file with command ` gpg --symmetric --batch --passphrase="<IOS_GPG_KEY>" --output ./LutraConsultingLtdInputAppStore.mobileprovision.gpg ./LutraConsultingLtdInputAppStore.mobileprovision`
- Copy both files to `.github/secrets/ios`
- Update secret (passports) on github

## QGIS debugging
When you want to debug QGIS code it's necessary to first build the QGIS dependency in `RelWithDebugInfo` mode. This can be achieved by specifying these environment variables:

|         Name          |       Value        |
|:---------------------:|:------------------:|
|  `QGIS_DEBUG_BUILD`   |         1          |
| `VCPKG_KEEP_ENV_VARS` | `QGIS_DEBUG_BUILD` |

Furthermore, to debug android version of Mergin Maps it's highly recommended to use QtCreator, even though it should be possible also without it.