/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"

Page {
  id: root

  signal backClicked
  signal continueClicked

  function uncheck(except, isChecked, items) {
    if (!isChecked)
      return;

    items.forEach( function(item) {
      if (item !== except) {
        item.checked = false
      }
    } )
  }

  function uncheckAllPrimary(except, isChecked) {

    var items = [
      searchSource,
      blogSource,
      mouthSource,
      qgisSource,
      applicationStoreSource,
      teacherSource,
      conferenceSource,
      socialMediaSource,
      otherSource
    ]

    uncheck(except, isChecked, items)
  }

  function uncheckAllSocialMedia(except, isChecked) {
    if (!isChecked)
      return;

    var items = [
      youtubeSocialMediaSource,
      twitterSocialMediaSource,
      facebookSocialMediaSource,
      linkedinSocialMediaSource,
      mastodonSocialMediaSource,
      redditSocialMediaSource,
    ]

    uncheck(except, isChecked, items)
  }

  Column {
    MMHeader {
      headerTitle: qsTr("How did you learn about us")
      step: 2

      onBackClicked: root.backClicked()
    }

    MMIconCheckBoxHorizontal {
      id: searchSource
      sourceIcon: __style.searchIcon
      text: qsTr("Search engine (Google, ...)")
      onToggled: uncheckAllPrimary(searchSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: blogSource
      sourceIcon: __style.termsIcon
      text: qsTr("Blog")
      onToggled: uncheckAllPrimary(blogSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: mouthSource
      sourceIcon: __style.mouthIcon
      text: qsTr("Mouth")
      onToggled: uncheckAllPrimary(mouthSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: qgisSource
      sourceIcon: __style.qgisIcon
      text: qsTr("QGIS website")
      onToggled: uncheckAllPrimary(qgisSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: applicationStoreSource
      sourceIcon: __style.subscriptionsIcon
      text: qsTr("Application store")
      onToggled: uncheckAllPrimary(applicationStoreSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: teacherSource
      sourceIcon: __style.teacherIcon
      text: qsTr("Teacher")
      onToggled: uncheckAllPrimary(teacherSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: conferenceSource
      sourceIcon: __style.briefcaseIcon
      text: qsTr("Conference")
      onToggled: uncheckAllPrimary(conferenceSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: socialMediaSource
      sourceIcon: __style.socialMediaIcon
      text: qsTr("Social media")
      onToggled: uncheckAllPrimary(socialMediaSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: youtubeSocialMediaSource
      visible: socialMediaSource.checked
      sourceIcon: __style.youtubeIcon
      text: qsTr("YouTube")
      small:true
      onToggled: uncheckAllSocialMedia(youtubeSocialMediaSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: twitterSocialMediaSource
      visible: socialMediaSource.checked
      sourceIcon: __style.xTwitterIcon
      text: qsTr("Twitter")
      small:true
      onToggled: uncheckAllSocialMedia(twitterSocialMediaSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: facebookSocialMediaSource
      visible: socialMediaSource.checked
      sourceIcon: __style.facebookIcon
      text: qsTr("Facebook")
      small:true
      onToggled: uncheckAllSocialMedia(facebookSocialMediaSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: linkedinSocialMediaSource
      visible: socialMediaSource.checked
      sourceIcon: __style.linkedinIcon
      text: qsTr("LinkedIn")
      small:true
      onToggled: uncheckAllSocialMedia(linkedinSocialMediaSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: mastodonSocialMediaSource
      visible: socialMediaSource.checked
      sourceIcon: __style.mastodonIcon
      text: qsTr("Mastodon")
      small:true
      onToggled: uncheckAllSocialMedia(mastodonSocialMediaSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: redditSocialMediaSource
      visible: socialMediaSource.checked
      sourceIcon: __style.redditIcon
      text: qsTr("Reddit")
      small:true
      onToggled: uncheckAllSocialMedia(redditSocialMediaSource, checked)
    }

    MMIconCheckBoxHorizontal {
      id: otherSource
      sourceIcon: __style.otherIcon
      text: qsTr("Other")
      onToggled: uncheckAllPrimary(otherSource, checked)
    }

    MMInput {
      id: otherSourceDescription
      visible: otherSource.checked
      title: qsTr("Source")
      placeholderText: qsTr("Please specify the source")
    }

    MMButton {
      onClicked: root.continueClicked()
      text: qsTr("Continue")
    }
  }
}
