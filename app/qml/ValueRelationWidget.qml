import QtQuick 2.0
import QtQuick.Controls 2.12

Item {

  property alias handler: valueRelationHandler

  QtObject {
    id: valueRelationHandler

    // pointer to widget from qgis feature form
    property var itemWidget: null

    property var valueRelationOpened: function valueRelationOpened( widget, valueRelationModel ) {
      itemWidget = widget
      valueRelationPage.visible = true
      valueRelationPage.featuresModel = valueRelationModel
    }

    function featureSelected( index ) {
      console.log(index)
      itemWidget.itemSelected( index )
    }

  }

  id: valueRelationWidget
  anchors.fill: parent

  BrowseDataFeaturesPanel {
    id: valueRelationPage
    visible: false
    anchors.fill: parent

    pageTitle: "Choose a type"
    onBackButtonClicked: {
      valueRelationPage.visible = false
    }

    onFeatureClicked: {
      valueRelationHandler.featureSelected( featureId )
      valueRelationPage.visible = false
    }
  }
}
