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

see: https://appbus.wordpress.com/2017/10/06/ios-11-and-xcode-9-in-qt-5-9-x-projects/

NOTE: icon must be without transparency!

```
brew install ImageMagick
git clone https://github.com/smallmuou/ios-icon-generator
chmod 777 ios-icon-generator.sh
cd <repo>/input/app/ios
<path_to_generator>/ios-icon-generator.sh ic_input_no_transparency.png appicon/
```

### launch screen
- iOS7: use http://ticons.fokkezb.nl/ and rename
- iPhone6: (XIB) generate in XCode (https://medium.com/better-programming/swift-3-creating-a-custom-view-from-a-xib-ecdfe5b3a960)

## iOS dist certificates

1. Create new iOS distribution certificate
- open Keychain Access -> Certificate Assistant -> Request certificate (see https://help.apple.com/developer-account/#/devbfa00fef7)
   - User email: peter.petrik@lutraconsulting.co.uk
   - Common name: LUTRA CONSULTING LIMITED
   - CA email: None
      - Request is: Saved to disk
  -> creates `CertificateSigningRequest.certSigningRequest` file
- click + on https://developer.apple.com/account/resources/certificates/list and create new iOS Distribution Certificate
- Download it (`ios_distribution.cer`) and double click to open in Keychain Access
- In Keychain Access, right click and export p12 file (`iPhone Distribution: LUTRA CONSULTING LIMITED (xxxxxxxxx)`), with passport (IOS_CERT_KEY). You need to have it imported to "login" or personal space to be able to export p12 file.
- Store request and cer file, p12 in passbolt.lutraconsulting.co.uk (files in google drive in PP's MerginMaps/dev folder)

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

## iOS in-app purchases

### add new subscription
- go to: https://appstoreconnect.apple.com > In-App Purchases Manage
- click +
    - Auto-renewable subscription
    - Reference name: mergin_tier_<x> (where <x> is 1,2,.. representing are 1GB, 10GB... tiers)
    - Product ID: apple_mergin_tier_<x> (where <x> is same as reference name)
    - Subscription Group: mergin_1
    - Add and fill subscription duration and prize
- create the SAME subscription plan in Mergin Maps via Admin interface

### to do apple in-app purchases test 

without actually paying
https://itunesconnect.apple.com

- create InputApp test user in https://itunesconnect.apple.com: Users and Access > Sandbox Testers > New Tester (create your user)
    - You may want to test different location (for QLocale)
    - Create unique email (group/alias) for the new user
- create the Mergin Maps Account for the user on app.dev.merginmaps.com
- you need to logout your regular MacOs/Ios Apple User from device before trying to purchase something
- login as test user and you can simulate purchasing
- note that this works only for app.dev.merginmaps.com. It will not be possible to use test user on public/production server.