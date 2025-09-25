# Mergin Maps mobile app

<picture>
  <source media="(prefers-color-scheme: dark)" width=350 srcset="https://raw.githubusercontent.com/MerginMaps/.github/main/images/MM_logo_HORIZ_COLOR_INVERSE_VECTOR.svg">
  <img width=350 src="https://raw.githubusercontent.com/MerginMaps/.github/main/images/MM_logo_HORIZ_COLOR_VECTOR.svg" alt="Mergin Maps logo">
</picture>

Platform builds:

[![Android Build](https://github.com/MerginMaps/mobile/workflows/Android/badge.svg)](https://github.com/MerginMaps/mobile/actions?query=workflow%3A%22Android%22)
[![iOS Build](https://github.com/MerginMaps/mobile/workflows/iOS%20Build/badge.svg)](https://github.com/MerginMaps/mobile/actions?query=workflow%3A%22iOS+Build%22)
[![win64 Build](https://github.com/MerginMaps/mobile/actions/workflows/win.yml/badge.svg)](https://github.com/MerginMaps/mobile/actions/workflows/win.yml)
[![macOS Build](https://github.com/MerginMaps/mobile/actions/workflows/macos.yml/badge.svg)](https://github.com/MerginMaps/mobile/actions/workflows/macos.yml)
[![linux Build](https://github.com/MerginMaps/mobile/actions/workflows/linux.yml/badge.svg)](https://github.com/MerginMaps/mobile/actions/workflows/linux.yml)

Other Checks:
[![Code Layout](https://github.com/MerginMaps/mobile/workflows/Code%20Layout/badge.svg)](https://github.com/MerginMaps/mobile/actions?query=workflow%3A%22Code+Layout%22)
[![Coverage Status](https://coveralls.io/repos/github/MerginMaps/mobile/badge.svg?branch=master)](https://coveralls.io/github/MerginMaps/mobile?branch=master)
[![Translations](https://github.com/MerginMaps/mobile/actions/workflows/i18n.yml/badge.svg)](https://github.com/MerginMaps/mobile/actions/workflows/i18n.yml)
[![Gallery App](https://github.com/MerginMaps/mobile/actions/workflows/gallery.yml/badge.svg)](https://github.com/MerginMaps/mobile/actions/workflows/gallery.yml)

Citation link:

[![DOI](https://zenodo.org/badge/46554390.svg)](https://zenodo.org/badge/latestdoi/46554390)

Mergin Maps mobile app (formerly known as Input app) makes surveying of geospatial data easy. You can design your survey project in QGIS with custom forms.

## Download
Click below to download the app on your mobile device:

**Current Release**
<p>
<a href='https://play.google.com/store/apps/details?id=uk.co.lutraconsulting&ah=GSqwibzO2n63iMlCjHmMuBk89t4&pcampaignid=MKT-Other-global-all-co-prtnr-py-PartBadge-Mar2515-1&pcampaignid=MKT-Other-global-all-co-prtnr-py-PartBadge-Mar2515-1'><img alt='Get it on Google Play' src='https://raw.githubusercontent.com/MerginMaps/.github/main/images/google-play-store.png' height="57" /></a>
<a href='https://apps.apple.com/us/app/input/id1478603559?ls=1'><img alt='Download it from TestFlight' src='https://raw.githubusercontent.com/MerginMaps/.github/main/images/app-store.png' width="170" /></a>
<a href='https://github.com/MerginMaps/mobile/releases/tag/2025.3.0'><img alt='Available on Windows' src='https://raw.githubusercontent.com/MerginMaps/.github/main/images/app_download_windows.png' height="57" /></a>
</p>

**Beta Release**
<p>
<a href='https://play.google.com/apps/testing/uk.co.lutraconsulting'><img alt='Get it on Google Play' src='https://raw.githubusercontent.com/MerginMaps/.github/main/images/google-play-store.png' height="28.5" /></a>
<a href='https://testflight.apple.com/join/JO5EIywn'><img alt='Download it from TestFlight' src='https://raw.githubusercontent.com/MerginMaps/.github/main/images/app-store.png' width="85" /></a>
<a href='https://github.com/MerginMaps/mobile/actions/workflows/win.yml'><img alt='Available on Windows' src='https://raw.githubusercontent.com/MerginMaps/.github/main/images/app_download_windows.png' height="28.5" /></a>
</p>

For more information on how to use the app, see [Documentation](https://merginmaps.com/docs).

## How to help/contribute?

- Help with translations, join [Mergin Maps mobile app Transifex Team](https://explore.transifex.com/lutra-consulting/mergin-maps-mobile/)
- Write a review of the application on [App Store](https://apps.apple.com/us/app/mergin-maps/id1478603559) or [Google Play](https://play.google.com/store/apps/details?id=uk.co.lutraconsulting&hl=en&gl=US)
- Test the application and [report bugs](https://github.com/MerginMaps/mobile/issues)
- Write a blog post or case study or create a YouTube video. We are happy to help to promote it or co-author and place on our websites
- Software developer? Code and prepare a pull request. We will help you with [setup of the development environment](./INSTALL.md) and answer your questions.
- Donate or [subscribe](https://merginmaps.com/) to the Mergin Maps Cloud service to help us maintain the project.

<div><img align="left" width="45" height="45" src="https://raw.githubusercontent.com/MerginMaps/docs/main/src/public/slack.svg"><a href="https://merginmaps.com/community/join">Join our community chat</a><br/>and ask questions!</div>

## Features

Mergin Maps mobile app features touch optimised GUI components based on Qt Quick (based on QGIS core library).

* Mapping components - map canvas, GPS position, scale bar, markers
* Support for capturing of new geometries
* Display and editing of feature forms
* Built-in service for [storing and synchronising data](https://github.com/MerginMaps/server)
* Translated to several [languages](https://explore.transifex.com/lutra-consulting/mergin-maps-input/)

<div><a href="https://merginmaps.com/product"><img src="https://raw.githubusercontent.com/MerginMaps/.github/main/images/mm_app-1-800x600.jpg" alt="Picture showcasing Mergin Maps mobile features like collaboration, recording geospatial data."></a></div>

## Documentation

Read more about the app [https://merginmaps.com/docs](https://merginmaps.com/docs)

## Developers

To setup your development environment, read [INSTALL](./INSTALL.md)

New subproject 'gallery' app is used to develop/design all UI components, used in the Mergin Maps app

### Code conventions

To learn about our code conventions, please see the [code conventions](./docs/code_convention.md) file.

## Privacy policy
Read more about the app privacy policy [here](https://merginmaps.com/docs/reference/privacy/)
