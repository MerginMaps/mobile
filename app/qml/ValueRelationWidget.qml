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

      valueRelationLayoutStack.push(componentValueRelationPage, {
                                      featuresModel: valueRelationModel,
                                      pageTitle: itemWidget.fieldName,
                                      allowMultiselect: itemWidget.allowMultipleValues,
                                      preSelectedFeatures: itemWidget.getCurrentValueAsFeatureId()
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
