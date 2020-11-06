import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  signal widgetClosed()

  property alias handler: valueRelationHandler

  QtObject {
    id: valueRelationHandler

    // pointer to widget from qgis feature form
    property var itemWidget: null

    property var valueRelationOpened: function valueRelationOpened( widget, valueRelationModel ) {
      itemWidget = widget

      if ( valueRelationModel.featuresCount > 4 || itemWidget.allowMultipleValues ) {
        valueRelationPage.visible = true
        valueRelationPage.featuresModel = valueRelationModel
        valueRelationPage.pageTitle = itemWidget.fieldName
        valueRelationPage.forceActiveFocus()
        valueRelationPage.allowMultiselect = itemWidget.allowMultipleValues
      }
      else {
        itemWidget.openCombobox()
      }
    }

    function featureSelected( index ) {
      itemWidget.itemSelected( index )
    }
  }

  function closeValueRelationPage() {
    valueRelationPage.visible = false
    valueRelationPage.deactivateSearch()
    valueRelationWidget.widgetClosed()
  }

  id: valueRelationWidget
  anchors.fill: parent

  BrowseDataFeaturesPanel {
    id: valueRelationPage
    visible: false
    anchors.fill: parent

    onBackButtonClicked: {
      closeValueRelationPage()
    }

    onFeatureClicked: {
      if ( typeof featureIds === "object" )
        featureIds = Object.values( featureIds )

      console.log( featureIds )
//      valueRelationHandler.featureSelected( featureIds )
      closeValueRelationPage()
    }

    onSearchTextChanged: {
      featuresModel.searchExpression = text
    }

    Keys.onReleased: {
      if ( valueRelationPage.visible && ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) ) {
        event.accepted = true;
        closeValueRelationPage()
      }
    }
  }
}
