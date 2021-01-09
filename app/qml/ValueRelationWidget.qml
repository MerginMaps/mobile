import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  signal widgetClosed()

  property alias handler: valueRelationHandler

  QtObject {
    id: valueRelationHandler

    // pointer to widget from qgis feature form
    property var itemWidget: null

    property var getTypeOfWidget: function getTypeOfWidget( widget, valueRelationModel ) {
      if ( widget.allowMultipleValues || valueRelationModel.featuresCount > 4 )
        return "textfield"
      return "combobox"
    }

    property var valueRelationOpened: function valueRelationOpened( widget, valueRelationModel ) {
      itemWidget = widget

      let selectedFeatures = itemWidget.getCurrentValueAsFeatureId()
      if ( Array.isArray( selectedFeatures ) ) {
        selectedFeatures = selectedFeatures.map( id => Number(id) ) // ids can be of string type, convert them to number
      }

      valueRelationLayoutStack.push(componentValueRelationPage, {
                                      featuresModel: valueRelationModel,
                                      pageTitle: itemWidget.fieldName,
                                      allowMultiselect: itemWidget.allowMultipleValues,
                                      selectedFeatures: selectedFeatures
                                    })
    }

    function featureSelected( featureIds ) {
      itemWidget.setValue( featureIds )
    }
  }

  StackView {
    // this stackview can be moved to FeatureForm when we will create multiple instances of feature form
    id: valueRelationLayoutStack
    anchors.fill: parent
    focus: true

    Keys.onReleased: {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;
        closeValueRelationPage()
      }
    }
  }

  function closeValueRelationPage() {
    valueRelationWidget.widgetClosed()
    valueRelationLayoutStack.clear()
  }

  id: valueRelationWidget
  anchors.fill: parent

  Component {
    id: componentValueRelationPage

    BrowseDataFeaturesPanel {
      id: valueRelationPage
      anchors.fill: parent

      onBackButtonClicked: {
        deactivateSearch()
        closeValueRelationPage()
      }

      onFeatureClicked: {
        valueRelationHandler.featureSelected( featureIds )
        deactivateSearch()
        closeValueRelationPage()
      }

      onSearchTextChanged: {
        featuresModel.searchExpression = text
      }

      Keys.onReleased: {
        if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
          event.accepted = true;
          deactivateSearch()
          closeValueRelationPage()
        }
      }
    }
  }
}
