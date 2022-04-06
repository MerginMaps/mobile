#!/usr/bin/env python3

import sys
import plistlib
import xml.etree.ElementTree as ET

if len(sys.argv) != 3:
    print("Wrong args, correct run: check_ios_translation INFOPLIST_PATH INPUTI18N_QRC_PATH")
    exit(1)

infoplistpath = sys.argv[1]
input18npath = sys.argv[2]

# parse info plist
ioslangs = []

with open(infoplistpath, "rb") as f:
    plistdata = plistlib.load(f)
    ioslangs = plistdata['CFBundleLocalizations']

# parse input_i18n.qrc (xml)
i18nlangs = []
rcc = ET.parse(input18npath).getroot()

for lang in rcc.findall('qresource/file'):
    i18nlangs.append(lang.text)

# convert i18n langs to ios format, e.g. 'input_es.qm' -> 'es'
i18nlangs = list(
    map(
        lambda l: l.split('.qm')[0],
        map(
            lambda la: la.split('input_')[1],
            i18nlangs
        )
    )
)

# find missing langs
missinglangs = [lang for lang in i18nlangs if lang not in ioslangs]

if len(missinglangs) == 0:
    print("Success, all languages are in info.plist")
    exit(0)
else:
    print("Failure, Info.plist is missing languages:", missinglangs)
    exit(1)
