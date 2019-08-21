# development certificate

- device UDID: either iTunes or about this mac->system report->USB->find iPAD (Serial Number)
https://deciphertools.com/blog/2014_11_19_how_to_find_your_iphone_udid/
- register app
- create dev ios certificate
- generate profile
- install all on device
- set in Qt Creator

# application icon

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
