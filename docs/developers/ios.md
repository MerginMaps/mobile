looking for ios [publishing](./publishing.md)

# in-app purchases

read https://doc.qt.io/qt-5/qtpurchasing-appstore.html

## add new subscription
- go to: https://appstoreconnect.apple.com > In-App Purchases Manage
- click +
    - Auto-renewable subscription
    - Reference name: mergin_tier_<x> (where <x> is 1,2,.. representing are 1GB, 10GB... tiers)
    - Product ID: apple_mergin_tier_<x> (where <x> is same as reference name)
    - Subscription Group: mergin_1
    - Add and fill subscription duration and prize
- create the SAME subscription plan in Mergin via Admin interface

## to do apple in-app purchases test (without actually paying)
https://itunesconnect.apple.com

- create InputApp test user in https://itunesconnect.apple.com: Users and Access > Sandbox Testers > New Tester (create your user)
    - You may want to test different location (for QLocale)
    - Create unique email (group/alias) for the new user 
- create the Mergin Account for the user on test.dev.cloudmergin.com and/or dev.dev.cloudmergin.com
- you need to logout your regular MacOs/Ios Apple User from device before trying to purchase something
- login as test user and you can simulate purchasing
- note that this works only for test.dev.cloudmergin.com and dev.dev.cloudmergin.com. It will not be possible to use test user on public/production server.

# development certificate

- device UDID: either iTunes or about this mac->system report->USB->find iPAD (Serial Number)
https://deciphertools.com/blog/2014_11_19_how_to_find_your_iphone_udid/
- register app
- create dev ios certificate
- generate profile
- install all on device
- set in Qt Creator

# production certificate

1. Create new iOS distribution certificate
- open Keychain Access -> Certificate Assistant -> Request certificate (see https://help.apple.com/developer-account/#/devbfa00fef7)
   - User email: peter.petrik@lutraconsulting.co.uk
   - Common name: LUTRA CONSULTING LIMITED
   - CA email: None 
      - Request is: Saved to disk
  -> creates `CertificateSigningRequest.certSigningRequest` file
- click + on https://developer.apple.com/account/resources/certificates/list and create new iOS Distribution Certificate
- Download it (`ios_distribution.cer`) and double click to open in Keychain Access 
- In Keychain Access, right click and export p12 file (`iPhone Distribution: LUTRA CONSULTING LIMITED (xxxxxxxxx)`), with passport (IOS_CERT_KEY)
- Store request and cer file, p12 in passbolt.lutraconsulting.co.uk

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


# application icon

see: https://appbus.wordpress.com/2017/10/06/ios-11-and-xcode-9-in-qt-5-9-x-projects/

NOTE: icon must be without transparency!

```
brew install ImageMagick
git clone https://github.com/smallmuou/ios-icon-generator
chmod 777 ios-icon-generator.sh
cd <repo>/input/app/ios
<path_to_generator>/ios-icon-generator.sh ic_input_no_transparency.png appicon/
```

# launch screen
- iOS7: use http://ticons.fokkezb.nl/ and rename
- iPhone6: (XIB) generate in XCode (https://medium.com/better-programming/swift-3-creating-a-custom-view-from-a-xib-ecdfe5b3a960)
