# Translations

https://docs.transifex.com/transifex-github-integrations/github-tx-ui

1. update ts files with `scripts/update_i18n_ts.bash`
   - it needs Qt5 installation directory as an argument, for example it is `/usr/share/qt5` on Ubuntu with system packages, or `/opt/Qt/5.14.2/clang_64` on mac
2. auto-sync and translate on transifex (i18n/*.ts files)
3. get back the changes (manual sync after translation)
4. run `scripts/release_i18n_qm.bash`
5. test app

# How to add language

1. add language on transifex
2. translate and download/merge (file input_<lang>.tx)
3. run `scripts/release_i18n_qm.bash`
4. add language to app/ios/Info.plist (CFBundleLocalizations)
